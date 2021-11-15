#include "unionfs.h"
#include "private.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

static bool less_timespec(struct timespec* a, struct timespec* b)
{
  if (a->tv_sec != b->tv_sec)
    return a->tv_sec < b->tv_sec;
  return a->tv_nsec < b->tv_nsec;
}

static struct timespec* max_timespec(struct timespec* a, struct timespec* b)
{
  return less_timespec(a, b) ? a : b;
}

int ufs_getattr(struct unionfs* fs, const char* path, struct stat* stbuf)
{
  bool item_found = false;

  for (struct ufs_disk* disk = fs->all_disks; disk != fs->all_disks + fs->disks_count; ++disk) {
    char* real_path = get_real_path(disk, path);

    struct stat buf;
    int res = lstat(real_path, &buf);

    free(real_path);

    if (res != 0)
      continue;

    if (!item_found) {
      *stbuf = buf;
      stbuf->st_ino = calc_ino(fs, buf.st_dev, buf.st_ino);
      item_found = true;
    } else {
      stbuf->st_nlink += buf.st_nlink - 2;
      stbuf->st_size += buf.st_size;
      stbuf->st_atim = *max_timespec(&stbuf->st_atim, &buf.st_atim);
      stbuf->st_ctim = *max_timespec(&stbuf->st_ctim, &buf.st_ctim);
      stbuf->st_mtim = *max_timespec(&stbuf->st_mtim, &buf.st_mtim);
    }

    if (!S_ISDIR(buf.st_mode))
      break;
  }

  return item_found ? 0 : -errno;
}

int ufs_fgetattr(struct unionfs* fs, ufs_fd_t fd, struct stat* stbuf)
{
  if (fstat(fd, stbuf) != 0)
    return -errno;

  stbuf->st_ino = calc_ino(fs, stbuf->st_dev, stbuf->st_ino);
  return 0;
}

int ufs_chmod(struct unionfs* fs, const char* path, mode_t mode)
{
  bool item_found = false;

  for (struct ufs_disk* disk = fs->all_disks; disk != fs->all_disks + fs->disks_count; ++disk) {
    char* real_path = get_real_path(disk, path);

    if (chmod(real_path, mode) == 0)
      item_found = true;

    free(real_path);

    if (!item_found && errno != ENOENT)
      break;
  }

  return item_found ? 0 : -errno;
}

int ufs_fchmod(struct unionfs* fs, ufs_fd_t fd, mode_t mode)
{
  (void) fs;
  return fchmod(fd, mode) == 0 ? 0 : -errno;
}

int ufs_chown(struct unionfs* fs, const char* path, uid_t uid, gid_t gid)
{
  bool item_found = false;

  for (struct ufs_disk* disk = fs->all_disks; disk != fs->all_disks + fs->disks_count; ++disk) {
    char* real_path = get_real_path(disk, path);

    if (chown(real_path, uid, gid) == 0)
      item_found = true;

    free(real_path);

    if (!item_found && errno != ENOENT)
      break;
  }

  return item_found ? 0 : -errno;
}

int ufs_fchown(struct unionfs* fs, ufs_fd_t fd, uid_t uid, gid_t gid)
{
  (void) fs;
  return fchown(fd, uid, gid) == 0 ? 0 : -errno;
}

int ufs_statfs(struct unionfs* fs, const char* path, struct statvfs* stbuf)
{
  memset(stbuf, 0, sizeof(struct statvfs));
  stbuf->f_bsize = 4096;
  stbuf->f_frsize = stbuf->f_bsize;

  errno = 0;

  for (struct ufs_disk* disk = fs->all_disks; disk != fs->all_disks + fs->disks_count; ++disk) {
    char* real_path = get_real_path(disk, path);

    struct statvfs buf;
    if (statvfs(real_path, &buf) == 0) {
      stbuf->f_blocks += buf.f_blocks * buf.f_frsize / stbuf->f_frsize;
      stbuf->f_bfree += buf.f_bfree * buf.f_frsize / stbuf->f_frsize;
      stbuf->f_bavail += buf.f_bavail * buf.f_frsize / stbuf->f_frsize;
      stbuf->f_files += buf.f_files;
      stbuf->f_ffree += buf.f_ffree;
      stbuf->f_favail += buf.f_favail;
      stbuf->f_namemax = stbuf->f_namemax < buf.f_namemax ? stbuf->f_namemax : buf.f_namemax;
    }

    free(real_path);
  }

  return -errno;
}

int ufs_utimens(struct unionfs* fs, const char* path, const struct timespec times[2])
{
  bool item_found = false;

  for (struct ufs_disk* disk = fs->all_disks; disk != fs->all_disks + fs->disks_count; ++disk) {
    char* real_path = get_real_path(disk, path);

    if (utimensat(-1, real_path, times, 0) == 0)
      item_found = true;

    free(real_path);

    if (!item_found && errno != ENOENT)
      break;
  }

  return item_found ? 0 : -errno;
}

int ufs_futimens(struct unionfs* fs, ufs_fd_t fd, const struct timespec times[2])
{
  (void) fs;
  return futimens(fd, times) == 0 ? 0 : -errno;
}
