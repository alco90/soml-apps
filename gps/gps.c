#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#define USE_OPTS
#include "gps_popt.h"

#define OML_FROM_MAIN
#include "gps_oml.h"

void error(char *msg)
{
    perror(msg);
    exit(0);
}

static int
run(opts_t* opts, oml_mps_t* oml_mps)
{
  int sockfd, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  OmlValueU v[2];
  char buffer[256];
  char* position;
  char comparison[2];
  char lat[16];
  char* longi;
  char* pch;
  int position_of_limit;
  double latitude;
  double longitude;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  server = gethostbyname(opts->address);

  if (server == NULL) {
    fprintf(stderr,"ERROR, no such host '%s'\n", opts->address);
    exit(0);
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
  serv_addr.sin_port = htons(opts->port);

  if (connect(sockfd,&serv_addr,sizeof(serv_addr)) < 0)
    error("ERROR connecting");
  bzero(buffer,sizeof (buffer));

  while(1)
    {
      n = write(sockfd,"p\n",3);
      if (n < 0)
        error("ERROR writing to socket");
      bzero(buffer,sizeof (buffer));
      n = read(sockfd,buffer,sizeof (buffer)-1);
      if (n < 0)
        error("ERROR reading from socket");
      position=strstr(buffer,"=")+1;
      strncpy(comparison, position, 1);
      comparison[1] = '\0';
      pch=strchr(position,' ');
      position_of_limit = pch-position+1;
      if(strcmp(comparison,"?")==0)
        ;
      else{
        strncpy(lat, position, position_of_limit);
        longi = strstr(position, " ")+1;
        lat[position_of_limit] = '\0';
        omlc_set_const_string(v[0], lat);
        omlc_set_const_string(v[1], longi);
        omlc_inject(g_oml_mps->gps_information, v);
      }
      sleep(g_opts->sample_interval);
    }
  return 0;
}

int
main(int argc, const char *argv[])
{
  char *progname = strdup(argv[0]), *p = progname;
  do *p = (*p == '-') ? '_' : *p; while (*p++);
  omlc_init(progname, &argc, argv, NULL);

  // parsing command line arguments
  poptContext optCon = poptGetContext(NULL, argc, argv, options, 0);
  int c;
  while ((c = poptGetNextOpt(optCon)) > 0) {}

  if (g_opts->address == NULL) {
    fprintf(stderr, "ERROR: You must specify the address of the gpsd server.\n");
    exit(EXIT_FAILURE);
  }

  // Initialize measurment points
  oml_register_mps();  // defined in xxx_oml.h
  omlc_start();

  // Do some work
  run(g_opts, g_oml_mps);
  return 0;
}
