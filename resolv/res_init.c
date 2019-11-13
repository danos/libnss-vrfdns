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

#include "nss_vrfdns.h"

#define RFC1535 1

#if PACKETSZ > 65536
# define MAXPACKET	PACKETSZ
#else
# define MAXPACKET	65536
#endif

int __vrf_res_vinit(res_state statp, int preinit, char *resconf_path);

unsigned long long int __res_initstamp;

/*
 * Set up default settings.  If the configuration file exist, the values
 * there will have precedence.  Otherwise, the server address is set to
 * INADDR_LOOPBACK and the default domain name comes from gethostname.
 *
 * Return 0 if completes successfully, -1 on error
 */
int
_vrf_res_ninit(res_state statp, char *resconf_path) {
	if (__res_initstamp == 0) {
        ++__res_initstamp;
    }
    return (__vrf_res_vinit(statp, 1, resconf_path));
}

static void
res_setoptions(res_state statp, const char *options, const char *source) {
	const char *cp = options;
	int i;

#ifdef DEBUG
	if (statp->options & RES_DEBUG)
		printf(";; res_setoptions(\"%s\", \"%s\")...\n",
		       options, source);
#endif
	while (*cp) {
		/* skip leading and inner runs of spaces */
		while (*cp == ' ' || *cp == '\t')
			cp++;
		/* search for and process individual options */
		if (!strncmp(cp, "ndots:", sizeof("ndots:") - 1)) {
			i = atoi(cp + sizeof("ndots:") - 1);
			if (i <= RES_MAXNDOTS)
				statp->ndots = i;
			else
				statp->ndots = RES_MAXNDOTS;
#ifdef DEBUG
			if (statp->options & RES_DEBUG)
				printf(";;\tndots=%d\n", statp->ndots);
#endif
		} else if (!strncmp(cp, "timeout:", sizeof("timeout:") - 1)) {
			i = atoi(cp + sizeof("timeout:") - 1);
			if (i <= RES_MAXRETRANS)
				statp->retrans = i;
			else
				statp->retrans = RES_MAXRETRANS;
		} else if (!strncmp(cp, "attempts:", sizeof("attempts:") - 1)){
			i = atoi(cp + sizeof("attempts:") - 1);
			if (i <= RES_MAXRETRY)
				statp->retry = i;
			else
				statp->retry = RES_MAXRETRY;
		} else if (!strncmp(cp, "debug", sizeof("debug") - 1)) {
#ifdef DEBUG
			if (!(statp->options & RES_DEBUG)) {
				printf(";; res_setoptions(\"%s\", \"%s\")..\n",
				       options, source);
				statp->options |= RES_DEBUG;
			}
			printf(";;\tdebug\n");
#endif
		} else {
		  static const struct
		  {
		    char str[22];
		    uint8_t len;
		    uint8_t clear;
		    unsigned long int flag;
		  } options[] = {
#define STRnLEN(str) str, sizeof (str) - 1
		    { STRnLEN ("inet6"), 0, RES_USE_INET6 },
#ifdef RES_USEBSTRING
		    { STRnLEN ("ip6-bytestring"), 0, RES_USEBSTRING },
#endif
#ifdef RES_NOIP6DOTINT
		    { STRnLEN ("no-ip6-dotint"), 0, RES_NOIP6DOTINT },
		    { STRnLEN ("ip6-dotint"), 1, ~RES_NOIP6DOTINT },
#endif
		    { STRnLEN ("rotate"), 0, RES_ROTATE },
		    { STRnLEN ("no-check-names"), 0, RES_NOCHECKNAME },
		    { STRnLEN ("edns0"), 0, RES_USE_EDNS0 },
		    { STRnLEN ("single-request-reopen"), 0, RES_SNGLKUPREOP },
		    { STRnLEN ("single-request"), 0, RES_SNGLKUP },
		    { STRnLEN ("no_tld_query"), 0, RES_NOTLDQUERY },
		    { STRnLEN ("no-tld-query"), 0, RES_NOTLDQUERY },
		    { STRnLEN ("use-vc"), 0, RES_USEVC }
		  };
#define noptions (sizeof (options) / sizeof (options[0]))
		  int i;
		  for (i = 0; i < noptions; ++i)
		    if (strncmp (cp, options[i].str, options[i].len) == 0)
		      {
			if (options[i].clear)
			  statp->options &= options[i].flag;
			else
			  statp->options |= options[i].flag;
			break;
		      }
		  if (i == noptions) {
		    /* XXX - print a warning here? */
		  }
		}
		/* skip to next run of spaces */
		while (*cp && *cp != ' ' && *cp != '\t')
			cp++;
	}
}

