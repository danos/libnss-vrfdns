#  Libnss vrfdns Readme 

This document provides an design overview for the libnss-vrfdns module. Getting
to the current code base took several revisions and different design attempts
which are outlined below.


1. lwres
We attempted to write this module using the lwres resolver from the bind9 package,
but realized that it required the used of a daemon to do the actual resolving. So
much for light weight.

2. Extraction of res-vinit from libresolv
For our purposes we need to be able to sepcify a custom resolv.conf path per
resolve. libresolv has this path hardcoded. To get around this we extracted the 
res-vinit() function which populates the res structure. Once struct res is setup,
we call the original libnss-dns nss-dns-gethostbyname3-r.


Other resolvers looked at:
udns -- no activity since Jan 2014?

Programs using the glibc library for host resolution can be hijacked to use our own libnss_vrfdns resolver. 

Areas needing of change:

1. /etc/nsswitch.conf:  This changes the host resolution order to include the new vrfdns nss library. Note, we remove the old dns resolver completely if we detect that a routing instance was configured on the router, otherwise the original glibc dns resolver is used.

if: routing routing-instance blue
```Bash
hosts:          files vrfdns
```
else:
```Bash
hosts:          files dns
```

We add a new libnss shared object to the vRouter, based on glibc/resolver/nss_dns 

\_\_res_ninit() will need to search vrf specific resolv.conf files. These will be found in /run/dns/vrf/$vrfname/resolv.conf. This requires setting _PATH_RESCONF to the correct reslov.conf during _nss_dns_gethostbyname3_r(). We call this new path __PATH_RESCONF2


More info can be found here: https://swnconfluence.brocade.com/display/VROUT/VRF+Support+for+DNS+Low+Level+Design
