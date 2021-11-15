#include "unionfs.h"
#include "private.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/xattr.h>

int ufs_setxattr(struct unionfs* fs, const char* path, const char* name, const void* value, size_t size, int flags)
{
  bool attr_set = false;

  for (struct ufs_disk* disk = fs->all_disks; disk != fs->all_disks + fs->disks_count; ++disk) {
    char* real_path = get_real_path(disk, path);

    if (lsetxattr(real_path, name, value, size, flags) == 0)
      attr_set = true;

    free(real_path);

    if (!attr_set && errno != ENOENT)
      break;
  }

  return attr_set ? 0 : -errno;
}

int ufs_fsetxattr(struct unionfs* fs, ufs_fd_t fd, const char* name, const void* value, size_t size, int flags)
{
  (void) fs;
  return fsetxattr(fd, name, value, size, flags) == 0 ? 0 : -errno;
}

ssize_t ufs_getxattr(struct unionfs* fs, const char* path, const char* name, void* value, size_t size)
{
  ssize_t res = 0;
  for (struct ufs_disk* disk = fs->all_disks; disk != fs->all_disks + fs->disks_count; ++disk) {
    char* real_path = get_real_path(disk, path);

    errno = 0;
    res = lgetxattr(real_path, name, value, size);

    free(real_path);

    if (res < 0 && errno != ENOENT) {
      res = -errno;
      break;
    }

    if (res >= 0)
      break;
  }
  return res;
}

ssize_t ufs_fgetxattr(struct unionfs* fs, ufs_fd_t fd, const char* name, void* value, size_t size)
{
  (void) fs;
  ssize_t res = fgetxattr(fd, name, value, size);
  return res >= 0 ? res : -errno;
}

ssize_t ufs_listxattr(struct unionfs* fs, const char* path, char* list, size_t size)
{
  ssize_t res = 0;
  for (struct ufs_disk* disk = fs->all_disks; disk != fs->all_disks + fs->disks_count; ++disk) {
    char* real_path = get_real_path(disk, path);

    errno = 0;
    res = llistxattr(real_path, list, size);

    free(real_path);

    if (res < 0 && errno != ENOENT) {
      res = -errno;
      break;
    }

    if (res >= 0)
      break;
  }
  return res;
}

ssize_t ufs_flistxattr(struct unionfs* fs, ufs_fd_t fd, char* list, size_t size)
{
  (void) fs;
  ssize_t res = flistxattr(fd, list, size);
  return res >= 0 ? res : -errno;
}

int ufs_removexattr(struct unionfs* fs, const char* path, const char* name)
{
  bool attr_removed = false;

  for (struct ufs_disk* disk = fs->all_disks; disk != fs->all_disks + fs->disks_count; ++disk) {
    char* real_path = get_real_path(disk, path);

    if (lremovexattr(real_path, name) == 0)
      attr_removed = true;

    free(real_path);

    if (!attr_removed && errno != ENOENT)
      break;
  }

  return attr_removed ? 0 : -errno;
}

int ufs_fremovexattr(struct unionfs* fs, ufs_fd_t fd, const char* name)
{
  (void) fs;
  return fremovexattr(fd, name) == 0 ? 0 : -errno;
}
