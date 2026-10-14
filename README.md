# pntr_physfs

[PhysicsFS](https://github.com/icculus/physfs) support for [pntr](https://github.com/RobLoach/pntr) to load and save images with PhysFS.

## Usage

Include [`pntr_physfs.h`](pntr_physfs.h) before including `pntr.h`. This will define `PNTR_LOAD_FILE` and `PNTR_SAVE_FILE` to have pntr load/save files with PhysicsFS.

Then include it a second time, after `pntr.h`, with `PNTR_PHYSFS_IMPLEMENTATION` defined. `pntr_set_error()` takes a `pntr_error`, and an enum can't be forward declared, so the implementation has to come after `pntr.h` for it to be able to report why a file failed to load or save. Defining `PNTR_PHYSFS_IMPLEMENTATION` on the first include still works, but every failure is then silent.

``` c
#include "physfs.h"

#include "pntr_physfs.h"

#define PNTR_IMPLEMENTATION
#include "pntr.h"

#define PNTR_PHYSFS_IMPLEMENTATION
#include "pntr_physfs.h"

int main() {
    // Initialize PhysFS
    PHYSFS_init(0);
    PHYSFS_mount("resources.zip", "res", 1);

    // Load an image from the resources.zip file.
    pntr_image* image = pntr_load_image("res/hello.png");

    // Unload the image.
    pntr_unload_image(image);

    // Deinitialize PhysFS
    PHYSFS_deinit();
}
```

## Threads

pntr_physfs holds no state of its own and uses a `PHYSFS_File` handle per call, which is what PhysicsFS asks of its callers. pntr's error state is shared, though: `PNTR_ERROR_STORAGE` defaults to `static`, so every thread reads and writes the same last-reported error. Give it a thread-local storage class when assets are loaded from more than one thread.

``` c
#define PNTR_ERROR_STORAGE static _Thread_local
```

## License

Unless stated otherwise, all works are:

- Copyright (c) 2026 [Rob Loach](https://robloach.net)

... and licensed under:

- [zlib License](LICENSE)
