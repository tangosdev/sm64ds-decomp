#ifndef PRIVATE_BTA_FILE_H
#define PRIVATE_BTA_FILE_H

#include "types.h"

/* One texture-SRT component's keys: a single constant value at firstKey when
 * numKeys is 1, otherwise the value for frame f at firstKey + f. */
typedef struct BTAKeyRange {
    u16 numKeys;
    u16 firstKey;
} BTAKeyRange;

/* A material's texture-SRT animation, resolved by name to a BMD material
 * (func_02046b64) and applied to the runtime material record (func_020469e8). */
typedef struct BTAMaterialRecord {
    u16 materialId;
    u16 pad_02;
    char *name;
    BTAKeyRange scaleS;
    BTAKeyRange scaleT;
    BTAKeyRange rotation;
    BTAKeyRange transS;
    BTAKeyRange transT;
} BTAMaterialRecord;

/* Pointers in a BTA file are relative until UpdateFileOffsets rebases them.
 * The three key arrays are indexed by BTAKeyRange: scales and translations
 * are fx32, rotations are u16 angles. Layout read from func_02046b64 and
 * func_020469e8; nothing past materialRecords is established, so do not size
 * or allocate this view. */
typedef struct BTA_File {
    u16 numFrames;
    u16 pad_02;
    fx32 *keyScales;
    u16 *keyRotations;
    fx32 *keyTranslations;
    u16 numMaterialRecords;
    u16 pad_12;
    BTAMaterialRecord *materialRecords;
} BTA_File;

#ifndef SM64DS_PLATFORM_PC
typedef char BTAMaterialRecord_size_must_be_0x1c[sizeof(BTAMaterialRecord) == 0x1c ? 1 : -1];
#endif

#endif
