#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <fuse.h>
#include <fuse_lowlevel.h>

#include "context.h"
#include "unityfs.h"

static int f3_ufs_getattr(const char* path, struct stat* stbuf, struct fuse_file_info* fi)
{
  struct unityfs* fs = fuse_get_context()->private_data;
  return fi && fi->fh > 0 ? ufs_fgetattr(fs, (ufs_fd_t)fi->fh, stbuf) : ufs_getattr(fs, path, stbuf);
}

static int f3_ufs_readlink(const char* path, char* buf, size_t bufsize)
{
  struct unityfs* fs = fuse_get_context()->private_data;
  return ufs_readlink(fs, path, buf, bufsize);
}

static int f3_ufs_mkdir(const char* path, mode_t mode)
{
  struct fuse_context* fctx = fuse_get_context();
  struct unityfs* fs = fctx->private_data;
  struct process_context pctx;
  change_process_context(fctx, &pctx);
  int res = ufs_mkdir(fs, path, mode);
  restore_process_context(&pctx);
  return res;
}

static int f3_ufs_unlink(const char* path)
{
  struct unityfs* fs = fuse_get_context()->private_data;
  return ufs_remove(fs, path);
}

static int f3_ufs_rmdir(const char* path)
{
  struct unityfs* fs = fuse_get_context()->private_data;
  return ufs_rmdir(fs, path);
}

static int f3_ufs_symlink(const char* target, const char* link_path)
{
  struct fuse_context* fctx = fuse_get_context();
  struct unityfs* fs = fctx->private_data;
  struct process_context pctx;
  change_process_context(fctx, &pctx);
  int res = ufs_symlink(fs, target, link_path);
  restore_process_context(&pctx);
  return res;
}

static int f3_ufs_chmod(const char* path, mode_t mode, struct fuse_file_info* fi)
{
  struct unityfs* fs = fuse_get_context()->private_data;
  return fi && fi->fh > 0 ? ufs_fchmod(fs, (ufs_fd_t)fi->fh, mode) : ufs_chmod(fs, path, mode);
}

static int f3_ufs_chown(const char* path, uid_t uid, gid_t gid, struct fuse_file_info* fi)
{
  struct unityfs* fs = fuse_get_context()->private_data;
  return fi && fi->fh > 0 ? ufs_fchown(fs, (ufs_fd_t)fi->fh, uid, gid) : ufs_chown(fs, path, uid, gid);
}

static int f3_ufs_truncate(const char* path, off_t offset, struct fuse_file_info* fi)
{
  struct unityfs* fs = fuse_get_context()->private_data;
  return fi && fi->fh > 0 ? ufs_ftruncate(fs, (ufs_fd_t)fi->fh, offset) : ufs_truncate(fs, path, offset);
}

static int f3_ufs_open(const char* path, struct fuse_file_info* fi)
{
  struct fuse_context* fctx = fuse_get_context();
  struct unityfs* fs = fctx->private_data;
  struct process_context pctx;
  change_process_context(fctx, &pctx);
  int res = ufs_open(fs, path, fi->flags, (ufs_fd_t*)&fi->fh);
  restore_process_context(&pctx);
  return res;
}

static int f3_ufs_read(const char* path, char* buffer, size_t size, off_t offset,
                       struct fuse_file_info* fi)
{
  (void) path;
  struct unityfs* fs = fuse_get_context()->private_data;
  return (int)ufs_read(fs, (ufs_fd_t)fi->fh, offset, size, buffer);
}


static int f3_ufs_write(const char* path, const char* buffer, size_t size, off_t offset,
                        struct fuse_file_info* fi)
{
  (void) path;
  struct unityfs* fs = fuse_get_context()->private_data;
  return (int)ufs_write(fs, (ufs_fd_t)fi->fh, offset, size, buffer);
}

static int f3_ufs_statfs(const char* path, struct statvfs* stbuf)
{
  struct unityfs* fs = fuse_get_context()->private_data;
  return ufs_statfs(fs, path, stbuf);
}

static int f3_ufs_release(const char* path, struct fuse_file_info* fi)
{
  (void) path;
  struct unityfs* fs = fuse_get_context()->private_data;
  return ufs_close(fs, (ufs_fd_t)fi->fh);
}

static int f3_ufs_fsync(const char* path, int datasync, struct fuse_file_info* fi)
{
  (void) path;
  struct unityfs* fs = fuse_get_context()->private_data;
  return ufs_fsync(fs, (ufs_fd_t)fi->fh, !datasync);
}

static int f3_ufs_setxattr(const char* path, const char* name, const char* value, size_t size, int flags)
{
  struct unityfs* fs = fuse_get_context()->private_data;
  return ufs_setxattr(fs, path, name, value, size, flags);
}

