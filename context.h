#ifndef CONTEXT_H
#define CONTEXT_H

#include <sys/types.h>

struct fuse_context;
struct unionfs;

struct init_data {
  char* mountpoint;
};

struct global_data {
  struct unionfs* fs;
  pthread_mutex_t context_mutex;
};

struct global_data* global_data_create(const struct init_data* init_data);
void global_data_destroy(struct global_data* global_data);

struct global_data* to_global_data(struct fuse_context* fctx);

static inline struct unionfs* get_unionfs(struct fuse_context* fctx)
{
  return to_global_data(fctx)->fs;
}

struct process_context {
  mode_t umask;
};

void change_process_context(struct fuse_context* fctx, struct process_context* pctx);
void restore_process_context(struct fuse_context* fctx, struct process_context* pctx);

#endif // CONTEXT_H
