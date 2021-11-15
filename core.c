#include "core.h"
#include "private.h"

#include <stdlib.h>

struct unionfs* unionfs_create()
{
  struct unionfs* fs = calloc(1, sizeof(struct unionfs));
  if (fs) {
    fs->config = calloc(1, sizeof(struct ufs_config));
    if (fs->config) {
      fs->config->disk_cache_timeout = 3600;
    } else {
      unionfs_destroy(fs);
      fs = NULL;
    }
  }
  return fs;
}

void unionfs_destroy(struct unionfs* fs)
{
  if (fs && fs->all_disks) {
    for (size_t i = 0; i < fs->disks_count; i++)
      free(fs->all_disks[i].mountpoint);
    free(fs->all_disks);
  }
  free(fs->config);
  free(fs);
}
