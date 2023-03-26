#include <stdio.h>

#include "physfs.h"

#define PNTR_PHYSFS_IMPLEMENTATION
#include "pntr-physfs.h"

#define PNTR_DISABLE_MATH
#define PNTR_IMPLEMENTATION
#include "pntr.h"

int main() {
    pntr_image* image = NULL;

    // Initialize PhysFS
    if (PHYSFS_init(0) == 0) {
        printf("Failed to initialize physfs\n");
        goto error;
    }

    // Set the write directory.
    if (PHYSFS_setWriteDir("resources") == 0) {
        printf("Failed to set write directory\n");
        goto error;
    }

    // Mount the resources directory as "res"
    if (PHYSFS_mount("resources", "res", 1) == 0) {
        printf("Failed to mount directory\n");
        goto error;
    }

    // Load the image from the "res" mountpoint in PhysFS.
    image = pntr_load_image("res/image.png");
    if (image == NULL) {
        printf("Failed to load image from resources/image.png\n");
        goto error;
    }

    // Manipulate the image
    pntr_draw_circle_fill(image, 100, 150, 50, PNTR_BLUE);

    // Save the image, expecting it to write to resources/output.png
    if (pntr_save_image(image, "output.png")) {
        printf("Write file to output.png, should be in resources directory\n");
    }
    else {
        printf("Failed to write resources/output.png\n");
    }

    // Unload the image.
    pntr_unload_image(image);

    if (PHYSFS_deinit() == 0) {
        printf("PHYSFS_deinit() unsuccessful");
        goto error;
    }

    return 0;
error:
    pntr_unload_image(image);
    PHYSFS_deinit();
    return 1;
}
