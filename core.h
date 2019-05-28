#ifndef UNITYFS_CORE_H
#define UNITYFS_CORE_H

struct unityfs;

struct unityfs* unityfs_create(void);
void unityfs_destroy(struct unityfs* fs);

#endif // UNITYFS_CORE_H
