#include "disk.h"
#include "private.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/statvfs.h>

static uint64_t get_next_inode_offset(struct ufs_disk* last_disk)
{
  return last_disk->inode_offset + last_disk->max_inodes;
}

int ufs_add_disk(struct unityfs* fs, const char* mountpoint, unsigned int flags)
{
  struct statvfs st_buf;
  if (statvfs(mountpoint, &st_buf) != 0)
    return -errno;

  struct stat fst_buf;
  if (stat(mountpoint, &fst_buf) != 0)
    return -errno;

  fs->disks_count++;
  fs->all_disks = realloc(fs->all_disks, fs->disks_count * sizeof(struct ufs_disk));

  struct ufs_disk* new_disk = fs->all_disks + fs->disks_count - 1;
  new_disk->mountpoint = strdup(mountpoint);
  new_disk->device_id = fst_buf.st_dev;
  new_disk->max_inodes = st_buf.f_files;
  new_disk->inode_offset = new_disk == fs->all_disks ? 0 : get_next_inode_offset(new_disk - 1);
  new_disk->mount_flags = st_buf.f_flag;
  new_disk->custom_flags = flags;
  new_disk->reserved = 0xFFFFFFFF;

  return 0;
}
