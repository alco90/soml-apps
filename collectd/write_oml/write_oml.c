/*
 * OML writer plugin for collectd
 *
 * This plugin hooks into collectd and reports data over OML, creating
 * measurement points on the fly.
 *
 * Author: Max Ott  <max.ott@nicta.com.au>, (C) 2012
 * Author: Olivier Mehani  <olivier.mehani@nicta.com.au>, (C) 2012-2014
 * Author: Christoph Dwertmann <christoph.dwertmann@nicta.com.au>, (C) 2012
 *
 * Copyright 2012-2014 National ICT Australia (NICTA)
 *
 * This software may be used and distributed solely under the terms of
 * the MIT license (License).  You should find a copy of the License in
 * COPYING or at http://opensource.org/licenses/MIT. By downloading or
 * using this software you accept the terms and the liability disclaimer
 * in the License.
 */
#include <string.h>
#include <time.h>
#include <pthread.h>

#include <ocomm/o_log.h>
#include <oml2/omlc.h>

#include "collectd.h"
#include "plugin.h"
#include "common.h"
#include "utils_cache.h"
#include "utils_parse_option.h"

#ifdef HAVE_CONFIG_H
/* Avoid inheritng collectd's definitions and/or warnings */
# undef PACKAGE
# undef PACKAGE_BUGREPORT
# undef PACKAGE_NAME
# undef PACKAGE_STRING
# undef PACKAGE_TARNAME
# undef PACKAGE_VERSION
# undef VERSION
# include "config.h"
#else
# define PACKAGE_STRING __FILE__
#endif

#define LOGPREFIX "oml_writer plugin: "

static const char *config_keys[] = {
  "NodeID",
  "Domain",
  "CollectURI",
  "OMLLogLevel",
  "OMLBufferSize",
};
static int config_keys_num = STATIC_ARRAY_SIZE(config_keys);


typedef struct MPoint {
  char   name[DATA_MAX_NAME_LEN];

  // From value_list_t (plugin.h)
  char     plugin[DATA_MAX_NAME_LEN];
  char     plugin_instance[DATA_MAX_NAME_LEN];
  char     type[DATA_MAX_NAME_LEN];
  char     type_instance[DATA_MAX_NAME_LEN];

  OmlMP*     oml_mp;
  OmlMPDef*  mp_defs;

  pthread_mutex_t mp_lock;
  struct MPoint* next;
} MPoint;

typedef struct {
  char* collect_uri;
  char* domain;
  char* node_id;
  int   loglevel;
  char* buffer_size;

  MPoint* mpoint;  // linked list of mpoint definitions
  int     oml_initialized;

  pthread_mutex_t session_lock;

} Session;

static Session session;

static MPoint*
find_mpoint_struct(const char* name)
{
  MPoint* mp = session.mpoint;

  while (mp) {
    if (strncmp(mp->name, name, DATA_MAX_NAME_LEN) == 0) {
      return mp;
    }
    mp = mp->next;
  }
  return NULL;
}

/** Configure a new measurement point, and add it to the session
 * XXX: This assumes mp has not been inserted into the session.mpoint list
 * otherwise, the mp->mp_lock mutex should be held by the calling function
 */
static void
configure_mpoint(MPoint* mp, const data_set_t *ds, const value_list_t *vl)
{
  int header = 6; /* There are 6 fields by default */
  int ndef = (ds->ds_num + header + 1); /* terminating NULL */
  int i = 0;

  mp->mp_defs = (OmlMPDef*)malloc(ndef * sizeof(OmlMPDef));
  memset(mp->mp_defs, 0, ndef * sizeof(OmlMPDef));

  mp->mp_defs[i].name = "time"; mp->mp_defs[i].param_types = OML_UINT64_VALUE;
  mp->mp_defs[++i].name = "host"; mp->mp_defs[i].param_types = OML_STRING_VALUE;
  mp->mp_defs[++i].name = "plugin"; mp->mp_defs[i].param_types = OML_STRING_VALUE;
  mp->mp_defs[++i].name = "plugin_instance"; mp->mp_defs[i].param_types = OML_STRING_VALUE;
  mp->mp_defs[++i].name = "type"; mp->mp_defs[i].param_types = OML_STRING_VALUE;
  mp->mp_defs[++i].name = "type_instance"; mp->mp_defs[i].param_types = OML_STRING_VALUE;

  for (i = 0; i < ds->ds_num; i++) {
    data_source_t* d = &ds->ds[i];
    OmlMPDef* md = &mp->mp_defs[i + header];
    char* s = (char*)malloc(sizeof(d->name) + 1);
    strncpy(s, d->name, sizeof(d->name));
    md->name = s;
    assert(d->type <= 3);
    switch(d->type) {
      //  see plugin.h
      //    typedef unsigned long long counter_t;
      //    typedef double gauge_t;
      //    typedef int64_t derive_t;
      //    typedef uint64_t absolute_t;
      case 0: md->param_types = OML_INT64_VALUE; break;
      case 1: md->param_types = OML_DOUBLE_VALUE; break;
      case 2: md->param_types = OML_INT64_VALUE; break;
      case 3: md->param_types = OML_UINT64_VALUE; break;
    }
  }
  mp->oml_mp = omlc_add_mp(mp->name, mp->mp_defs);
  INFO(LOGPREFIX "New measurement point %s", mp->name);
}

