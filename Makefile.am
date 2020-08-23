bin_PROGRAMS = unityfs
unityfs_SOURCES =	\
	attrs.c		\
	config_file.c	\
	config_file.h	\
	context.c	\
	context.h	\
	core.c		\
	core.h		\
	disk.c		\
	disk.h		\
	init.c		\
	io.c		\
	links.c		\
	ordered_set.c	\
	ordered_set.h	\
	private.c	\
	private.h	\
	readdir.c	\
	unityfs.h	\
	xattrs.c
unityfs_CPPFLAGS = $(fuse_CFLAGS)
unityfs_LDADD = $(fuse_LIBS)

if use_fuse3
unityfs_SOURCES += main.c
fuse_api_version = 32
else
unityfs_SOURCES += main2.c
fuse_api_version = 26
endif
unityfs_CPPFLAGS += -DFUSE_USE_VERSION=$(fuse_api_version)
