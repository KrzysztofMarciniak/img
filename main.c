#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "backend/backend.h"
#include "files/files.h"
#include "x11/x11.h"

#define ZOOM_MIN 0.1
#define ZOOM_MAX 10.0
#define ZOOM_STEP 1.1
#define PAN_STEP 40

static void
print_usage(const char *prog)
{
    printf("usage: %s [-h|--h|-help|--help] FILE...\n", prog);
    printf("\n");
    printf("keybindings:\n");
    printf("  h/l/j/k     pan left/right/up/down\n");
    printf("  Shift+h/l   previous/next image\n");
    printf("  +/-         zoom in/out\n");
    printf("  0           reset zoom\n");
    printf("  q/Esc       quit\n");
}

int
main(int argc, char **argv)
{
    if (argc < 2 ||
        strcmp(argv[1], "-h") == 0 ||
        strcmp(argv[1], "--h") == 0 ||
        strcmp(argv[1], "-help") == 0 ||
        strcmp(argv[1], "--help") == 0) {
        print_usage(argv[0]);
        return argc < 2 ? EXIT_FAILURE : EXIT_SUCCESS;
    }

    FileList files = {0};
    if (!files_collect(&files, argc - 1, argv + 1)) {
	print_usage(argv[0]);
	printf("\n");
        fprintf(stderr, "no images found\n");
        return EXIT_FAILURE;
    }

    size_t idx = 0;
    double zoom = 1.0;
    int pan_x = 0, pan_y = 0;

    Image *img = img_load(files.paths[idx]);
    if (!img) {
        fprintf(stderr, "failed to load image: %s\n", files.paths[idx]);
        files_free(&files);
        return EXIT_FAILURE;
    }

    if (!x11_init(img->width, img->height)) {
        img_free(img);
        files_free(&files);
        return EXIT_FAILURE;
    }
    x11_draw(img, zoom, pan_x, pan_y);

    for (;;) {
        Event ev = x11_wait_event();
        switch (ev) {
        case EVENT_REDRAW:
            x11_draw(img, zoom, pan_x, pan_y);
            break;
        case EVENT_NEXT:
        case EVENT_PREV: {
            if (files.count <= 1)
                break;
            size_t next = (ev == EVENT_NEXT)
                ? (idx + 1) % files.count
                : (idx + files.count - 1) % files.count;
            Image *loaded = img_load(files.paths[next]);
            if (!loaded) {
                fprintf(stderr, "failed to load image: %s\n", files.paths[next]);
                break;
            }
            img_free(img);
            img = loaded;
            idx = next;
            zoom = 1.0;
            pan_x = pan_y = 0;
            x11_draw(img, zoom, pan_x, pan_y);
            break;
        }
        case EVENT_ZOOM_IN:
            zoom *= ZOOM_STEP;
            if (zoom > ZOOM_MAX)
                zoom = ZOOM_MAX;
            x11_draw(img, zoom, pan_x, pan_y);
            break;
        case EVENT_ZOOM_OUT:
            zoom /= ZOOM_STEP;
            if (zoom < ZOOM_MIN)
                zoom = ZOOM_MIN;
            x11_draw(img, zoom, pan_x, pan_y);
            break;
        case EVENT_ZOOM_RESET:
            zoom = 1.0;
            pan_x = pan_y = 0;
            x11_draw(img, zoom, pan_x, pan_y);
            break;
        case EVENT_PAN_LEFT:
            pan_x += PAN_STEP;
            x11_draw(img, zoom, pan_x, pan_y);
            break;
        case EVENT_PAN_RIGHT:
            pan_x -= PAN_STEP;
            x11_draw(img, zoom, pan_x, pan_y);
            break;
        case EVENT_PAN_UP:
            pan_y += PAN_STEP;
            x11_draw(img, zoom, pan_x, pan_y);
            break;
        case EVENT_PAN_DOWN:
            pan_y -= PAN_STEP;
            x11_draw(img, zoom, pan_x, pan_y);
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
