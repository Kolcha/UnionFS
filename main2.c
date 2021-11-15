#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <fuse.h>

#include "context.h"
#include "unionfs.h"

static int f2_ufs_getattr(const char* path, struct stat* stbuf)
{
  return ufs_getattr(get_unionfs(fuse_get_context()), path, stbuf);
}

static int f2_ufs_readlink(const char* path, char* buf, size_t bufsize)
{
  struct unionfs* fs = get_unionfs(fuse_get_context());
  return ufs_readlink(fs, path, buf, bufsize);
}

static int f2_ufs_mkdir(const char* path, mode_t mode)
{
  struct fuse_context* fctx = fuse_get_context();
  struct unionfs* fs = get_unionfs(fctx);
  struct process_context pctx;
  change_process_context(fctx, &pctx);
  int res = ufs_mkdir(fs, path, mode);
  restore_process_context(fctx, &pctx);
  return res;
}

static int f2_ufs_unlink(const char* path)
{
  struct unionfs* fs = get_unionfs(fuse_get_context());
  return ufs_remove(fs, path);
}

static int f2_ufs_rmdir(const char* path)
{
  struct unionfs* fs = get_unionfs(fuse_get_context());
  return ufs_rmdir(fs, path);
}

static int f2_ufs_symlink(const char* target, const char* link_path)
{
  struct fuse_context* fctx = fuse_get_context();
  struct unionfs* fs = get_unionfs(fctx);
  struct process_context pctx;
  change_process_context(fctx, &pctx);
  int res = ufs_symlink(fs, target, link_path);
  restore_process_context(fctx, &pctx);
  return res;
}

static int f2_ufs_chmod(const char* path, mode_t mode)
{
  return ufs_chmod(get_unionfs(fuse_get_context()), path, mode);
}

static int f2_ufs_chown(const char* path, uid_t uid, gid_t gid)
{
  return ufs_chown(get_unionfs(fuse_get_context()), path, uid, gid);
}

static int f2_ufs_truncate(const char* path, off_t offset)
{
  return ufs_truncate(get_unionfs(fuse_get_context()), path, offset);
}

static int f2_ufs_open(const char* path, struct fuse_file_info* fi)
{
  struct fuse_context* fctx = fuse_get_context();
  struct unionfs* fs = get_unionfs(fctx);
  struct process_context pctx;
  change_process_context(fctx, &pctx);
  int res = ufs_open(fs, path, fi->flags, (ufs_fd_t*)&fi->fh);
  restore_process_context(fctx, &pctx);
  return res;
}

static int f2_ufs_read(const char* path, char* buffer, size_t size, off_t offset,
                       struct fuse_file_info* fi)
{
  (void) path;
  struct unionfs* fs = get_unionfs(fuse_get_context());
  return (int)ufs_read(fs, (ufs_fd_t)fi->fh, offset, size, buffer);
}

static int f2_ufs_write(const char* path, const char* buffer, size_t size, off_t offset,
                        struct fuse_file_info* fi)
{
  (void) path;
  struct unionfs* fs = get_unionfs(fuse_get_context());
  return (int)ufs_write(fs, (ufs_fd_t)fi->fh, offset, size, buffer);
}

static int f2_ufs_statfs(const char* path, struct statvfs* stbuf)
{
  struct unionfs* fs = get_unionfs(fuse_get_context());
  return ufs_statfs(fs, path, stbuf);
}

static int f2_ufs_release(const char* path, struct fuse_file_info* fi)
{
  (void) path;
  struct unionfs* fs = get_unionfs(fuse_get_context());
  return ufs_close(fs, (ufs_fd_t)fi->fh);
}

static int f2_ufs_fsync(const char* path, int datasync, struct fuse_file_info* fi)
{
  (void) path;
  struct unionfs* fs = get_unionfs(fuse_get_context());
  return ufs_fsync(fs, (ufs_fd_t)fi->fh, !datasync);
}

static int f2_ufs_setxattr(const char* path, const char* name, const char* value, size_t size, int flags)
{
  struct unionfs* fs = get_unionfs(fuse_get_context());
  return ufs_setxattr(fs, path, name, value, size, flags);
}

static int f2_ufs_getxattr(const char* path, const char* name, char* value, size_t size)
{
  struct unionfs* fs = get_unionfs(fuse_get_context());
  return (int)ufs_getxattr(fs, path, name, value, size);
}

static int f2_ufs_listxattr(const char* path, char* list, size_t size)
{
  struct unionfs* fs = get_unionfs(fuse_get_context());
  return (int)ufs_listxattr(fs, path, list, size);
}

