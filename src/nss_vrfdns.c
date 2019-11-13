/* Copyright (C) 1996-2016 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Extended from original form by Ulrich Drepper <drepper@cygnus.com>, 1996.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

/* Parts of this file are plain copies of the file `gethtnamadr.c' from
   the bind package and it has the following copyright.  */

/*
 * ++Copyright++ 1985, 1988, 1993
 * -
 * Copyright (c) 1985, 1988, 1993
 *    The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * -
 * Portions Copyright (c) 1993 by Digital Equipment Corporation.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies, and that
 * the name of Digital Equipment Corporation not be used in advertising or
 * publicity pertaining to distribution of the document or software without
 * specific, written prior permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * -
 * --Copyright--
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <nss.h>
#include <resolv.h>
#include "nss_vrfdns.h"

struct hostent;

extern enum nss_status _nss_dns_gethostbyname_r (const char *name, struct hostent *result,
						 char *buffer, size_t buflen, int *errnop,
						 int *h_errnop);

extern enum nss_status _nss_dns_gethostbyname3_r (const char *name, int af,
                                                  struct hostent *result,
                                                  char *buffer, size_t buflen,
                                                  int *errnop, int *h_errnop,
                                                  int32_t *ttlp, char **canonp);

extern enum nss_status _nss_dns_gethostbyaddr2_r (const void *addr,
                                                  socklen_t len, int af,
                                                  struct hostent *result,
                                                  char *buffer, size_t buflen,
                                                  int *errnop, int *h_errnop,
                                                  int32_t *ttlp);

enum nss_status
_nss_vrfdns_gethostbyname2_r (const char *name, int af, struct hostent *result,
			   char *buffer, size_t buflen, int *errnop,
			   int *h_errnop)
{
  char *vrfname = get_vrfname();
  if (strcmp(vrfname, "default") != 0) {
    char _vrf_resconf_path[1024];
    memset(_vrf_resconf_path, '\0', sizeof(_vrf_resconf_path));
    snprintf(_vrf_resconf_path, sizeof(_vrf_resconf_path), "/run/dns/vrf/%s/resolv.conf", vrfname);
    if (_vrf_res_ninit (&_res, _vrf_resconf_path) == -1) {
        if(vrfname)
            free(vrfname);
        return (-1);
    }

  }

  if(vrfname)
    free(vrfname);

  return _nss_dns_gethostbyname3_r (name, af, result, buffer, buflen, errnop,
				    h_errnop, NULL, NULL);
}

enum nss_status
_nss_vrfdns_gethostbyname_r (const char *name, struct hostent *result,
			  char *buffer, size_t buflen, int *errnop,
			  int *h_errnop)
{
  char *vrfname = get_vrfname();
  enum nss_status status = NSS_STATUS_NOTFOUND;
  if (strcmp(vrfname, "default") == 0) {
    if(vrfname)
        free(vrfname);
    /* Default VRF, don't use VRF resolver (i.e. also uses global _res) */
    return _nss_dns_gethostbyname_r (name, result, buffer, buflen, errnop,
				    h_errnop);
  } else {
    char _vrf_resconf_path[1024];
    memset(_vrf_resconf_path, '\0', sizeof(_vrf_resconf_path));
    snprintf(_vrf_resconf_path, sizeof(_vrf_resconf_path), "/run/dns/vrf/%s/resolv.conf", vrfname);
    if (_vrf_res_ninit (&_res, _vrf_resconf_path) == -1) {
        if(vrfname)
            free(vrfname);
        return (-1);
    }
    if(vrfname)
        free(vrfname);

    if (_res.options & RES_USE_INET6)
        status = _nss_dns_gethostbyname3_r (name, AF_INET6, result, buffer,
                                            buflen, errnop, h_errnop, NULL, NULL);
    if (status == NSS_STATUS_NOTFOUND)
        status = _nss_dns_gethostbyname3_r (name, AF_INET, result, buffer,
                                            buflen, errnop, h_errnop, NULL, NULL);

    return status;
  }
}

enum nss_status
_nss_vrfdns_gethostbyaddr_r (const void *addr, socklen_t len, int af,
			  struct hostent *result, char *buffer, size_t buflen,
			  int *errnop, int *h_errnop)
{
  char *vrfname = get_vrfname();
  if (strcmp(vrfname, "default") != 0) {
    char _vrf_resconf_path[1024];
    memset(_vrf_resconf_path, '\0', sizeof(_vrf_resconf_path));
    snprintf(_vrf_resconf_path, sizeof(_vrf_resconf_path), "/run/dns/vrf/%s/resolv.conf", vrfname);
    if (_vrf_res_ninit (&_res, _vrf_resconf_path) == -1) {
        if(vrfname)
            free(vrfname);
        return (-1);
    }
  }

  if(vrfname)
        free(vrfname);

  return _nss_dns_gethostbyaddr2_r (addr, len, af, result, buffer, buflen,
        				    errnop, h_errnop, NULL);
}

enum nss_status
_nss_vrfdns_gethostbyaddr2_r (const void *addr, socklen_t len, int af,
			   struct hostent *result, char *buffer, size_t buflen,
			   int *errnop, int *h_errnop, int32_t *ttlp)
{
    return _nss_vrfdns_gethostbyaddr_r (addr, len, af, result, buffer,
                                           buflen, errnop, h_errnop);
}
