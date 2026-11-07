#include <stdio.h>
#include <stdlib.h>

#include "physfs.h"

#include "../pntr_physfs.h"

#define PNTR_IMPLEMENTATION
#include "pntr.h"

// Included again, after pntr.h, so that failures can be reported with pntr_set_error().
#define PNTR_PHYSFS_IMPLEMENTATION
#include "../pntr_physfs.h"

// Unlike assert(), runs the check in every build type, including NDEBUG.
#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: CHECK failed: %s\n", __FILE__, __LINE__, #condition); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

int main(void) {
    // Initialize PhysFS
    CHECK(PHYSFS_init(0) != 0);

    // Nothing is mounted yet, so this file isn't in the search path. pntr's error state is
    // sticky, so prime it with an unrelated error to prove the stale value is overwritten
    // rather than inherited.
    pntr_set_error(PNTR_ERROR_NO_MEMORY);
    CHECK(pntr_load_file("res/image.png", NULL) == NULL);
    CHECK(pntr_get_error_code() == PNTR_ERROR_FAILED_TO_OPEN);

    // No write directory has been set yet, so the file can't be opened for writing.
    pntr_set_error(PNTR_ERROR_NO_MEMORY);
    CHECK(pntr_save_file("output.png", "pntr_physfs", 11) == false);
    CHECK(pntr_get_error_code() == PNTR_ERROR_FAILED_TO_WRITE);

    // Set the write directory.
    CHECK(PHYSFS_setWriteDir("resources") != 0);

    // Mount the resources directory as "res"
    CHECK(PHYSFS_mount("resources", "res", 1) != 0);

    // Create an empty file to load.
    PHYSFS_File* emptyHandle = PHYSFS_openWrite("empty.bin");
    CHECK(emptyHandle != NULL);
    CHECK(PHYSFS_close(emptyHandle) != 0);

    // An empty file loads successfully, matching pntr_load_file().
    pntr_set_error(PNTR_ERROR_NO_MEMORY);
    unsigned int emptyBytesRead = 1;
    unsigned char* emptyData = pntr_load_file("res/empty.bin", &emptyBytesRead);
    CHECK(emptyData != NULL);
    CHECK(emptyBytesRead == 0);
    pntr_unload_file(emptyData);

    // It is pntr_load_image() that reports an empty file as a problem with the data,
    // rather than pntr_physfs reporting it as a failure to open the file.
    pntr_set_error(PNTR_ERROR_NO_MEMORY);
    CHECK(pntr_load_image("res/empty.bin") == NULL);
    CHECK(pntr_get_error_code() == PNTR_ERROR_INVALID_DATA);

    // Load the image from the "res" mountpoint in PhysFS.
    pntr_set_error(PNTR_ERROR_NONE);
    pntr_image* image = pntr_load_image("res/image.png");
    CHECK(image != NULL);
    CHECK(pntr_get_error_code() == PNTR_ERROR_NONE);

    // Manipulate the image
    pntr_draw_circle_fill(image, image->width / 2, image->height / 2, 50, PNTR_BLUE);

    // Save the image, expecting it to write to resources/output.png
    CHECK(pntr_save_image(image, "output.png"));
    CHECK(pntr_get_error_code() == PNTR_ERROR_NONE);

    // Unload the image.
    pntr_unload_image(image);

    // Make sure the file was written correctly.
    FILE* file = fopen("resources/output.png", "r");
    CHECK(file != NULL);
    fclose(file);

    CHECK(PHYSFS_deinit() != 0);

    return 0;
}
