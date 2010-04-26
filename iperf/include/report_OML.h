/*
 * Reporter based on the OML library.
 * Copyright (c) 2010, Nicta Olivier Mehani <olivier.mehani@nicta.com.au>
 * All rights reserved.
 *
 * $Id: bsdnotice.ab.c 371 2008-05-30 03:45:32Z shtrom $
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of Nicta nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef REPORT_OML_H
#define REPORT_OML_H

/* Needed for inclusion in C++ headers for OML_init() and OML_set_measurepoints() */
#ifdef __cplusplus
extern "C" {
#endif

int OML_init(int *argc, const char** argv);
int OML_set_measurepoints(thread_Settings *mSettings);

void *OML_peer( Connection_Info *stats, int ID);
void OML_stats( Transfer_Info *stats );
void OML_serverstats( Connection_Info *conn, Transfer_Info *stats );

#ifdef __cplusplus
}
#endif

#endif // REPORT_OML_H