int
__vrf_res_vinit(res_state statp, int preinit, char *resconf_path) {
	FILE *fp;
	char *cp, **pp;
	int n;
	char buf[BUFSIZ];
	int nserv = 0;    /* number of nameservers read from file */
	int have_serv6 = 0;
	int haveenv = 0;
	int havesearch = 0;
	int nsort = 0;
	char *net;
#ifdef HAVE_RES_INITSTAMP
	statp->_u._ext.initstamp = __res_initstamp;
#endif

      	if (!preinit) {
		statp->retrans = RES_TIMEOUT;
		statp->retry = RES_DFLRETRY;
		statp->options = RES_DEFAULT;
		statp->id = res_randomid();
	}

	statp->nscount = 0;
	statp->defdname[0] = '\0';
	statp->ndots = 1;
	statp->pfcode = 0;
	statp->_vcsock = -1;
	statp->_flags = 0;
#ifdef HAVE_RES_QHOOK
	statp->qhook = NULL;
	statp->rhook = NULL;
#endif
	statp->_u._ext.nscount = 0;
	for (n = 0; n < MAXNS; n++)
	    statp->_u._ext.nsaddrs[n] = NULL;

	/* Allow user to override the local domain definition */
	if ((cp = getenv("LOCALDOMAIN")) != NULL) {
		(void)strncpy(statp->defdname, cp, sizeof(statp->defdname) - 1);
		statp->defdname[sizeof(statp->defdname) - 1] = '\0';
		haveenv++;

		/*
		 * Set search list to be blank-separated strings
		 * from rest of env value.  Permits users of LOCALDOMAIN
		 * to still have a search list, and anyone to set the
		 * one that they want to use as an individual (even more
		 * important now that the rfc1535 stuff restricts searches)
		 */
		cp = statp->defdname;
		pp = statp->dnsrch;
		*pp++ = cp;
		for (n = 0; *cp && pp < statp->dnsrch + MAXDNSRCH; cp++) {
			if (*cp == '\n')	/* silly backwards compat */
				break;
			else if (*cp == ' ' || *cp == '\t') {
				*cp = 0;
				n = 1;
			} else if (n) {
				*pp++ = cp;
				n = 0;
				havesearch = 1;
			}
		}
		/* null terminate last domain if there are excess */
		while (*cp != '\0' && *cp != ' ' && *cp != '\t' && *cp != '\n')
			cp++;
		*cp = '\0';
		*pp++ = 0;
	}

#define	MATCH(line, name) \
	(!strncmp(line, name, sizeof(name) - 1) && \
	(line[sizeof(name) - 1] == ' ' || \
	 line[sizeof(name) - 1] == '\t'))

	if ((fp = fopen(resconf_path, "r")) != NULL) {
	    /* No threads use this stream.  */
   
	    __fsetlocking (fp, FSETLOCKING_BYCALLER);
	    /* read the config file */
	    while (fgets_unlocked(buf, sizeof(buf), fp) != NULL) {
		/* skip comments */
		if (*buf == ';' || *buf == '#')
			continue;
		/* read default domain name */
		if (MATCH(buf, "domain")) {
		    if (haveenv)	/* skip if have from environ */
			    continue;
		    cp = buf + sizeof("domain") - 1;
		    while (*cp == ' ' || *cp == '\t')
			    cp++;
		    if ((*cp == '\0') || (*cp == '\n'))
			    continue;
		    strncpy(statp->defdname, cp, sizeof(statp->defdname) - 1);
		    statp->defdname[sizeof(statp->defdname) - 1] = '\0';
		    if ((cp = strpbrk(statp->defdname, " \t\n")) != NULL)
			    *cp = '\0';
		    havesearch = 0;
		    continue;
		}

		/* set search list */
		if (MATCH(buf, "search")) {
		    if (haveenv)	/* skip if have from environ */
			    continue;
		    cp = buf + sizeof("search") - 1;
		    while (*cp == ' ' || *cp == '\t')
			    cp++;
		    if ((*cp == '\0') || (*cp == '\n'))
			    continue;
		    strncpy(statp->defdname, cp, sizeof(statp->defdname) - 1);
		    statp->defdname[sizeof(statp->defdname) - 1] = '\0';
		    if ((cp = strchr(statp->defdname, '\n')) != NULL)
			    *cp = '\0';
		    /*
		     * Set search list to be blank-separated strings
		     * on rest of line.
		     */
		    cp = statp->defdname;
		    pp = statp->dnsrch;
		    *pp++ = cp;
		    for (n = 0; *cp && pp < statp->dnsrch + MAXDNSRCH; cp++) {
			    if (*cp == ' ' || *cp == '\t') {
				    *cp = 0;
				    n = 1;
			    } else if (n) {
				    *pp++ = cp;
				    n = 0;
			    }
		    }
		    /* null terminate last domain if there are excess */
		    while (*cp != '\0' && *cp != ' ' && *cp != '\t')
			    cp++;
		    *cp = '\0';
		    *pp++ = 0;
		    havesearch = 1;
		    continue;
		}

		/* read nameservers to query */
		if (MATCH(buf, "nameserver") && nserv < MAXNS) {
		    struct in_addr a;

		    cp = buf + sizeof("nameserver") - 1;
		    while (*cp == ' ' || *cp == '\t')
			cp++;
		    if ((*cp != '\0') && (*cp != '\n')
			&& inet_aton(cp, &a)) {
			statp->nsaddr_list[nserv].sin_addr = a;
			statp->nsaddr_list[nserv].sin_family = AF_INET;
			statp->nsaddr_list[nserv].sin_port =
				htons(NAMESERVER_PORT);
			nserv++;
		    } else {

			struct in6_addr a6;
			char *el;
			if ((el = strpbrk(cp, " \t\n")) != NULL)
			    *el = '\0';
			if ((el = strchr(cp, SCOPE_DELIMITER)) != NULL)
			    *el = '\0';
			if ((*cp != '\0') &&
			    (inet_pton(AF_INET6, cp, &a6) > 0)) {
			    struct sockaddr_in6 *sa6;

			    sa6 = malloc(sizeof(*sa6));
			    if (sa6 != NULL) {
				sa6->sin6_family = AF_INET6;
				sa6->sin6_port = htons(NAMESERVER_PORT);
				sa6->sin6_flowinfo = 0;
				sa6->sin6_addr = a6;

				if (__builtin_expect (el == NULL, 1))
				    sa6->sin6_scope_id = 0;
				else {
				    int try_numericscope = 1;
				    if (IN6_IS_ADDR_LINKLOCAL (&a6)
					|| IN6_IS_ADDR_MC_LINKLOCAL (&a6)) {
					sa6->sin6_scope_id
					  = if_nametoindex (el + 1);
					if (sa6->sin6_scope_id != 0)
					    try_numericscope = 0;
				    }

				    if (try_numericscope) {
					char *end;
					sa6->sin6_scope_id
					  = (uint32_t) strtoul (el + 1, &end,
								10);
					if (*end != '\0')
					    sa6->sin6_scope_id = 0;
				    }
				}

				statp->nsaddr_list[nserv].sin_family = 0;
				statp->_u._ext.nsaddrs[nserv] = sa6;
				statp->_u._ext.nssocks[nserv] = -1;
				have_serv6 = 1;
				nserv++;
			    }
			}
		    }
		    continue;
		}
        if (MATCH(buf, "sortlist")) {
		    struct in_addr a;

		    cp = buf + sizeof("sortlist") - 1;
		    while (nsort < MAXRESOLVSORT) {
			while (*cp == ' ' || *cp == '\t')
			    cp++;
			if (*cp == '\0' || *cp == '\n' || *cp == ';')
			    break;
			net = cp;
			while (*cp && !ISSORTMASK(*cp) && *cp != ';' &&
			       isascii(*cp) && !isspace(*cp))
				cp++;
			n = *cp;
			*cp = 0;
			if (inet_aton(net, &a)) {
			    statp->sort_list[nsort].addr = a;
			    if (ISSORTMASK(n)) {
				*cp++ = n;
				net = cp;
				while (*cp && *cp != ';' &&
					isascii(*cp) && !isspace(*cp))
				    cp++;
				n = *cp;
				*cp = 0;
				if (inet_aton(net, &a)) {
				    statp->sort_list[nsort].mask = a.s_addr;
				} else {
				    statp->sort_list[nsort].mask =
					net_mask(statp->sort_list[nsort].addr);
				}
			    } else {
				statp->sort_list[nsort].mask =
				    net_mask(statp->sort_list[nsort].addr);
			    }
			    nsort++;
			}
			*cp = n;
		    }
		    continue;
		}
		if (MATCH(buf, "options")) {
		    res_setoptions(statp, buf + sizeof("options") - 1, "conf");
		    continue;
		}
	    }
	    statp->nscount = nserv;
	    if (have_serv6) {
		/* We try IPv6 servers again.  */
		statp->ipv6_unavail = false;
	    }
	    statp->nsort = nsort;
	    (void) fclose(fp);
	}
	if (__builtin_expect(statp->nscount == 0, 0)) {
	    statp->nsaddr.sin_addr = inet_makeaddr(IN_LOOPBACKNET, 1);
	    statp->nsaddr.sin_family = AF_INET;
	    statp->nsaddr.sin_port = htons(NAMESERVER_PORT);
	    statp->nscount = 1;
	}
	if (statp->defdname[0] == 0 &&
	    gethostname(buf, sizeof(statp->defdname) - 1) == 0 &&
	    (cp = strchr(buf, '.')) != NULL)
		strcpy(statp->defdname, cp + 1);

	/* find components of local domain that might be searched */
	if (havesearch == 0) {
		pp = statp->dnsrch;
		*pp++ = statp->defdname;
		*pp = NULL;

	}
	if ((cp = getenv("RES_OPTIONS")) != NULL)
		res_setoptions(statp, cp, "env");
	statp->options |= RES_INIT;
        return (0);
}
