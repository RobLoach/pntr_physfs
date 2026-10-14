/**********************************************************************************************
*
*   pntr_physfs - Integrate PhysFS virtual file system with pntr.
*
*   Copyright 2026 Rob Loach (@RobLoach)
*
*   DEPENDENCIES:
*       pntr https://github.com/robloach/pntr
*       physfs https://github.com/icculus/physfs
*
*   LICENSE: zlib/libpng
*
*   pntr_physfs is licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software:
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#ifndef PNTR_PHYSFS_H_
#define PNTR_PHYSFS_H_

#include <stdbool.h> // bool
#include <stddef.h> // NULL, size_t

#ifndef PNTR_PHYSFS_API
    #ifdef PNTR_API
        #define PNTR_PHYSFS_API PNTR_API
    #else
        #define PNTR_PHYSFS_API
    #endif
#endif

#ifndef PNTR_LOAD_FILE
    #define PNTR_LOAD_FILE pntr_physfs_load_file
#endif

#ifndef PNTR_SAVE_FILE
    #define PNTR_SAVE_FILE pntr_physfs_save_file
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Loads a file using PhysFS.
 *
 * @param fileName The name of the file to load from PhysFS.
 * @param bytesRead The output variable to save the amount of bytes that were read.
 *
 * @see pntr_load_file()
 * @see PNTR_LOAD_FILE
 */
PNTR_PHYSFS_API unsigned char* pntr_physfs_load_file(const char *fileName, unsigned int *bytesRead);

/**
 * Saves a file using PhysFS.
 *
 * @param fileName The name of the file to save into PhysFS.
 * @param data The file data to save into PhysFS.
 * @param bytesToWrite The size of the file.
 *
 * @see pntr_save_file()
 * @see PNTR_SAVE_FILE
 */
PNTR_PHYSFS_API bool pntr_physfs_save_file(const char *fileName, const void *data, unsigned int bytesToWrite);

#ifdef __cplusplus
}
#endif

#endif  // PNTR_PHYSFS_H_

#ifdef PNTR_PHYSFS_IMPLEMENTATION
#ifndef PNTR_PHYSFS_IMPLEMENTATION_ONCE
#define PNTR_PHYSFS_IMPLEMENTATION_ONCE

#include <stddef.h> // NULL, size_t

#ifndef PNTR_LOAD_FILE
    #define PNTR_LOAD_FILE pntr_physfs_load_file
#endif

#ifndef PNTR_SAVE_FILE
    #define PNTR_SAVE_FILE pntr_physfs_save_file
#endif

#ifndef PNTR_PHYSFS_PHYSFS_H
#define PNTR_PHYSFS_PHYSFS_H "physfs.h"
#endif
#include PNTR_PHYSFS_PHYSFS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Reports a failure through pntr's error state, when pntr.h is in scope.
 *
 * pntr_set_error() takes a `pntr_error`, and an enum can't be forward declared portably,
 * so failures can only be reported when pntr.h was included before this implementation.
 * Include pntr_physfs.h for its declarations, then pntr.h, then pntr_physfs.h again with
 * PNTR_PHYSFS_IMPLEMENTATION defined. Including it only once, before pntr.h, still works,
 * but every failure is then silent.
 *
 * @see pntr_set_error()
 */
#ifndef PNTR_PHYSFS_SET_ERROR
    #ifdef PNTR_H__
        #define PNTR_PHYSFS_SET_ERROR(error) pntr_set_error(error)
    #else
        #define PNTR_PHYSFS_SET_ERROR(error)
    #endif
#endif

// Only needed when pntr.h hasn't declared these already.
#ifndef PNTR_H__
PNTR_PHYSFS_API void pntr_unload_memory(void* pointer);
PNTR_PHYSFS_API void* pntr_load_memory(size_t size);
#endif