static int f2_ufs_removexattr(const char* path, const char* name)
{
  struct unionfs* fs = get_unionfs(fuse_get_context());
  return ufs_removexattr(fs, path, name);
}

static int f2_ufs_opendir(const char* path, struct fuse_file_info* fi)
{
  struct unionfs* fs = get_unionfs(fuse_get_context());
  return ufs_opendir(fs, path, (ufs_dir_t**)&fi->fh);
}

static int f2_ufs_readdir(const char* path, void* buf, fuse_fill_dir_t filler,
                          off_t offset, struct fuse_file_info* fi)
{
  (void) path;
  (void) offset;

  struct unionfs* fs = get_unionfs(fuse_get_context());
  struct dirent* entry;
  struct stat stbuf;
  int res = 0;
  while ((res = ufs_readdir_plus(fs, (ufs_dir_t*)fi->fh, &entry, &stbuf)) == 0 && entry) {
    filler(buf, entry->d_name, &stbuf, 0);
  }
  return res;
}

static int f2_ufs_releasedir(const char* path, struct fuse_file_info* fi)
{
  (void) path;
  struct unionfs* fs = get_unionfs(fuse_get_context());
  return ufs_closedir(fs, (ufs_dir_t*)fi->fh);
}

static void* f2_ufs_init(struct fuse_conn_info* conn)
{
  (void) conn;
  return global_data_create(fuse_get_context()->private_data);
}

static void f2_ufs_destroy(void* private_data)
{
  global_data_destroy(private_data);
}

static int f2_ufs_create(const char* path, mode_t mode, struct fuse_file_info* fi)
{
  struct fuse_context* fctx = fuse_get_context();
  struct unionfs* fs = get_unionfs(fctx);
  struct process_context pctx;
  change_process_context(fctx, &pctx);
  int res = ufs_open3(fs, path, fi->flags, mode, (ufs_fd_t*)&fi->fh);
  restore_process_context(fctx, &pctx);
  return res;
}

static int f2_ufs_ftruncate(const char* path, off_t offset, struct fuse_file_info* fi)
{
  (void) path;
  return ufs_ftruncate(get_unionfs(fuse_get_context()), (ufs_fd_t)fi->fh, offset);
}

static int f2_ufs_fgetattr(const char* path, struct stat* stbuf, struct fuse_file_info* fi)
{
  (void) path;
  return ufs_fgetattr(get_unionfs(fuse_get_context()), (ufs_fd_t)fi->fh, stbuf);
}

static int f2_ufs_utimens(const char* path, const struct timespec tv[2])
{
  return ufs_utimens(get_unionfs(fuse_get_context()), path, tv);
}

static struct fuse_operations f2_ufs_oper = {
  .getattr    = f2_ufs_getattr,
  .readlink   = f2_ufs_readlink,
/*  .mknod      = f2_ufs_mknod, */
  .mkdir      = f2_ufs_mkdir,
  .unlink     = f2_ufs_unlink,
  .rmdir      = f2_ufs_rmdir,
  .symlink    = f2_ufs_symlink,
/*  .rename     = f2_ufs_rename, */
/*  .link       = f2_ufs_link, */
  .chmod      = f2_ufs_chmod,
  .chown      = f2_ufs_chown,
  .truncate   = f2_ufs_truncate,
  .open       = f2_ufs_open,
  .read       = f2_ufs_read,
  .write      = f2_ufs_write,
  .statfs     = f2_ufs_statfs,
  .release    = f2_ufs_release,
  .fsync      = f2_ufs_fsync,
  .setxattr   = f2_ufs_setxattr,
  .getxattr   = f2_ufs_getxattr,
  .listxattr  = f2_ufs_listxattr,
  .removexattr= f2_ufs_removexattr,
  .opendir    = f2_ufs_opendir,
  .readdir    = f2_ufs_readdir,
  .releasedir = f2_ufs_releasedir,
  .init       = f2_ufs_init,
  .destroy    = f2_ufs_destroy,
  .create     = f2_ufs_create,
  .ftruncate  = f2_ufs_ftruncate,
  .fgetattr   = f2_ufs_fgetattr,
  .utimens    = f2_ufs_utimens,

  .flag_nullpath_ok = 1,
  .flag_nopath      = 1,

  .flag_utime_omit_ok = 1,
};

int main(int argc, char* argv[])
{
  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
  struct init_data data;

  if (fuse_parse_cmdline(&args, &data.mountpoint, NULL, NULL) == -1)
    return 1;

  umask(0);

  int res = fuse_main(argc, argv, &f2_ufs_oper, &data);

  free(data.mountpoint);
  fuse_opt_free_args(&args);

  return res;
}
