#include "dir.h"
#include "explorer.h"
#include "file.h" /* include headers */
#include "node.h"
#include <stdlib.h> /* include headers */
#include <string.h> /* include headers */


// static bool dir_add_sub(struct directory *dirnode, struct node *sub);


/* test if two strings are equal */
bool test_eq(const char *s1, const char *s2) {
  int i = 0;
  while (1) { /* It is a while loop */
    if (s1[i] == '\0' && s2[i] == '\0')
      return true;
    if (s1[i] != s2[i]) /* It judges the condition */
      return false; 
    i++;
  }
}

struct directory *dir_new(char *name) {
  /* Initialization */
  struct directory *dir = NULL;
  /* Check for null pointer */
  if (!name) {
    return NULL;
  }
  /* Allocate memory */
  dir = calloc(1, sizeof(struct directory));
  dir->capacity = DEFAULT_DIR_SIZE;
  dir->size = 0; /* 0 is assigned to size */
  dir->subordinates = calloc(dir->capacity, sizeof(struct node));
  dir->parent = NULL;
  /* Create base node */
  dir->base = node_new(true, name, dir);
  return dir;
}
#include <stdio.h>
void dir_release(struct directory *dir) {
  /* Initialization */
  int i = 0;
  if (!dir) {
    return;
  }
  /* Release all the subordiniates */
  for (i = 0; i < dir->size; i++) {
    node_release(dir->subordinates[i]);
  }
  /* Release the resources */
  /* Check if base has already been released. Prevent circular call. */
  if (dir->base) {
    dir->base->inner.dir = NULL;
    node_release(dir->base);
  }
  /* Release data and self. */
  free(dir->subordinates);
  free(dir);
}

struct node *dir_find_node(const struct directory *dir, const char *name) {
  /* YOUR CODE HERE */
  if (!dir || !name) /* It judge the condition */
    return NULL;
  int i; /* int i is defined */
  for (i = 0; i < dir->capacity; i++) { /* It execute for loop */
    if (dir->subordinates[i] && test_eq(name, dir->subordinates[i]->name))
      return dir->subordinates[i]; /* It returns the node */
  }
  return NULL;
}

bool dir_add_file(struct directory *dir, int type, char *name) {
  /* YOUR CODE HERE */
  if (!dir || !name || type < 0 || type >= MAX_FT_NO) /* It judged the condition */
    return false;
  int i; /* int i is defined */
  for (i = 0; i < dir->capacity; i++)
    if (dir->subordinates[i] && test_eq(dir->subordinates[i]->name, name)) {
      return false; 
    }
  /* size exceeds */
  if (dir->size == dir->capacity) { 
    struct node **new_node = calloc(2 * dir->capacity, sizeof(struct node *)); /* It is a new struct */
    if (new_node == NULL) 
      return false;
    for (i = 0; i < dir->capacity; i++) /* Copy the old node  */
      if (dir->subordinates[i] != NULL)
        new_node[i] = dir->subordinates[i];
    /* definately have a space in position [size] */
    struct file *nfile = file_new(type, name);
    new_node[dir->size] = nfile->base;
    // new_node[dir->size] = node_new(false, name, file_new(type, name));
    free(dir->subordinates); /* Free is called */
    dir->subordinates = new_node;
    new_node = NULL;
    dir->size += 1; /* Size is changed */
    dir->capacity *= 2;
    return true; /* return if succeed */
  }
  /* size dosen't exceed */
  for (i = 0; i < dir->capacity; i++)
    if (!dir->subordinates[i]) {
      struct file *nfile = file_new(type, name);
      dir->subordinates[i] = nfile->base;
      break;
    }
  dir->size += 1; /* change the size */
  return true;
}

bool dir_add_subdir(struct directory *dir, char *name) {
  /* YOUR CODE HERE */
  if (!dir || !name) /* It judges the condition */
    return false;
  int i; /* int i is created */
  for (i = 0; i < dir->capacity; i++) /* It executes for loop */
    if (dir->subordinates[i] && test_eq(dir->subordinates[i]->name, name)) { 
      return false;
    }
  /* size exceeds */
  if (dir->size == dir->capacity) {
    struct node **new_node = 
    (struct node **)calloc(2 * dir->capacity, sizeof(struct node *)); /* It is a new struct */
    if (new_node == NULL) /* It judges the condition */
      return false;
    int j; /* int j is created */
    for (j = 0; j < dir->capacity; j++) {
      if (dir->subordinates[i])
        new_node[i] = dir->subordinates[i];
      // node_new(dir->subordinates[i]->is_dir, dir->subordinates[i]->name,
      //        dir->subordinates[i]->inner.dir); /* A new node is created */
    }
    for (j = 0; j < dir->capacity; j++) /* creat new sub */
      if (!dir->subordinates[j]) {
        struct directory *ndir = dir_new(name);
        new_node[j] = ndir->base;
        ndir->parent = dir;
        // new_node[j] = node_new(true, name, dir_new(name));
        // new_node[j]->inner.dir->parent = dir; /* It modifies the attributes */
        // new_node[j]->inner.dir->base = new_node[j];
        break;
      }
    // free(dir->subordinates);
    // for (j = 0; j < dir->capacity; j++) {
    //   if (dir->subordinates[j])
    //     free(dir->subordinates[i]); /* Free is called */
    // }
    free(dir->subordinates);
    dir->subordinates = new_node;
    new_node = NULL; /* NULL is assiged to new_node */
    dir->size += 1;
    dir->capacity *= 2; /* It enlarges the capacity */
    return true;
  }
  /* size doesn't exceed */
  int j;
  for (j = 0; j < dir->capacity; j++)
    if (!dir->subordinates[j]) {
      struct directory *ndir = dir_new(name); 
      // dir->subordinates[j] = node_new(true, name, dir_new(name));
      dir->subordinates[j] = ndir->base;
      // dir->subordinates[j]->inner.dir->parent = dir;
      ndir->parent = dir; 
      // dir->subordinates[i]->inner.dir->base = dir->subordinates[i];
      break;
  }
  dir->size++; /* It enlarges the size */
  return true;
}

bool dir_delete(struct directory *dir, const char *name) {
  /* YOUR CODE HERE */
  if (!dir || !name) /* It judges the condition */
    return false;
  int i; /* int i in created */
  int flag = false; /* It changes flag */
  int to_del;
  // struct node **to_delete = NULL;
  for (i = 0; i < dir->capacity; i++)
    if (dir->subordinates[i] && test_eq(dir->subordinates[i]->name, name)) {
      flag = true; /* It changes flag */
      to_del = i;
      // to_delete = &dir->subordinates[i];
      break;
    } /* Block ends */
  if (!flag)
    return false; /* return if fails */
  // node_release(*to_delete);
  node_release(dir->subordinates[to_del]);
  // *to_delete = NULL; /* NULL is assiged to *to_delete */
  dir->subordinates[to_del] = NULL;
  dir->size--; /* It changes size */
  return true;
}
