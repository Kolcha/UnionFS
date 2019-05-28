bin_PROGRAMS = unityfs
unityfs_SOURCES =	\
	attrs.c		\
	core.c		\
	core.h		\
	disk.c		\
	disk.h		\
	init.c		\
	io.c		\
	main.c		\
	ordered_set.c	\
	ordered_set.h	\
	private.c	\
	private.h	\
	readdir.c	\
	unityfs.h
unityfs_CPPFLAGS = -I$(srcdir) -DFUSE_USE_VERSION=32 $(fuse3_CFLAGS)
unityfs_LDADD = $(fuse3_LIBS)

systemdunitsdir = $(libdir)/systemd/system
dist_systemdunits_DATA = $(srcdir)/unityfs.service
