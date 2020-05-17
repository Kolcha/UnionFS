#include "context.h"

#include <stdlib.h>

#include <pthread.h>
#include <unistd.h>

#include <fuse.h>

#include "unityfs.h"

static inline pthread_mutex_t* get_context_mutex(struct fuse_context* fctx)
{
  struct global_data* gdata = fctx->private_data;
  return &gdata->context_mutex;
}

struct global_data* global_data_create(const struct init_data* init_data)
{
  struct global_data* gdata = calloc(1, sizeof(struct global_data));
  if (gdata) {
    pthread_mutex_init(&gdata->context_mutex, NULL);
    gdata->fs = ufs_init(init_data->mountpoint);
  }
  return gdata;
}

void global_data_destroy(struct global_data* global_data)
{
  ufs_shutdown(global_data->fs);
  pthread_mutex_destroy(&global_data->context_mutex);
  free(global_data);
}

inline struct global_data* to_global_data(struct fuse_context* fctx)
{
  return (struct global_data*)fctx->private_data;
}

void change_process_context(struct fuse_context* fctx, struct process_context* pctx)
{
  pthread_mutex_lock(get_context_mutex(fctx));
  setegid(fctx->gid);
  seteuid(fctx->uid);
  pctx->umask = umask(fctx->umask);
}

void restore_process_context(struct fuse_context* fctx, struct process_context* pctx)
{
  setegid(getgid());
  seteuid(getuid());
  umask(pctx->umask);
  pthread_mutex_unlock(get_context_mutex(fctx));
}
