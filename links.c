#include "unityfs.h"
#include "private.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libgen.h>
#include <unistd.h>

int ufs_readlink(struct unityfs* fs, const char* path, char* buf, size_t sz)
{
  for (struct ufs_disk* disk = fs->all_disks; disk != fs->all_disks + fs->disks_count; ++disk) {
    char* real_path = get_real_path(disk, path);

    errno = 0;
    struct stat st_buf;

    if (lstat(real_path, &st_buf) == 0 && S_ISLNK(st_buf.st_mode)) {
      ssize_t lsz = readlink(real_path, buf, sz);
      if (lsz >= 0)
        buf[(size_t)lsz < sz - 1 ? (size_t)lsz : sz - 1] = '\0';
      free(real_path);
      break;
    }

    free(real_path);
  }
  return -errno;
}

static const char* remove_mountpoint(const char* path, const char* mountpoint)
{
  size_t mlen = strlen(mountpoint);
  if (strncmp(path, mountpoint, mlen) == 0)
    path += mlen;
  return path;
}

static char* resolve_link_target(const char* target, const char* link)
{
  if (target[0] == '/')
    return strdup(target);

  char* link_cp = strdup(link);

  char* link_dir = dirname(link_cp);
  if (strlen(link_dir) == 1 && link_dir[0] == '/')
    link_dir[0] = '\0';

  char* r_target = malloc(strlen(link_dir) + strlen(target) + 2);
  sprintf(r_target, "%s/%s", link_dir, target);

  free(link_cp);

  return r_target;
}

static char** new_real_links(struct unityfs* fs, const char* target, const char* link)
{
  char** path_list = calloc(fs->disks_count + 1, sizeof(char*));
  char** path_list_iter = path_list;

  char* tr_target = NULL;
  if (target[0] == '/') {
    tr_target = strdup(remove_mountpoint(target, fs->mountpoint));
  } else {
    tr_target = resolve_link_target(target, link);
  }

  for (struct ufs_disk* disk = fs->all_disks; disk != fs->all_disks + fs->disks_count; ++disk) {
    char* real_path = get_real_path(disk, tr_target);

    if (access(real_path, F_OK) == 0)
      *path_list_iter++ = get_real_path(disk, link);

    free(real_path);
  }

  if (path_list_iter == path_list)
    *path_list_iter = new_real_path(fs, link);

  free(tr_target);

  return path_list;
}

static void free_links_list(char** paths_list)
{
  for (char** iter = paths_list; *iter;)
    free(*iter++);
  free(paths_list);
}

int ufs_symlink(struct unityfs* fs, const char* target, const char* link_path)
{
  bool created = false;
  char** links_paths = new_real_links(fs, target, link_path);
  for (char** link_iter = links_paths; *link_iter; ++link_iter)
    if (symlink(target, *link_iter) == 0 && !created)
      created = true;
  free_links_list(links_paths);

  return created ? 0 : -errno;
}
