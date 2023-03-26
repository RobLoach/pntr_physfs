# pntr-physfs

[PhysicsFS](https://github.com/icculus/physfs) support for [pntr](https://github.com/RobLoach/pntr) to load and save images with PhysFS.

## Usage

Include [`pntr-physfs.h`](pntr-physfs.h) before including `pntr.h`. This will define `PNTR_LOAD_FILE` and `PNTR_SAVE_FILE` to have pntr load/save files with PhysicsFS.

``` c
#include "physfs.h"

#define PNTR_PHYSFS_IMPLEMENTATION
#include "pntr-physfs.h"

#define PNTR_IMPLEMENTATION
#include "pntr.h"
```

After being included, pntr will load and save files via PhysFS.

``` c
// Load an image from the res mount point in PhysFS
pntr_image* image = pntr_load_image("res/hello.png");

// Save an image to PhysFS's write directory.
pntr_save_image("output.png");
```

## License

Unless stated otherwise, all works are:

- Copyright (c) 2023 [Rob Loach](https://robloach.net)

... and licensed under:

- [zlib License](LICENSE)
