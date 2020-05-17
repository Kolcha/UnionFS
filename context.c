#include "context.h"

#include <unistd.h>

#include <fuse.h>

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
