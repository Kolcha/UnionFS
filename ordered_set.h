#ifndef ORDERED_SET_H
#define ORDERED_SET_H

#include <stdbool.h>

struct _ordered_set;
typedef struct _ordered_set ordered_set_t;

ordered_set_t* ordered_set_create(void);
void ordered_set_destroy(ordered_set_t* set);

bool ordered_set_insert(ordered_set_t* set, const char* data);

#endif // ORDERED_SET_H
