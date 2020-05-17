#include "context.h"

#include <stdlib.h>

#include <unistd.h>

#include <fuse.h>

#include "unityfs.h"

struct global_data* global_data_create(const struct init_data* init_data)
{
  struct global_data* gdata = calloc(1, sizeof(struct global_data));
  if (gdata) {
    gdata->fs = ufs_init(init_data->mountpoint);
  }
  return gdata;
}

void global_data_destroy(struct global_data* global_data)
{
  ufs_shutdown(global_data->fs);
  free(global_data);
}

inline struct global_data* to_global_data(struct fuse_context* fctx)
{
  return (struct global_data*)fctx->private_data;
}

void change_process_context(struct fuse_context* fctx, struct process_context* pctx)
{
  setegid(fctx->gid);
  seteuid(fctx->uid);
  pctx->umask = umask(fctx->umask);
}

void restore_process_context(struct process_context* pctx)
{
  setegid(getgid());
  seteuid(getuid());
  umask(pctx->umask);
}
