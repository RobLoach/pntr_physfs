#include <stdio.h>
#include <assert.h>

#if defined(__linux__)
    // Used to induce a genuine short write below.
    #include <string.h> // memset
    #include <signal.h> // signal, SIGXFSZ, SIG_IGN, SIG_DFL, SIG_ERR
    #include <sys/resource.h> // struct rlimit, getrlimit, setrlimit, RLIMIT_FSIZE
#endif

#include "physfs.h"

#include "../pntr_physfs.h"

#define PNTR_IMPLEMENTATION
#include "pntr.h"

// Included again, after pntr.h, so that failures can be reported with pntr_set_error().
#define PNTR_PHYSFS_IMPLEMENTATION
#include "../pntr_physfs.h"

int main() {
    // Initialize PhysFS
    assert(PHYSFS_init(0) != 0);

    // Nothing is mounted yet, so this file isn't in the search path. pntr's error state is
    // sticky, so prime it with an unrelated error to prove the stale value is overwritten
    // rather than inherited.
    pntr_set_error(PNTR_ERROR_NO_MEMORY);
    assert(pntr_load_file("res/image.png", NULL) == NULL);
    assert(pntr_get_error_code() == PNTR_ERROR_FAILED_TO_OPEN);

    // No write directory has been set yet, so the file can't be opened for writing.
    pntr_set_error(PNTR_ERROR_NO_MEMORY);
    assert(pntr_save_file("output.png", "pntr_physfs", 11) == false);
    assert(pntr_get_error_code() == PNTR_ERROR_FAILED_TO_WRITE);

    // Set the write directory.
    assert(PHYSFS_setWriteDir("resources") != 0);

    // Mount the resources directory as "res"
    assert(PHYSFS_mount("resources", "res", 1) != 0);

    // Create an empty file to load.
    PHYSFS_File* emptyHandle = PHYSFS_openWrite("empty.bin");
    assert(emptyHandle != NULL);
    assert(PHYSFS_close(emptyHandle) != 0);

    // An empty file loads successfully, matching pntr_load_file().
    pntr_set_error(PNTR_ERROR_NO_MEMORY);
    unsigned int emptyBytesRead = 1;
    unsigned char* emptyData = pntr_load_file("res/empty.bin", &emptyBytesRead);
    assert(emptyData != NULL);
    assert(emptyBytesRead == 0);
    pntr_unload_file(emptyData);

    // It is pntr_load_image() that reports an empty file as a problem with the data,
    // rather than pntr_physfs reporting it as a failure to open the file.
    pntr_set_error(PNTR_ERROR_NO_MEMORY);
    assert(pntr_load_image("res/empty.bin") == NULL);
    assert(pntr_get_error_code() == PNTR_ERROR_INVALID_DATA);

    // Load the image from the "res" mountpoint in PhysFS.
    pntr_set_error(PNTR_ERROR_NONE);
    pntr_image* image = pntr_load_image("res/image.png");
    assert(image != NULL);
    assert(pntr_get_error_code() == PNTR_ERROR_NONE);

    // Manipulate the image
    pntr_draw_circle_fill(image, image->width / 2, image->height / 2, 50, PNTR_BLUE);

    // Save the image, expecting it to write to resources/output.png
    assert(pntr_save_image(image, "output.png"));
    assert(pntr_get_error_code() == PNTR_ERROR_NONE);

    // Unload the image.
    pntr_unload_image(image);

    // Make sure the file was written correctly.
    FILE* file = fopen("resources/output.png", "r");
    assert(file != NULL);
    fclose(file);

    // A write that stops part way through leaves a truncated file behind, so it has to be
    // reported as a failure rather than a success. Linux shortens a write that crosses
    // RLIMIT_FSIZE, writing the bytes that fit and reporting the short count, which is the
    // only portable way found to induce a real short write. Other platforms refuse the
    // whole write instead, which the failure above already covers.
#if defined(__linux__)
    {
        struct rlimit originalLimit;
        struct rlimit shortLimit;
        char shortData[8192];
        const unsigned int shortLength = (unsigned int)sizeof(shortData) / 2;

        assert(getrlimit(RLIMIT_FSIZE, &originalLimit) == 0);

        // Crossing the limit also raises SIGXFSZ, which would otherwise kill the test.
        assert(signal(SIGXFSZ, SIG_IGN) != SIG_ERR);

        shortLimit = originalLimit;
        shortLimit.rlim_cur = (rlim_t)shortLength;
        assert(setrlimit(RLIMIT_FSIZE, &shortLimit) == 0);

        memset(shortData, 'p', sizeof(shortData));
        pntr_set_error(PNTR_ERROR_NONE);
        assert(pntr_save_file("short.bin", shortData, (unsigned int)sizeof(shortData)) == false);
        assert(pntr_get_error_code() == PNTR_ERROR_FAILED_TO_WRITE);

        assert(setrlimit(RLIMIT_FSIZE, &originalLimit) == 0);
        assert(signal(SIGXFSZ, SIG_DFL) != SIG_ERR);

        // Half the data did reach the disk, which is what makes this a short write rather
        // than a write that failed outright.
        PHYSFS_File* shortHandle = PHYSFS_openRead("res/short.bin");
        assert(shortHandle != NULL);
        assert(PHYSFS_fileLength(shortHandle) == (PHYSFS_sint64)shortLength);
        assert(PHYSFS_close(shortHandle) != 0);
        assert(PHYSFS_delete("short.bin") != 0);
    }
#endif

    assert(PHYSFS_deinit() != 0);

    return 0;
}
