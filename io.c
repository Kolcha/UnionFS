#include "unityfs.h"
#include "private.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>

int ufs_readlink(struct unityfs* fs, const char* path, char* buf, size_t sz)
{
  for (struct ufs_disk* disk = fs->all_disks; disk != fs->all_disks + fs->disks_count; ++disk) {
    char* real_path = get_real_path(disk, path);

    errno = 0;
    struct stat st_buf;

    if (lstat(real_path, &st_buf) == 0 && S_ISLNK(st_buf.st_mode)) {
      ssize_t lsz = readlink(real_path, buf, sz);
      if (lsz >= 0)
        buf[(size_t)lsz < sz - 1 ? (size_t)lsz : sz - 1] = '\0';
      free(real_path);
      break;
    }

    free(real_path);
  }
  return -errno;
}

int ufs_mkdir(struct unityfs* fs, const char* path, mode_t mode)
{
  char* real_path = new_real_path(fs, path);
  int res = mkdir(real_path, mode);
  free(real_path);
  return res == 0 ? 0 : -errno;
}

int ufs_remove(struct unityfs* fs, const char* path)
{
  for (struct ufs_disk* disk = fs->all_disks; disk != fs->all_disks + fs->disks_count; ++disk) {
    char* real_path = get_real_path(disk, path);

    errno = 0;
    int res = unlink(real_path);

    free(real_path);

    if (res == 0 || errno != ENOENT)
      break;
  }
  return -errno;
}

int ufs_rmdir(struct unityfs* fs, const char* path)
{
  bool dir_removed = false;

  for (struct ufs_disk* disk = fs->all_disks; disk != fs->all_disks + fs->disks_count; ++disk) {
    char* real_path = get_real_path(disk, path);

    if (rmdir(real_path) == 0)
      dir_removed = true;

    free(real_path);

    if (!dir_removed && errno != ENOENT)
      break;
  }

  return dir_removed ? 0 : -errno;
}

int ufs_truncate(struct unityfs* fs, const char* path, off_t offset)
{
  for (struct ufs_disk* disk = fs->all_disks; disk != fs->all_disks + fs->disks_count; ++disk) {
    char* real_path = get_real_path(disk, path);

    errno = 0;
    int res = truncate(real_path, offset);

    free(real_path);

    if (res == 0 || errno != ENOENT)
      break;
  }
  return -errno;
}

int ufs_ftruncate(struct unityfs* fs, ufs_fd_t fd, off_t offset)
{
  (void) fs;
  return ftruncate(fd, offset) == 0 ? 0 : -errno;
}

int ufs_open(struct unityfs* fs, const char* path, int flags, ufs_fd_t* fd)
{
  for (struct ufs_disk* disk = fs->all_disks; disk != fs->all_disks + fs->disks_count; ++disk) {
    char* real_path = get_real_path(disk, path);

    errno = 0;
    *fd = open(real_path, flags);

    free(real_path);

    if (*fd > 0 || errno != ENOENT)
      break;
  }
  return -errno;
}

int ufs_open3(struct unityfs* fs, const char* path, int flags, mode_t mode, ufs_fd_t* fd)
{
  char* real_path = new_real_path(fs, path);
  *fd = open(real_path, flags, mode);
  free(real_path);
  return *fd > 0 ? 0 : -errno;
}

ssize_t ufs_read(struct unityfs* fs, ufs_fd_t fd, off_t offset, size_t size, void* buf)
{
  (void) fs;
  return pread(fd, buf, size, offset);
}

ssize_t ufs_write(struct unityfs* fs, ufs_fd_t fd, off_t offset, size_t size, const void* data)
{
  (void) fs;
  return pwrite(fd, data, size, offset);
}

int ufs_close(struct unityfs* fs, ufs_fd_t fd)
{
  (void) fs;
  return close(fd) == 0 ? 0 : -errno;
}

int ufs_fsync(struct unityfs* fs, ufs_fd_t fd, int sync_metadata)
{
  (void) fs;
  int(*func)(int) = sync_metadata ? &fsync : &fdatasync;
  return (*func)(fd) == 0 ? 0 : -errno;
}
