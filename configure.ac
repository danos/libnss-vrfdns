#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_define([VERSION_ID], [m4_esyscmd([
    echo -n `dpkg-parsechangelog -S Version`
    ])])

AC_PREREQ([2.69])
AC_INIT(libnss_vrfdns, VERSION_ID, support@brocade.com)
AM_INIT_AUTOMAKE([foreign])

AC_SUBST(version, [2:0:0])

AC_CONFIG_SRCDIR([src/nss_vrfdns.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
LT_INIT
AC_PROG_MAKE_SET
AC_GNU_SOURCE
AM_PROG_CC_C_O
test -n "$GCC" && WARNFLAGS="-W -Wall -Wbad-function-cast -Wcast-align -Winline -Wnested-externs -Wshadow -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes -Wundef -Werror"

# Checks for libraries.
AC_SEARCH_LIBS([main], [libresolv])

# Checks for header files.
AC_HEADER_RESOLV
AC_CHECK_HEADERS([arpa/inet.h fcntl.h libintl.h limits.h memory.h netdb.h netinet/in.h shadow.h stdint.h stdio_ext.h stdlib.h string.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h syslog.h unistd.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gethostbyaddr gethostbyname gettimeofday inet_ntoa isascii memmove mempcpy memset socket strcasecmp strchr strdup strerror strncasecmp strpbrk strtol strtoul])

AC_CHECK_MEMBER([struct __res_state._u._ext._initstamp], [CPPFLAGS="$CPPFLAGS -DHAVE_RES_INITSTAMP"], [], [[#include <resolv.h>]])
AC_CHECK_MEMBER([struct __res_state.qhook], [CPPFLAGS="$CPPFLAGS -DHAVE_RES_QHOOK"], [], [[#include <resolv.h>]])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