PNTR_PHYSFS_API unsigned char* pntr_physfs_load_file(const char *fileName, unsigned int *bytesRead) {
    // Nothing has been read yet, so every failure below can just return.
    if (bytesRead != NULL) {
        *bytesRead = 0;
    }

    // Open up the file.
    PHYSFS_File* handle = PHYSFS_openRead(fileName);
    if (handle == NULL) {
        PNTR_PHYSFS_SET_ERROR(PNTR_ERROR_FAILED_TO_OPEN);
        return NULL;
    }

    // Check to see how large the file is. The file did open, so an unknown length is
    // reported as a failure to read it rather than a failure to open it.
    PHYSFS_sint64 size = PHYSFS_fileLength(handle);
    if (size == -1) {
        PHYSFS_close(handle);
        PNTR_PHYSFS_SET_ERROR(PNTR_ERROR_FAILED_TO_READ);
        return NULL;
    }

    // The size is reported back through an unsigned int, so a file too large to count in
    // one can't be loaded without silently wrapping its size. pntr_load_file() rejects
    // the same file, so both loaders agree on what is too large to load.
    if ((PHYSFS_sint64)(unsigned int)size != size) {
        PHYSFS_close(handle);
        PNTR_PHYSFS_SET_ERROR(PNTR_ERROR_NOT_SUPPORTED);
        return NULL;
    }

    // An empty file loads successfully, the same way pntr_load_file() does, leaving
    // pntr_load_image() to report that no data can never be an image. PNTR_MALLOC(0) is
    // implementation-defined and is allowed to return NULL, so ask for at least one byte
    // to keep an empty file's result reliably non-NULL.
    void* buffer = pntr_load_memory(size > 0 ? (size_t)size : 1);
    if (buffer == NULL) {
        PHYSFS_close(handle);
        PNTR_PHYSFS_SET_ERROR(PNTR_ERROR_NO_MEMORY);
        return NULL;
    }

    // Read the file data into the buffer, and close the file handle.
    PHYSFS_sint64 read = PHYSFS_readBytes(handle, buffer, (PHYSFS_uint64)size);
    PHYSFS_close(handle);

    // A file that shrank since it was measured, or that stopped part way through, would
    // leave the tail of the buffer uninitialized, so a short read is a failure.
    if (read != size) {
        pntr_unload_memory(buffer);
        PNTR_PHYSFS_SET_ERROR(PNTR_ERROR_FAILED_TO_READ);
        return NULL;
    }

    if (bytesRead != NULL) {
        *bytesRead = (unsigned int)read;
    }

    return (unsigned char*) buffer;
}

PNTR_PHYSFS_API bool pntr_physfs_save_file(const char *fileName, const void *data, unsigned int bytesToWrite) {
    // Protect against empty writes.
    if (bytesToWrite == 0 || data == NULL) {
        PNTR_PHYSFS_SET_ERROR(PNTR_ERROR_FAILED_TO_WRITE);
        return false;
    }

    // Open the file.
    PHYSFS_File* handle = PHYSFS_openWrite(fileName);
    if (handle == NULL) {
        PNTR_PHYSFS_SET_ERROR(PNTR_ERROR_FAILED_TO_WRITE);
        return false;
    }

    // Write the data to the file handle. A write that stopped part way through, like one
    // that ran out of space or hit a quota, leaves a truncated file behind, so a short
    // write is a failure rather than a success.
    PHYSFS_sint64 written = PHYSFS_writeBytes(handle, data, bytesToWrite);

    // PhysicsFS buffers its writes, so a failure to flush them only surfaces when the
    // handle is closed. Ignoring that would report lost data as a successful save.
    int closed = PHYSFS_close(handle);

    if (written != (PHYSFS_sint64)bytesToWrite || closed == 0) {
        PNTR_PHYSFS_SET_ERROR(PNTR_ERROR_FAILED_TO_WRITE);
        return false;
    }

    return true;
}

#ifdef __cplusplus
}
#endif

#endif  // PNTR_PHYSFS_IMPLEMENTATION_ONCE
#endif  // PNTR_PHYSFS_IMPLEMENTATION
