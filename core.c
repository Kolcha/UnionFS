#include "core.h"
#include "private.h"

#include <stdlib.h>

struct unityfs* unityfs_create()
{
  struct unityfs* fs = calloc(1, sizeof(struct unityfs));
  if (fs) {
    fs->config = calloc(1, sizeof(struct ufs_config));
    if (fs->config) {
      fs->config->disk_cache_timeout = 3600;
    } else {
      unityfs_destroy(fs);
      fs = NULL;
    }
  }
  return fs;
}

void unityfs_destroy(struct unityfs* fs)
{
  if (fs && fs->all_disks) {
    for (size_t i = 0; i < fs->disks_count; i++)
      free(fs->all_disks[i].mountpoint);
    free(fs->all_disks);
  }
  free(fs->config);
  free(fs);
}
