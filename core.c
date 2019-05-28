#include "core.h"
#include "private.h"

#include <stdlib.h>

struct unityfs* unityfs_create()
{
  return calloc(1, sizeof(struct unityfs));
}

void unityfs_destroy(struct unityfs* fs)
{
  if (fs->all_disks) {
    for (size_t i = 0; i < fs->disks_count; i++)
      free(fs->all_disks[i].mountpoint);
    free(fs->all_disks);
  }
  free(fs);
}
