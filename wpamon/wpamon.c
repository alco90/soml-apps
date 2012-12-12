/*
 * OML application reporting event from wpa_supplicant
 *
 * This application connect to wpa_supplicant's control socket and reports
 * Wi-Fi events.
 *
 * Author: François Hoguet  <françois.hoguet@nicta.com.au>, (C) 2012
 * Author: Olivier Mehani  <olivier.mehani@nicta.com.au>, (C) 2012
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
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <time.h>
#include <oml2/omlc.h>

#define SOCK_PATH "/var/run/wpa_supplicant/eth1"
#define LOCAL_PATH "/tmp/awesome_socket"
// Known MAC@
#define HOME_MAC "00:0b:6b:0a:83:4a"
#define FOR1_MAC "06:0b:6b:0a:83:4a"
#define FOR2_MAC "0a:0b:6b:0a:83:4a"

int s;

typedef struct {
	OmlMP* network_event;
} oml_mps_t;

static OmlMPDef oml_network_event_def[] = {
	{"event", OML_STRING_VALUE},
	{"network", OML_STRING_VALUE},
	{"tv_sec", OML_UINT32_VALUE},
	{"tv_usec", OML_UINT32_VALUE},
	{NULL, (OmlValueT)0}
};

static oml_mps_t g_oml_mps_storage;
oml_mps_t* g_oml_mps = &g_oml_mps_storage;

static void oml_register_mps() {
	g_oml_mps->network_event = omlc_add_mp("network_event", oml_network_event_def);
}

void sigfun(int sig) {
	printf("catching signal ctrl+c, will quit\n");
	close(s);
	if(remove(LOCAL_PATH) == -1) {
		perror("Error removing socket file");
	}
	printf("Socket closed\n");
	omlc_close();
	exit(0);
}

int main(int argc, char *argv[]) {
    int t, len, loclen;
    struct sockaddr_un local, remote;
    char str[100], temp[100] = "\0";
	char *found;
	char* OMLevent;
	char* OMLnetwork = "homenet";
	OmlValueU v_network_event[4];
	struct timeval tv;

	//(void) signal(SIGINT, sigfun);

	// just for fun
	printf("+-------------------------------------+\n");
	printf("| Welcome to the OML spy and reporter |\n");
	printf("|     Featuring : wpa_supplicant      |\n");
	printf("+-------------------------------------+\n\n");

	omlc_init("wpa_suppl", &argc, argv, NULL);
	oml_register_mps();
	omlc_start();

	(void) signal(SIGINT, sigfun);

    if ((s = socket(PF_UNIX, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, LOCAL_PATH);
    loclen = strlen(local.sun_path) + sizeof(local.sun_family);
    if(bind(s, (struct sockaddr *) &local, sizeof(local)) < 0) {
	perror("bind");
	exit(1);
    }

    printf("Trying to connect...\n");

    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(s, (struct sockaddr *)&remote, len) == -1) {
        perror("connect");
        exit(1);
    }
    
    if(send(s, "ATTACH", 6, 0) < 0) {
	printf("erorr in send\n");
    }

    printf("Connected.\n");

    for(;;) {
        if ((t=recv(s, str, 100, 0)) > 0) {
			gettimeofday(&tv, NULL);
            str[t] = '\0';
			if(strcmp(temp, str) != 0) {
				// Detection disctonnect event
				found = strstr(str, "EVENT-DISCONNECTED");
				if(found != NULL) {
					printf("**** Found DISCONNECTED EVENT ****\n");
					omlc_set_string(v_network_event[0], "Disconnected");
					omlc_set_string(v_network_event[1], OMLnetwork);
					omlc_set_uint32(v_network_event[2], tv.tv_sec);
					omlc_set_uint32(v_network_event[3], tv.tv_usec);
					omlc_inject(g_oml_mps->network_event, v_network_event);
					printf("<3 <3 <3 OML Inject Done <3 <3 <3\n");
				}
				// Detection connect event
				found = strstr(str, "EVENT-CONNECTED");
				if(found != NULL) {
					printf("**** Found CONNECTED EVENT ****\n");
					omlc_set_string(v_network_event[0], "Connected");
					if(strstr(str, HOME_MAC) != NULL) {
						OMLnetwork = "homenet";
					} else if(strstr(str, FOR1_MAC) != NULL) {
						OMLnetwork = "theone";
					} else if(strstr(str, FOR2_MAC) != NULL) {
						OMLnetwork = "thetwo";
					} else {
						OMLnetwork = "Unknown";
					} 
					omlc_set_string(v_network_event[1], OMLnetwork);
					omlc_set_uint32(v_network_event[2], tv.tv_sec);
					omlc_set_uint32(v_network_event[3], tv.tv_usec);
					omlc_inject(g_oml_mps->network_event, v_network_event);
					printf("<3 <3 <3 OML Inject Done <3 <3 <3\n");
				}
			}
			strcpy(temp, str);
        } else {
	    printf("Nothing recieved\n");
            if (t < 0) perror("recv");
            else printf("Server closed connection\n");
            exit(1);
        }
    }

    close(s);
	omlc_close();

    return 0;
}
