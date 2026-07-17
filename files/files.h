#ifndef FILES_H
#define FILES_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    char **paths;
    size_t count;
} FileList;

bool files_collect(FileList *files, int argc, char **argv);
void files_free(FileList *files);

#endif

