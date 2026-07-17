#include "files.h"

#include <stdlib.h>
#include <string.h>

static char *
str_dup(const char *s)
{
    size_t len = strlen(s) + 1;

    char *copy = malloc(len);
    if (!copy)
        return NULL;

    memcpy(copy, s, len);

    return copy;
}

bool
files_collect(FileList *files, int argc, char **argv)
{
    if (argc == 0)
        return false;

    files->paths = calloc(argc, sizeof(*files->paths));
    if (!files->paths)
        return false;

    for (int i = 0; i < argc; i++) {
        files->paths[i] = str_dup(argv[i]);

        if (!files->paths[i]) {
            files_free(files);
            return false;
        }
    }

    files->count = argc;

    return true;
}

void
files_free(FileList *files)
{
    if (!files)
        return;

    for (size_t i = 0; i < files->count; i++)
        free(files->paths[i]);

    free(files->paths);

    files->paths = NULL;
    files->count = 0;
}

