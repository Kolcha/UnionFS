#ifndef UNITYFS_DISK_H
#define UNITYFS_DISK_H

struct unityfs;

/** exclude disk from shared folder targets list */
#define UFS_DISK_NO_SHARED_WRITES           0x00000001

int ufs_add_disk(struct unityfs* fs, const char* mountpoint, unsigned int flags);

#endif // UNITYFS_DISK_H