/** Create a new measurement point, and add it to the session
 * XXX: The session.session_lock mutex should be held by the calling function */
static MPoint*
create_mpoint(const char* name, const data_set_t *ds, const value_list_t *vl)
{
  MPoint* mp;

  // Create MPoint and insert it into session's existing MP chain.
  mp = (MPoint*)malloc(sizeof(MPoint));

  strncpy(mp->name, name, DATA_MAX_NAME_LEN);
  pthread_mutex_init(&mp->mp_lock, /* attr = */NULL);
  configure_mpoint(mp, ds, vl);

  mp->next = session.mpoint;
  session.mpoint = mp;
  return mp;
}

static MPoint*
find_mpoint(const char* name, const data_set_t *ds, const value_list_t *vl)
{
  pthread_mutex_lock(&session.session_lock);
  MPoint* mp = find_mpoint_struct(name);

  if (mp == NULL) {
    mp = create_mpoint(name, ds, vl);
  }
  pthread_mutex_unlock(&session.session_lock);
  return mp;
}

static int
oml_write (const data_set_t *ds, const value_list_t *vl, user_data_t __attribute__((unused)) *user_data)
{
  int header = 6;
  int i = 0;
  OmlValueU v[64];
  MPoint* mp = find_mpoint(ds->type, ds, vl);

  pthread_mutex_lock(&session.session_lock);
  if (mp == NULL || !session.oml_initialized) {
    pthread_mutex_unlock(&session.session_lock);
    return(0);
  }
  pthread_mutex_unlock(&session.session_lock);

  omlc_zero_array(v, 64);

  if (vl->values_len >= 64 - header) {
    ERROR(LOGPREFIX "Can't handle more than 64 values per measurement");
    return(-1);
  }

  DEBUG(LOGPREFIX "New data in %s (%s, %s, %s, %s, %s)",
      mp->name, vl->host, vl->plugin, vl->plugin_instance, vl->type, vl->type_instance);
  omlc_set_uint64(v[0], (uint64_t)vl->time);
  omlc_set_string(v[1], vl->host != NULL ? (char*)vl->host : "");
  omlc_set_string(v[2], vl->plugin != NULL ? (char*)vl->plugin : "");
  omlc_set_string(v[3], vl->plugin_instance != NULL ? (char*)vl->plugin_instance : "");
  omlc_set_string(v[4], vl->type != NULL ? (char*)vl->type : "");
  omlc_set_string(v[5], vl->type_instance != NULL ? (char*)vl->type_instance : "");

  for (; i < ds->ds_num; i++) {
    data_source_t* d = &ds->ds[i];
    value_t* vi = &vl->values[i];

    assert(d->type <= 3);
    switch(d->type) {
      //  see configure_mpoint()
      case 0: omlc_set_int64(v[i + header], vi->counter); break;
      case 1: omlc_set_double(v[i + header], vi->gauge); break;
      case 2: omlc_set_int64(v[i + header], vi->derive); break;
      case 3: omlc_set_uint64(v[i + header], vi->absolute); break;
    }
  }
  pthread_mutex_lock(&mp->mp_lock);
  omlc_inject(mp->oml_mp, v);
  pthread_mutex_unlock(&mp->mp_lock);
  for(i=1;i<header;i++) {
    omlc_reset_string(v[i]);
  }

  return(0);
}

