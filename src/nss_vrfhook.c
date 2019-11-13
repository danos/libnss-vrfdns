/*
 * Copyright (c) 2016 by Brocade Communications Systems, Inc.
 * All rights reserved.
 */

#include "nss_vrfdns.h"

char * get_vrfname(void)
{
    char *vrfname = getenv("VYATTA_VRF_NSS_HOSTS");

    if (!vrfname)
        vrfname = getenv("VYATTA_VRF");

    if (!vrfname)
        return strdup("default");

    return strdup(vrfname);
}


