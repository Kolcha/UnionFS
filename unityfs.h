#ifndef UNITYFS_H
#define UNITYFS_H

#include <dirent.h>

#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>

struct unityfs;

typedef int ufs_fd_t;

struct ufs_dir;
typedef struct ufs_dir ufs_dir_t;

struct unityfs* ufs_init(const char* mountpoint);
void ufs_shutdown(struct unityfs* fs);

int ufs_getattr(struct unityfs* fs, const char* path, struct stat* stbuf);
int ufs_fgetattr(struct unityfs* fs, ufs_fd_t fd, struct stat* stbuf);
int ufs_readlink(struct unityfs* fs, const char* path, char* buf, size_t sz);
/*int ufs_mknod(struct unityfs* fs, const char* path, const mode_t mode, const dev_t dev);*/
int ufs_mkdir(struct unityfs* fs, const char* path, mode_t mode);
int ufs_remove(struct unityfs* fs, const char* path);
int ufs_rmdir(struct unityfs* fs, const char* path);
/*int ufs_symlink(struct unityfs* fs, const char* target, const char* link_path);*/
/*int ufs_rename(struct unityfs* fs, const char* old_path, const char* new_path);*/
/*int ufs_link(struct unityfs* fs, const char* target, const char* link_path);*/
int ufs_chmod(struct unityfs* fs, const char* path, mode_t mode);
int ufs_fchmod(struct unityfs* fs, ufs_fd_t fd, mode_t mode);
int ufs_chown(struct unityfs* fs, const char* path, uid_t uid, gid_t gid);
int ufs_fchown(struct unityfs* fs, ufs_fd_t fd, uid_t uid, gid_t gid);
int ufs_truncate(struct unityfs* fs, const char* path, off_t offset);
int ufs_ftruncate(struct unityfs* fs, ufs_fd_t fd, off_t offset);
int ufs_open(struct unityfs* fs, const char* path, int flags, ufs_fd_t* fd);
int ufs_open3(struct unityfs* fs, const char* path, int flags, mode_t mode, ufs_fd_t* fd);
ssize_t ufs_read(struct unityfs* fs, ufs_fd_t fd, off_t offset, size_t size, void* buf);
ssize_t ufs_write(struct unityfs* fs, ufs_fd_t fd, off_t offset, size_t size, const void* data);
int ufs_statfs(struct unityfs* fs, const char* path, struct statvfs* stbuf);
int ufs_close(struct unityfs* fs, ufs_fd_t fd);
int ufs_fsync(struct unityfs* fs, ufs_fd_t fd, int sync_metadata);
int ufs_setxattr(struct unityfs* fs, const char* path, const char* name, const void* value, size_t size, int flags);
int ufs_fsetxattr(struct unityfs* fs, ufs_fd_t fd, const char* name, const void* value, size_t size, int flags);
ssize_t ufs_getxattr(struct unityfs* fs, const char* path, const char* name, void* value, size_t size);
ssize_t ufs_fgetxattr(struct unityfs* fs, ufs_fd_t fd, const char* name, void* value, size_t size);
ssize_t ufs_listxattr(struct unityfs* fs, const char* path, char* list, size_t size);
ssize_t ufs_flistxattr(struct unityfs* fs, ufs_fd_t fd, char* list, size_t size);
int ufs_removexattr(struct unityfs* fs, const char* path, const char* name);
int ufs_fremovexattr(struct unityfs* fs, ufs_fd_t fd, const char* name);
int ufs_opendir(struct unityfs* fs, const char* path, ufs_dir_t** dir);
int ufs_readdir(struct unityfs* fs, ufs_dir_t* dir, struct dirent** entry);
int ufs_readdir_plus(struct unityfs* fs, ufs_dir_t* dir, struct dirent** entry, struct stat* stbuf);
int ufs_closedir(struct unityfs* fs, ufs_dir_t* dir);
int ufs_utimens(struct unityfs* fs, const char* path, const struct timespec times[2]);
int ufs_futimens(struct unityfs* fs, ufs_fd_t fd, const struct timespec times[2]);

#endif // UNITYFS_H
