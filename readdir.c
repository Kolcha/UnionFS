#include "unityfs.h"
#include "private.h"

#include <errno.h>
#include <stdlib.h>

#include "ordered_set.h"

struct ufs_dir {
  DIR** opened_dirs;
  DIR** current_dir;
  ordered_set_t* known_names;
};

int ufs_opendir(struct unityfs* fs, const char* path, struct ufs_dir** rdir)
{
  struct ufs_dir* udir = malloc(sizeof(struct ufs_dir));
  udir->opened_dirs = calloc(fs->disks_count + 1, sizeof(DIR*));
  udir->current_dir = udir->opened_dirs;
  udir->known_names = ordered_set_create();

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
  if (!udir->current_dir || (errno != 0 && errno != ENOENT)) {
    ufs_closedir(fs, udir);
    return -errno;
  }

  *rdir = udir;
  return 0;
}

int ufs_readdir(struct unityfs* fs, struct ufs_dir* dir, struct dirent** rentry)
{
  (void) fs;

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
  free(dir);
  return 0;
}
