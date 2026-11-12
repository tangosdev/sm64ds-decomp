#ifndef PRIVATE_BMA_FILE_H
#define PRIVATE_BMA_FILE_H

#include "types.h"

/* One material colour channel's key: the value for frame f is
 * keyValues[firstKey + (perFrame ? f : 0)] (func_02046e28). */
typedef struct BMAKey {
    u8 pad_00;
    u8 perFrame;
    u16 firstKey;
} BMAKey;

/* A material's colour animation, resolved by name to a BMD material
 * (func_020470e8): diffuse, ambient, specular and emission RGB, then alpha. */
typedef struct BMAMaterialRecord {
    u16 materialId;
    u16 pad_02;
    char *name;
    BMAKey keys[13];
} BMAMaterialRecord;

/* Pointers in a BMA file are relative until UpdateFileOffsets rebases them.
 * Layout read from func_020470e8 and func_02046e28; nothing past
 * materialRecords is established, so do not size or allocate this view. */
typedef struct BMA_File {
    u16 numFrames;
    u16 pad_02;
    u8 *keyValues;
    u16 numMaterialRecords;
    u16 pad_0a;
    BMAMaterialRecord *materialRecords;
} BMA_File;

#ifndef SM64DS_PLATFORM_PC
typedef char BMAMaterialRecord_size_must_be_0x3c[sizeof(BMAMaterialRecord) == 0x3c ? 1 : -1];
#endif

#endif
