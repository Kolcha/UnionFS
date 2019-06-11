bin_PROGRAMS = unityfs
unityfs_SOURCES =	\
	attrs.c		\
	config_file.c	\
	config_file.h	\
	core.c		\
	core.h		\
	disk.c		\
	disk.h		\
	init.c		\
	io.c		\
	main2.c		\
	ordered_set.c	\
	ordered_set.h	\
	private.c	\
	private.h	\
	readdir.c	\
	unityfs.h	\
	xattrs.c
unityfs_CPPFLAGS = -I$(srcdir) -DFUSE_USE_VERSION=26 $(fuse_CFLAGS)
unityfs_LDADD = $(fuse_LIBS)

systemdunitsdir = $(libdir)/systemd/system
dist_systemdunits_DATA = $(srcdir)/unityfs.service
