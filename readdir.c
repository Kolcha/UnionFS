#include "unityfs.h"
#include "private.h"

#include <errno.h>
#include <stdlib.h>

#include <unistd.h>

#include "ordered_set.h"

struct ufs_dir {
  DIR** opened_dirs;
  DIR** current_dir;
  ordered_set_t* known_names;
  dev_t* o_dir_disks;
};

int ufs_opendir(struct unityfs* fs, const char* path, struct ufs_dir** rdir)
{
  struct ufs_dir* udir = malloc(sizeof(struct ufs_dir));
  udir->opened_dirs = calloc(fs->disks_count + 1, sizeof(DIR*));
  udir->current_dir = udir->opened_dirs;
  udir->known_names = ordered_set_create();
  udir->o_dir_disks = NULL;

  DIR** opened_dir = udir->opened_dirs;

  for (struct ufs_disk* disk = fs->all_disks; disk != fs->all_disks + fs->disks_count; ++disk) {
    char* real_path = get_real_path(disk, path);

    errno = 0;
    DIR* dir = opendir(real_path);

    free(real_path);

    if (!dir && errno != ENOENT)
      break;

    if (dir)
      *(opened_dir++) = dir;
  }

  /* no directories where opened or error happened after some successful open(s) */
  if (!*udir->opened_dirs || (errno != 0 && errno != ENOENT)) {
    ufs_closedir(fs, udir);
    return -errno;
  }

  /* store device ids for disks where directories were opened */
  udir->o_dir_disks = calloc((size_t)(opened_dir - udir->opened_dirs), sizeof(dev_t));
  for (DIR** d = udir->opened_dirs; d != opened_dir; ++d) {
    struct stat stbuf;
    if (fstat(dirfd(*d), &stbuf) != 0) {
      ufs_closedir(fs, udir);
      return -errno;
    }
    udir->o_dir_disks[d - udir->opened_dirs] = stbuf.st_dev;
  }

  *rdir = udir;
  return 0;
}

int ufs_readdir(struct unityfs* fs, struct ufs_dir* dir, struct dirent** rentry)
{
  return ufs_readdir_plus(fs, dir, rentry, NULL);
}

int ufs_readdir_plus(struct unityfs* fs, struct ufs_dir* dir, struct dirent** rentry, struct stat* stbuf)
{
  while (*dir->current_dir) {
    errno = 0;
    struct dirent* entry = readdir(*dir->current_dir);

    if (!entry) {
      /* error happened */
      if (errno != 0)
        break;

      /* reached the end of current dir */
      ++dir->current_dir;
      continue;
    }

    if (ordered_set_insert(dir->known_names, entry->d_name)) {
      dev_t dev_id = dir->o_dir_disks[dir->current_dir - dir->opened_dirs];
      entry->d_ino = calc_ino(fs, dev_id, entry->d_ino);

      /* "Plus" mode */
      if (stbuf) {
        char old_cwd[PATH_MAX];
        if (!getcwd(old_cwd, sizeof(old_cwd)))
          break;

        int dir_fd = dirfd(*dir->current_dir);
        if (dir_fd < 0)
          break;

        if (fchdir(dir_fd) != 0)
          break;

        int stat_res = lstat(entry->d_name, stbuf);

        if (chdir(old_cwd) != 0)
          break;

        if (stat_res != 0)
          break;

        stbuf->st_ino = entry->d_ino;
      }

      *rentry = entry;
      return 0;
    }
  }

  *rentry = NULL;
  return -errno;
}

int ufs_closedir(struct unityfs* fs, struct ufs_dir* dir)
{
  (void) fs;

  ordered_set_destroy(dir->known_names);
  for (DIR** d = dir->opened_dirs; *d; ++d)
    closedir(*d);
  free(dir->opened_dirs);
  if (dir->o_dir_disks)
    free(dir->o_dir_disks);
  free(dir);
  return 0;
}
