#ifndef UNIONFS_CORE_H
#define UNIONFS_CORE_H

struct unionfs;

struct unionfs* unionfs_create(void);
void unionfs_destroy(struct unionfs* fs);

#endif // UNIONFS_CORE_H
