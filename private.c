#include "private.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <libgen.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

#include "disk.h"

char* get_real_path(struct ufs_disk* disk, const char* path)
{
  char* real_path = calloc(strlen(disk->mountpoint) + strlen(path) + 1, sizeof(char));
  strcpy(real_path, disk->mountpoint);
  strcat(real_path, path);
  return real_path;
}

static int is_path_exists(const char* path)
{
  struct stat stbuf;
  return lstat(path, &stbuf) == 0;
}

static uint64_t get_disk_free_space(const char* mountpoint)
{
  uint64_t free_space = 0;
  struct statvfs stbuf;
  if (statvfs(mountpoint, &stbuf) == 0)
    free_space = stbuf.f_bavail * stbuf.f_frsize;
  return free_space;
}

static struct ufs_disk* select_disk_by_free_space(struct ufs_disk* disks[])
{
  struct ufs_disk* selected_disk = disks[0];
  uint64_t max_free_space = get_disk_free_space(selected_disk->mountpoint);

  for (struct ufs_disk** disk_iter = disks + 1; *disk_iter; ++disk_iter) {
    uint64_t free_space = get_disk_free_space((*disk_iter)->mountpoint);
    if (free_space > max_free_space) {
      max_free_space = free_space;
      selected_disk = *disk_iter;
    }
  }

  return selected_disk;
}

static struct ufs_disk* select_disk(struct unityfs* fs, const char* path)
{
  /* find all disks which contain parent path */
  struct ufs_disk** parent_disks = calloc(fs->disks_count + 1, sizeof(struct ufs_disk*));
  struct ufs_disk** parent_disk_iter = parent_disks;

  for (struct ufs_disk* disk = fs->all_disks; disk != fs->all_disks + fs->disks_count; ++disk) {
    /* skip read-only filesystems */
    if (disk->mount_flags & ST_RDONLY || disk->custom_flags & UFS_DISK_NO_WRITES)
      continue;

    char* real_path = get_real_path(disk, path);

    if (is_path_exists(dirname(real_path)))
      *parent_disk_iter++ = disk;

    free(real_path);
  }
  /* consider "nothing found" as "found everywhere", this will lead to exact error later */
  if (parent_disk_iter == parent_disks)
    for (struct ufs_disk* disk = fs->all_disks; disk != fs->all_disks + fs->disks_count; ++disk)
      *parent_disk_iter++ = disk;

  assert(parent_disk_iter != parent_disks);
  assert(parent_disk_iter - parent_disks <= (intptr_t)fs->disks_count);

  struct ufs_disk* selected_disk = NULL;

  time_t now = time(NULL);
  /* if last disk selection happened long time ago - select new disk by free space */
  if (now - fs->last_disk_selection < 3600) {
    assert(fs->last_used_disk);
    /* if last used disk in list with disks contain parent path - select next disk */
    parent_disk_iter = parent_disks;
    while (*parent_disk_iter) {
      if (*parent_disk_iter == fs->last_used_disk)
        break;
      ++parent_disk_iter;
    }

    if (*parent_disk_iter) {
      ++parent_disk_iter;
      if (!*parent_disk_iter)
        parent_disk_iter = parent_disks;
      selected_disk = *parent_disk_iter;
    }
  }

  if (!selected_disk)
    selected_disk = select_disk_by_free_space(parent_disks);

  fs->last_used_disk = selected_disk;
  fs->last_disk_selection = now;

  free(parent_disks);

  return selected_disk;
}

char* new_real_path(struct unityfs* fs, const char* path)
{
  struct ufs_disk* disk = select_disk(fs, path);
  return get_real_path(disk, path);
}

static struct ufs_disk* get_disk_by_device_id(struct unityfs* fs, dev_t id)
{
  for (struct ufs_disk* disk = fs->all_disks; disk != fs->all_disks + fs->disks_count; ++disk)
    if (disk->device_id == id)
      return disk;
  return NULL;
}

ino_t calc_ino(struct unityfs* fs, dev_t dev_id, ino_t orig_ino)
{
  struct ufs_disk* disk = get_disk_by_device_id(fs, dev_id);
  return disk ? disk->inode_offset + orig_ino : orig_ino;
}
