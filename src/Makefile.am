CLEANFILES = *~

EXTRA_DIST = libnss_vrfdns.map libnss_vrffiles.map

AM_CFLAGS = -D__USE_MISC=1 -D_REENTRANT=1 -DNIS_INTERNAL=1
AM_CPPFLAGS = -I$(srcdir) -I$(srcdir)/../resolv

lib_LTLIBRARIES = libnss_vrfdns.la libnss_vrffiles.la

noinst_HEADERS = nss_vrfdns.h

libnss_vrfdns_la_CFLAGS = -lnss_dns -lresolv

libnss_vrfdns_la_LDFLAGS = -version-info @version@ \
        -Wl,--version-script=$(srcdir)/libnss_vrfdns.map \
	-lnss_dns -lresolv

libnss_vrfdns_la_SOURCES = nss_vrfdns.c nss_vrfhook.c ../resolv/res_init.c

libnss_vrffiles_la_LDFLAGS = -version-info @version@ \
        -Wl,--version-script=$(srcdir)/libnss_vrffiles.map \
	-lnss_dns -lresolv

libnss_vrffiles_la_SOURCES = vrffiles-hosts.c files-have_o_cloexec.c nss_vrfhook.c

install-exec-hook:
	echo "dest: $(DESTDIR) $(libdir)"
	rm -f $(DESTDIR)$(libdir)/libnss_vrfdns.la
	rm -f $(DESTDIR)$(libdir)/libnss_vrffiles.la
