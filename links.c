#include "unityfs.h"
#include "private.h"

#include <errno.h>
#include <stdlib.h>

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
