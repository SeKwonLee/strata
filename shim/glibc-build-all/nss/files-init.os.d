$(common-objpfx)nss/files-init.os: \
 nss_files/files-init.c ../include/stdc-predef.h \
 ../include/libc-symbols.h \
 $(common-objpfx)config.h \
 ../sysdeps/generic/symbol-hacks.h

../include/stdc-predef.h:

../include/libc-symbols.h:

$(common-objpfx)config.h:

../sysdeps/generic/symbol-hacks.h:
