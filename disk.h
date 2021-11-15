#ifndef UNIONFS_DISK_H
#define UNIONFS_DISK_H

struct unionfs;

/** exclude disk from shared folder targets list */
#define UFS_DISK_NO_SHARED_WRITES           0x00000001

int ufs_add_disk(struct unionfs* fs, const char* mountpoint, unsigned int flags);

#endif // UNIONFS_DISK_H
