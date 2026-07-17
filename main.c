#include <stdio.h>
#include <stdlib.h>

#include "backend/backend.h"
#include "files/files.h"
#include "x11/x11.h"

int
main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s FILE\n", argv[0]);
        return EXIT_FAILURE;
    }

    FileList files = {0};

    if (!files_collect(&files, argc - 1, argv + 1)) {
        fprintf(stderr, "no images found\n");
        return EXIT_FAILURE;
    }

    Image *img = img_load(files.paths[0]);
    if (!img) {
        fprintf(stderr, "failed to load image: %s\n", files.paths[0]);
        files_free(&files);
        return EXIT_FAILURE;
    }

    if (!x11_init(img->width, img->height)) {
        img_free(img);
        files_free(&files);
        return EXIT_FAILURE;
    }

    x11_draw(img);

    for (;;) {
        switch (x11_wait_event()) {

        case EVENT_REDRAW:
            x11_draw(img);
            break;

        case EVENT_QUIT:
            goto done;

        default:
            break;
        }
    }

done:
    img_free(img);
    x11_destroy();
    files_free(&files);

    return EXIT_SUCCESS;
}

