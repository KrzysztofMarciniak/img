#include "files.h"

#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>

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

typedef struct {
    char **paths;
    size_t count;
    size_t cap;
} PathBuf;

static bool
pathbuf_push(PathBuf *buf, const char *path)
{
    if (buf->count == buf->cap) {
        size_t new_cap = buf->cap ? buf->cap * 2 : 16;
        char **grown = realloc(buf->paths, new_cap * sizeof(*buf->paths));
        if (!grown)
            return false;
        buf->paths = grown;
        buf->cap = new_cap;
    }
    char *copy = str_dup(path);
    if (!copy)
        return false;
    buf->paths[buf->count++] = copy;
    return true;
}

static bool
add_path(PathBuf *buf, const char *path)
{
    struct stat st;
    if (stat(path, &st) != 0)
        return true; /* skip missing, not fatal */

    if (S_ISREG(st.st_mode))
        return pathbuf_push(buf, path);

    if (S_ISDIR(st.st_mode)) {
        DIR *dir = opendir(path);
        if (!dir)
            return true; /* skip unreadable dir, not fatal */

        size_t path_len = strlen(path);
        bool needs_slash = path_len > 0 && path[path_len - 1] != '/';

        struct dirent *ent;
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                continue;

            size_t full_len = path_len + strlen(ent->d_name) + 2;
            char *full = malloc(full_len);
            if (!full) {
                closedir(dir);
                return false;
            }
            snprintf(full, full_len, "%s%s%s", path, needs_slash ? "/" : "", ent->d_name);

            struct stat sub_st;
            if (stat(full, &sub_st) == 0 && S_ISREG(sub_st.st_mode)) {
                if (!pathbuf_push(buf, full)) {
                    free(full);
                    closedir(dir);
                    return false;
                }
            }
            free(full);
        }
        closedir(dir);
        return true;
    }

    return true; /* skip other types (symlinks to nowhere, devices, etc) */
}

bool
files_collect(FileList *files, int argc, char **argv)
{
    if (argc == 0)
        return false;

    PathBuf buf = {0};
    for (int i = 0; i < argc; i++) {
        if (!add_path(&buf, argv[i])) {
            for (size_t j = 0; j < buf.count; j++)
                free(buf.paths[j]);
            free(buf.paths);
            return false;
        }
    }

    if (buf.count == 0) {
        free(buf.paths);
        return false;
    }

    files->paths = buf.paths;
    files->count = buf.count;
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
