#ifndef UNITYFS_PRIVATE_H
#define UNITYFS_PRIVATE_H

#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include <sys/types.h>

struct ufs_disk {
  char* mountpoint;
  dev_t device_id;
  uint64_t max_inodes;
  uint64_t inode_offset;
  uint64_t mount_flags;
  uint32_t custom_flags;
  uint32_t reserved;
};

struct unityfs {
  struct ufs_disk* all_disks;
  size_t disks_count;
  time_t last_disk_selection;
  struct ufs_disk* last_used_disk;
};

char* get_real_path(struct ufs_disk* disk, const char* path);
char* new_real_path(struct unityfs* fs, const char* path);

ino_t calc_ino(struct unityfs* fs, dev_t dev_id, ino_t orig_ino);

#endif // UNITYFS_PRIVATE_H
