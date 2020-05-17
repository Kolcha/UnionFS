#ifndef CONTEXT_H
#define CONTEXT_H

#include <sys/types.h>

struct fuse_context;

struct process_context {
  mode_t umask;
};

void change_process_context(struct fuse_context* fctx, struct process_context* pctx);
void restore_process_context(struct process_context* pctx);

#endif // CONTEXT_H
