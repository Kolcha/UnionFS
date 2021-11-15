#ifndef UNIONFS_H
#define UNIONFS_H

#include <dirent.h>

#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>

struct unionfs;

typedef int ufs_fd_t;

struct ufs_dir;
typedef struct ufs_dir ufs_dir_t;

struct unionfs* ufs_init(const char* mountpoint);
void ufs_shutdown(struct unionfs* fs);

int ufs_getattr(struct unionfs* fs, const char* path, struct stat* stbuf);
int ufs_fgetattr(struct unionfs* fs, ufs_fd_t fd, struct stat* stbuf);
int ufs_readlink(struct unionfs* fs, const char* path, char* buf, size_t sz);
/*int ufs_mknod(struct unionfs* fs, const char* path, const mode_t mode, const dev_t dev);*/
int ufs_mkdir(struct unionfs* fs, const char* path, mode_t mode);
int ufs_remove(struct unionfs* fs, const char* path);
int ufs_rmdir(struct unionfs* fs, const char* path);
int ufs_symlink(struct unionfs* fs, const char* target, const char* link_path);
/*int ufs_rename(struct unionfs* fs, const char* old_path, const char* new_path);*/
/*int ufs_link(struct unionfs* fs, const char* target, const char* link_path);*/
int ufs_chmod(struct unionfs* fs, const char* path, mode_t mode);
int ufs_fchmod(struct unionfs* fs, ufs_fd_t fd, mode_t mode);
int ufs_chown(struct unionfs* fs, const char* path, uid_t uid, gid_t gid);
int ufs_fchown(struct unionfs* fs, ufs_fd_t fd, uid_t uid, gid_t gid);
int ufs_truncate(struct unionfs* fs, const char* path, off_t offset);
int ufs_ftruncate(struct unionfs* fs, ufs_fd_t fd, off_t offset);
int ufs_open(struct unionfs* fs, const char* path, int flags, ufs_fd_t* fd);
int ufs_open3(struct unionfs* fs, const char* path, int flags, mode_t mode, ufs_fd_t* fd);
ssize_t ufs_read(struct unionfs* fs, ufs_fd_t fd, off_t offset, size_t size, void* buf);
ssize_t ufs_write(struct unionfs* fs, ufs_fd_t fd, off_t offset, size_t size, const void* data);
int ufs_statfs(struct unionfs* fs, const char* path, struct statvfs* stbuf);
int ufs_close(struct unionfs* fs, ufs_fd_t fd);
int ufs_fsync(struct unionfs* fs, ufs_fd_t fd, int sync_metadata);
int ufs_setxattr(struct unionfs* fs, const char* path, const char* name, const void* value, size_t size, int flags);
int ufs_fsetxattr(struct unionfs* fs, ufs_fd_t fd, const char* name, const void* value, size_t size, int flags);
ssize_t ufs_getxattr(struct unionfs* fs, const char* path, const char* name, void* value, size_t size);
ssize_t ufs_fgetxattr(struct unionfs* fs, ufs_fd_t fd, const char* name, void* value, size_t size);
ssize_t ufs_listxattr(struct unionfs* fs, const char* path, char* list, size_t size);
ssize_t ufs_flistxattr(struct unionfs* fs, ufs_fd_t fd, char* list, size_t size);
int ufs_removexattr(struct unionfs* fs, const char* path, const char* name);
int ufs_fremovexattr(struct unionfs* fs, ufs_fd_t fd, const char* name);
int ufs_opendir(struct unionfs* fs, const char* path, ufs_dir_t** dir);
int ufs_readdir(struct unionfs* fs, ufs_dir_t* dir, struct dirent** entry);
int ufs_readdir_plus(struct unionfs* fs, ufs_dir_t* dir, struct dirent** entry, struct stat* stbuf);
int ufs_closedir(struct unionfs* fs, ufs_dir_t* dir);
int ufs_utimens(struct unionfs* fs, const char* path, const struct timespec times[2]);
int ufs_futimens(struct unionfs* fs, ufs_fd_t fd, const struct timespec times[2]);

#endif // UNIONFS_H
