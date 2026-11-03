#ifndef PRIVATE_BTA_FILE_H
#define PRIVATE_BTA_FILE_H

#include "types.h"

/* Only the frame-count prefix is established by current consumers. The rest
 * of the BTA file layout remains unknown; do not size or allocate this view. */
typedef struct BTA_File {
    u16 numFrames;
} BTA_File;

#endif
