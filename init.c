#include "unityfs.h"
#include "private.h"

#include <stdio.h>
#include <stdlib.h>

#include "core.h"
#include "disk.h"

static void add_disks_from_file(struct unityfs* fs, const char* filename)
{
  FILE* f = fopen(filename, "r");
  if (!f)
    return;

  char* line = NULL;
  size_t len = 0;
  ssize_t nread;

  while ((nread = getline(&line, &len, f)) != -1) {
    if (line[nread-1] == '\n')
      line[nread-1] = '\0';

    ufs_add_disk(fs, line, 0);

    free(line);
    line = NULL;
    len = 0;
  }

  if (len > 0)
    free(line);

  fclose(f);
}

struct unityfs* ufs_init(void)
{
  struct unityfs* fs = unityfs_create();
  add_disks_from_file(fs, "/etc/unityfs.conf");
  return fs;
}

void ufs_shutdown(struct unityfs* fs)
{
  unityfs_destroy(fs);
}