static int
oml_config(const char *key, const char *value)
{
  int ret = 0, tmp;
  char *endptr;

  pthread_mutex_lock(&session.session_lock);
  if (strcasecmp ("NodeID", key) == 0) {
    session.node_id = (char*)malloc(strlen(value) + 1);
    strncpy(session.node_id, value, strlen(value));
    INFO(LOGPREFIX "OML NodeID set to %s\n", value);

  } else if (strcasecmp ("Domain", key) == 0) {
    session.domain = (char*)malloc(strlen(value) + 1);
    strncpy(session.domain, value, strlen(value));
    INFO(LOGPREFIX "OML Domain set to %s\n", value);

  } else if (strcasecmp ("CollectURI", key) == 0) {
    session.collect_uri = (char*)malloc(strlen(value) + 1);
    strncpy(session.collect_uri, value, strlen(value));
    INFO(LOGPREFIX "OML Collection URI set to %s\n", value);

  } else if (strcasecmp ("OMLLogLevel", key) == 0) {
    tmp = strtol(value, &endptr, 10);
    if (endptr == value) {
      WARNING(LOGPREFIX "Cannot parse '%s %s' as a valid LogLevel number\n",
          key, value);
      ret = -1;

    } else {
      session.loglevel = tmp;
      INFO(LOGPREFIX "OML LogLevel set to %d\n", session.loglevel);
    }

  } else if (strcasecmp ("OMLBufferSize", key) == 0) {
    /* Collectd parses ints and reprints them in a different, decimal, format*/
    //NOTICE("%s %s", key, value);
    endptr = strchr(value, '.');
    if (endptr == value) {
      tmp = strlen(value) + 1;
    } else {
      tmp = endptr - value;
    }
    session.buffer_size = (char*)malloc(tmp + 1);
    strncpy(session.buffer_size, value, tmp);
    session.buffer_size[tmp] = 0;
    INFO(LOGPREFIX "OML Buffer Size set to %sB\n", session.buffer_size);

  } else {
    ret = -1;
  }
  pthread_mutex_unlock(&session.session_lock);

  return ret;
}

static void
o_log_collectd(int log_level, const char* format, ...)
{
  va_list va;
  va_start(va, format);
  switch(log_level) {
    case O_LOG_ERROR:
      ERROR(format, va);
      break;
    case O_LOG_WARN:
      WARNING(format, va);
      break;
    case O_LOG_INFO:
      NOTICE(format, va);
      break;
    case O_LOG_DEBUG:
    case O_LOG_DEBUG2:
    case O_LOG_DEBUG3:
    case O_LOG_DEBUG4:
      DEBUG(format, va);
      break;
    default:
      NOTICE(format, va);
      break;
  }
  va_end(va);
}

static int
oml_init(void)
{
  const char* app_name = "collectd";
  const char* argv[] = {"--oml-id", hostname_g, "--oml-domain", "collectd",
    "--oml-collect", "file:-", "--oml-bufsize", "4096"};
  int argc = 8, init = 0;

  OmlValueU v;
  omlc_zero(v);

  NOTICE(LOGPREFIX "" PACKAGE_STRING);

  pthread_mutex_lock(&session.session_lock);
  if (session.node_id != NULL) argv[1] = session.node_id;
  if (session.domain != NULL) argv[3] = session.domain;
  if (session.collect_uri != NULL) argv[5] = session.collect_uri;
  if (session.buffer_size != NULL) argv[7] = session.buffer_size;
  if (!omlc_init(app_name, &argc, argv, o_log_collectd)) {
    if(!omlc_start()) {
      init = 1;
    }
  }
  o_set_log_level(session.loglevel);

  session.oml_initialized = init;

  /* Inject some metadata
   * This has the added bonus of forcing OML to establish the connection before
   * the first oml_write can proceed */
  omlc_set_string(v, PACKAGE_NAME);
  omlc_inject_metadata(NULL, "appname", &v, OML_STRING_VALUE, NULL);
  omlc_set_string(v, PACKAGE_VERSION);
  omlc_inject_metadata(NULL, "version", &v, OML_STRING_VALUE, NULL);
  omlc_reset_string(v);

  pthread_mutex_unlock(&session.session_lock);
  return !1;
}

static int
oml_cleanup(void)
{
  int ret;
  MPoint *mp = session.mpoint, *omp;
  OmlMPDef *md;
  pthread_mutex_lock(&session.session_lock);

  ret = omlc_close();

  while(mp) {
    pthread_mutex_lock(&mp->mp_lock);

    md = mp->mp_defs;

    while(*md->name) {
     free((char*)md->name);
     md++;
    }

    free(mp->mp_defs);
    pthread_mutex_unlock(&mp->mp_lock);
    pthread_mutex_destroy(&mp->mp_lock);

    omp = mp;
    mp = omp->next;
    free(omp);
  }

  if(session.node_id) { free(session.node_id); }
  if(session.domain) { free(session.domain); }
  if(session.collect_uri) { free(session.collect_uri); }
  if(session.buffer_size) { free(session.buffer_size); }

  pthread_mutex_unlock(&session.session_lock);

  pthread_mutex_destroy(&session.session_lock);

  return ret;
}

void
module_register(void)
{
  memset(&session, 0, sizeof(Session));
  pthread_mutex_init(&session.session_lock, /* attr = */NULL);

  plugin_register_config("write_oml", oml_config, config_keys, config_keys_num);
  plugin_register_init("write_oml", oml_init);
  plugin_register_write("write_oml", oml_write, /* user_data = */ NULL);
  plugin_register_shutdown("write_oml", oml_cleanup);
} /* void module_register */

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
 */
