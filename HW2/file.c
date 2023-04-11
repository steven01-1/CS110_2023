#include "file.h"
#include "explorer.h" /* include headers */
#include <stdio.h>
#include <stdlib.h> /* include headers */
#include <string.h>

struct file *file_new(int type, char *name) {
  /* Initialization */
  struct file *file = NULL;
  /* Check for integrity */
  if (!name || type < 0 || type >= MAX_FT_NO) {
    return NULL;
  }
  /* Allocate memory and initialze the file. */
  file = calloc(1, sizeof(struct file));
  file->type = type;
  file->size = DEFAULT_FILE_SIZE;
  file->data = calloc(file->size, 1);
  /* Crate associtate node and set it to base. */
  file->base = node_new(false, name, file);
  return file;
}

void file_release(struct file *file) {
  /* Check for null pointer. */
  if (!file) {
    return;
  }
  /* relase the data. */
  /* Check if base has already been released. Prevent circular call. */
  if (file->base) {
    file->base->inner.file = NULL;
    node_release(file->base);
  }
  /* Release the resources. */
  free(file->data);
  free(file);
}

bool file_write(struct file *file, int offset, int bytes, const char *buf) {
  /* YOUR CODE HERE */
  if (!file || !buf || offset < 0) {
    return false;
  }
  /* new data exceeds */
  if (offset + bytes > file->size) {
    char *new_data = (char *)calloc(offset + bytes ,sizeof(char));
    if (new_data == NULL) {
      return false; /* return false if fails */
    }
    int t; /* int t is created */ // offset  file->size
    for (t = 0; t < file->size; t++) {
      new_data[t] = file->data[t];
    }
    int i, j; /* int i, j is created */
    for (i = offset, j = 0; i < offset + bytes; i++, j++)
      new_data[i] = buf[j]; /* buf[j] is assigned to new_data[i] */
    file->size = offset + bytes;
    free(file->data); /* Free is called */
    file->data = new_data;
    new_data = NULL; /* It changes new_data */
    return true;
  }

  int i, j; /* int i, j is created */
  for (i = offset, j = 0; i < offset + bytes; i++, j++) {
    file->data[i] = buf[j]; 
  }
  return true; /* return if succeed */
}

bool file_read(const struct file *file, int offset, int bytes, char *buf) {
  /* YOUR CODE HERE */
  /* printf("call read\n"); */
  if (!file || !buf || offset < 0 || offset + bytes > file->size)
    return false;
  int i; /* int i is created */
  for (i = offset; i < offset + bytes; i++)
    buf[i - offset] = file->data[i]; /* data[i] is assigned to bug[i - offset] */
  return true;
}
