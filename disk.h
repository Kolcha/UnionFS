#ifndef UNITYFS_DISK_H
#define UNITYFS_DISK_H

struct unityfs;

int ufs_add_disk(struct unityfs* fs, const char* mountpoint, unsigned int flags);

#endif // UNITYFS_DISK_H
