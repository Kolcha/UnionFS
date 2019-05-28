#include "ordered_set.h"

#include <stdlib.h>
#include <string.h>

struct _ordered_set_node;
typedef struct _ordered_set_node ordered_set_node_t;

struct _ordered_set_node {
  char* data;
  ordered_set_node_t* left;
  ordered_set_node_t* right;
};

struct _ordered_set {
  ordered_set_node_t* root;
};

static void free_node_recursive(ordered_set_node_t* node)
{
  if (node == NULL)
    return;

  free_node_recursive(node->left);
  free_node_recursive(node->right);
  free(node->data);
  free(node);
}

ordered_set_t* ordered_set_create()
{
  return calloc(1, sizeof(ordered_set_t));
}

void ordered_set_destroy(ordered_set_t* set)
{
  free_node_recursive(set->root);
  free(set);
}

bool ordered_set_insert(ordered_set_t* set, const char* data)
{
  ordered_set_node_t** walk = &set->root;
  while (*walk) {
    int cmp_res = strcmp(data, (*walk)->data);
    if (cmp_res == 0) {
      return false;
    }
    if (cmp_res > 0)
      walk = &(*walk)->right;
    else
      walk = &(*walk)->left;
  }
  *walk = calloc(1, sizeof(ordered_set_node_t));
  (*walk)->data = strdup(data);
  return true;
}
