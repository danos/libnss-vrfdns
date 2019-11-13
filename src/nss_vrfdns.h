#ifndef NSS_VRFDNS_H
#define NSS_VRFDNS_H

#include <stdio.h>

/* Must define _LIBC after other headers have been read otherwise we may get
 * some errors: 'Applications may not define the macro _LIBC'
 */
#define _LIBC   1
#include <resolv.h>

/* nss_vrfhook.c */
char * get_vrfname(void);

/* resolv/res_init.c */
int _vrf_res_ninit(res_state statp, char *resconf_path);
int __vrf_res_vinit(res_state statp, int preinit, char *resconf_path);

#endif /* NSS_VRFDNS_H */