static int f3_ufs_getxattr(const char* path, const char* name, char* value, size_t size)
{
  struct unityfs* fs = fuse_get_context()->private_data;
  return (int)ufs_getxattr(fs, path, name, value, size);
}

static int f3_ufs_listxattr(const char* path, char* list, size_t size)
{
  struct unityfs* fs = fuse_get_context()->private_data;
  return (int)ufs_listxattr(fs, path, list, size);
}

static int f3_ufs_removexattr(const char* path, const char* name)
{
  struct unityfs* fs = fuse_get_context()->private_data;
  return ufs_removexattr(fs, path, name);
}

static int f3_ufs_opendir(const char* path, struct fuse_file_info* fi)
{
  struct unityfs* fs = fuse_get_context()->private_data;
  return ufs_opendir(fs, path, (ufs_dir_t**)&fi->fh);
}

static int f3_ufs_readdir(const char* path, void* buf, fuse_fill_dir_t filler,
                          off_t offset, struct fuse_file_info* fi,
                          enum fuse_readdir_flags flags)
{
  (void) path;
  (void) offset;

  struct unityfs* fs = fuse_get_context()->private_data;
  struct dirent* entry;
  int res = 0;
  if (flags & FUSE_READDIR_PLUS) {
    struct stat stbuf;
    while ((res = ufs_readdir_plus(fs, (ufs_dir_t*)fi->fh, &entry, &stbuf)) == 0 && entry) {
      filler(buf, entry->d_name, &stbuf, 0, FUSE_FILL_DIR_PLUS);
    }
  } else {
    while ((res = ufs_readdir(fs, (ufs_dir_t*)fi->fh, &entry)) == 0 && entry) {
      filler(buf, entry->d_name, NULL, 0, 0);
    }
  }
  return res;
}

static int f3_ufs_releasedir(const char* path, struct fuse_file_info* fi)
{
  (void) path;
  struct unityfs* fs = fuse_get_context()->private_data;
  return ufs_closedir(fs, (ufs_dir_t*)fi->fh);
}

static void* f3_ufs_init(struct fuse_conn_info* conn, struct fuse_config* cfg)
{
  (void) conn;
  (void) cfg;
  return ufs_init(fuse_get_context()->private_data);
}

static void f3_ufs_destroy(void* private_data)
{
  ufs_shutdown(private_data);
}

static int f3_ufs_create(const char* path, mode_t mode, struct fuse_file_info* fi)
{
  struct fuse_context* fctx = fuse_get_context();
  struct unityfs* fs = fctx->private_data;
  struct process_context pctx;
  change_process_context(fctx, &pctx);
  int res = ufs_open3(fs, path, fi->flags, mode, (ufs_fd_t*)&fi->fh);
  restore_process_context(&pctx);
  return res;
}

static int f3_ufs_utimens(const char* path, const struct timespec tv[2],
                          struct fuse_file_info* fi)
{
  struct unityfs* fs = fuse_get_context()->private_data;
  return fi && fi->fh > 0 ? ufs_futimens(fs, (ufs_fd_t)fi->fh, tv) : ufs_utimens(fs, path, tv);
}

static struct fuse_operations f3_ufs_oper = {
  .getattr    = f3_ufs_getattr,
  .readlink   = f3_ufs_readlink,
/*  .mknod      = f3_ufs_mknod, */
  .mkdir      = f3_ufs_mkdir,
  .unlink     = f3_ufs_unlink,
  .rmdir      = f3_ufs_rmdir,
  .symlink    = f3_ufs_symlink,
/*  .rename     = f3_ufs_rename, */
/*  .link       = f3_ufs_link, */
  .chmod      = f3_ufs_chmod,
  .chown      = f3_ufs_chown,
  .truncate   = f3_ufs_truncate,
  .open       = f3_ufs_open,
  .read       = f3_ufs_read,
  .write      = f3_ufs_write,
  .statfs     = f3_ufs_statfs,
  .release    = f3_ufs_release,
  .fsync      = f3_ufs_fsync,
  .setxattr   = f3_ufs_setxattr,
  .getxattr   = f3_ufs_getxattr,
  .listxattr  = f3_ufs_listxattr,
  .removexattr= f3_ufs_removexattr,
  .opendir    = f3_ufs_opendir,
  .readdir    = f3_ufs_readdir,
  .releasedir = f3_ufs_releasedir,
  .init       = f3_ufs_init,
  .destroy    = f3_ufs_destroy,
  .create     = f3_ufs_create,
  .utimens    = f3_ufs_utimens,
};

int main(int argc, char* argv[])
{
  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
  struct fuse_cmdline_opts opts;

  if (fuse_parse_cmdline(&args, &opts) != 0)
    return 1;

  umask(0);

  int res = fuse_main(argc, argv, &f3_ufs_oper, opts.mountpoint);

  free(opts.mountpoint);
  fuse_opt_free_args(&args);

  return res;
}
