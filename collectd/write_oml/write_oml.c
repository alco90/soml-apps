/*
 * OML writer plugin for collectd
 *
 * This plugin hooks into collectd and reports data over OML, creating
 * measurement points on the fly.
 *
 * Author: Max Ott  <max.ott@nicta.com.au>, (C) 2012
 * Author: Olivier Mehani  <olivier.mehani@nicta.com.au>, (C) 2012
 * Author: Christoph Dwertmann <christoph.dwertmann@nicta.com.au>, (C) 2012
 *
 * Copyright (c) 2012 National ICT Australia (NICTA)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include <string.h>
#include <time.h>
#include <pthread.h>

#include <oml2/omlc.h>
#include <ocomm/o_log.h>

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

static const char *config_keys[] =
{
  "NodeID",
  "Domain",
  "CollectURI",
  "StartupDelay"
};
static int config_keys_num = STATIC_ARRAY_SIZE(config_keys);


typedef struct _mpoint {
  char   name[DATA_MAX_NAME_LEN];

  // From value_list_t (plugin.h)
  char     plugin[DATA_MAX_NAME_LEN];
  char     plugin_instance[DATA_MAX_NAME_LEN];
  char     type[DATA_MAX_NAME_LEN];
  char     type_instance[DATA_MAX_NAME_LEN];

  OmlMP*     oml_mp;
  OmlMPDef*  mp_defs;
  struct _mpoint* next;
} MPoint;

typedef struct {
  char* collect_uri;
  char* domain;
  char* node_id;
  int   startup_delay;

  MPoint* mpoint;  // linked list of mpoint definitions
  int     oml_intialized;
  pthread_mutex_t init_lock;

} Session;

static Session session;
static time_t start_time;

static MPoint*
find_mpoint_struct(
    const char* name
    ) {
  MPoint* mp = session.mpoint;

  while (mp) {
    if (strncmp(mp->name, name, DATA_MAX_NAME_LEN) == 0) {
      return mp;
    }
    mp = mp->next;
  } 
  return NULL;
}

static void
configure_mpoint(
    MPoint* mp,
    const data_set_t *ds,
    const value_list_t *vl
    ) {

  //  strncpy(mp->plugin, vl->plugin, sizeof(mp->plugin));
  //  strncpy(mp->plugin_instance, vl->plugin_instance, sizeof(mp->plugin_instance));
  //  strncpy(mp->type, vl->type, sizeof (mp->type));
  //  strncpy(mp->type_instance, vl->type_instance, sizeof (mp->type_instance));

  mp->mp_defs = (OmlMPDef*)malloc((ds->ds_num + 6 + 1) * sizeof(OmlMPDef));

  int i = 0;
  mp->mp_defs[i].name = "time"; mp->mp_defs[i].param_types = OML_UINT64_VALUE;
  mp->mp_defs[++i].name = "host"; mp->mp_defs[i].param_types = OML_STRING_VALUE;
  mp->mp_defs[++i].name = "plugin"; mp->mp_defs[i].param_types = OML_STRING_VALUE;
  mp->mp_defs[++i].name = "plugin_instance"; mp->mp_defs[i].param_types = OML_STRING_VALUE;
  mp->mp_defs[++i].name = "type"; mp->mp_defs[i].param_types = OML_STRING_VALUE;
  mp->mp_defs[++i].name = "type_instance"; mp->mp_defs[i].param_types = OML_STRING_VALUE;

  int offset = ++i;
  for (i = 0; i < ds->ds_num; i++) {
    data_source_t* d = &ds->ds[i];
    OmlMPDef* md = &mp->mp_defs[i + offset];
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
  // NULL out last one
  OmlMPDef* md = &mp->mp_defs[ds->ds_num + offset];
  md->name = 0; md->param_types = (OmlValueT)0;
  mp->oml_mp = omlc_add_mp(mp->name, mp->mp_defs);
  DEBUG("oml_writer plugin: New measurement point %s", mp->name);
}

static MPoint*
create_mpoint(
    const char* name,
    const data_set_t *ds,
    const value_list_t *vl
    ) {
  MPoint* mp;
  // Create MPoint and insert it into session's existing MP chain.
  mp = (MPoint*)malloc(sizeof(MPoint));
  strncpy(mp->name, name, DATA_MAX_NAME_LEN);
  MPoint* pmp = session.mpoint;
  mp->next = pmp;
  session.mpoint = mp;
  configure_mpoint(mp, ds, vl);
  return mp;
}

static MPoint*
find_mpoint(
    const char* name,
    const data_set_t *ds,
    const value_list_t *vl
    ) {
  MPoint* mp = find_mpoint_struct(name);

  if (mp == NULL) {
    if (session.oml_intialized) {
      ERROR("oml_writer plugin: We assumed that all collectors already checked in, but now we found '%s'", name);
    }
    mp = create_mpoint(name, ds, vl);
  }
  if (! session.oml_intialized) {
    // We are waiting for some time before we commit to a set of
    // reportable measurements.
    //
    time_t now;
    time(&now);
    if ((now - start_time) < session.startup_delay) {
      return NULL;  // let's wait a bit longer
    }
    // OK it's time to commit
    pthread_mutex_lock(&session.init_lock);
    if (! session.oml_intialized) { // just make sure nothing has changed since we aquired the lock
      DEBUG("oml_writer plugin: Starting OML");
      if(omlc_start()==0) {
        session.oml_intialized = 1;
      } else {
        ERROR("oml_writer plugin: Could not initialise OML");
        mp = NULL;
      }
    }
    pthread_mutex_unlock(&session.init_lock);

  }
  return mp;
}

static int
oml_write (
    const data_set_t *ds,
    const value_list_t *vl,
    user_data_t __attribute__((unused)) *user_data
    ) {
  MPoint* mp = find_mpoint(ds->type, ds, vl);
  if (mp == NULL || !session.oml_intialized) return(0);

  OmlValueU v[64];
  int header = 6;

  omlc_zero_array(v, 64);

  if (vl->values_len >= 64 - header) {
    ERROR("oml_writer plugin: Can't handle more than 64 values per measurement");
    return(-1);
  }

  DEBUG("oml_writer plugin: New data in %s (%s, %s, %s, %s, %s)",
      mp->name, vl->host, vl->plugin, vl->plugin_instance, vl->type, vl->type_instance);
  omlc_set_uint64(v[0], (uint64_t)vl->time);
  omlc_set_string(v[1], vl->host != NULL ? (char*)vl->host : "");
  omlc_set_string(v[2], vl->plugin != NULL ? (char*)vl->plugin : "");
  omlc_set_string(v[3], vl->plugin_instance != NULL ? (char*)vl->plugin_instance : "");
  omlc_set_string(v[4], vl->type != NULL ? (char*)vl->type : "");
  omlc_set_string(v[5], vl->type_instance != NULL ? (char*)vl->type_instance : "");

  int i = 0;
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
  omlc_inject(mp->oml_mp, v);
  for(i=1;i<header;i++)
    omlc_reset_string(v[i]);

  return(0);
}

static int
oml_config(
    const char *key,
    const char *value
    ) {
  if (strcasecmp ("NodeID", key) == 0) {
    session.node_id = (char*)malloc(strlen(value) + 1);
    strncpy(session.node_id, value, strlen(value));
  } else if (strcasecmp ("ContextName", key) == 0) {
    session.domain = (char*)malloc(strlen(value) + 1);
    strncpy(session.domain, value, strlen(value));
  } else if (strcasecmp ("CollectURI", key) == 0) {
    session.collect_uri = (char*)malloc(strlen(value) + 1);
    strncpy(session.collect_uri, value, strlen(value));
  } else if (strcasecmp ("StartupDelay", key) == 0) {
    session.startup_delay = atoi(value);
  } else {
    return(-1);
  }
  return(0);
}

static void o_log_collectd(int log_level, const char* format, ...)
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
      INFO(format, va);
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
oml_init(
    void
    ) {
  const char* app_name = "collectd";
  const char* argv[] = {"--oml-id", hostname_g, "--oml-domain", "collectd", "--oml-collect", "file:-"};
  int argc = 6;

  NOTICE("oml_writer plugin: " PACKAGE_STRING);

  if (session.node_id != NULL) argv[1] = session.node_id;
  if (session.domain != NULL) argv[3] = session.domain;
  if (session.collect_uri != NULL) argv[5] = session.collect_uri;
  return omlc_init(app_name, &argc, argv, o_log_collectd);
}

  void
module_register(void)

{
  memset(&session, 0, sizeof(Session));
  time(&start_time);
  session.startup_delay = 10;
  pthread_mutex_init(&session.init_lock, /* attr = */NULL);

  plugin_register_config("write_oml", oml_config, config_keys, config_keys_num);
  plugin_register_init("write_oml", oml_init);
  plugin_register_write("write_oml", oml_write, /* user_data = */ NULL);
} /* void module_register */

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
 */