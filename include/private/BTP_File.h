#ifndef PRIVATE_BTP_FILE_H
#define PRIVATE_BTP_FILE_H

#include "types.h"

/* Texture and palette entries are resolved by name to BMD resource indices. */
typedef struct BTPNameRecord {
    u16 resourceId;
    u16 pad_02;
    char *name;
} BTPNameRecord;

/* A sequence is resolved by name to a BMD material and selects a key range. */
typedef struct BTPSeqRecord {
    u16 materialId;
    u16 pad_02;
    char *name;
    u16 numKeys;
    u16 firstKey;
} BTPSeqRecord;

/* Pointers in a BTP file are relative until UpdateFileOffsets rebases them.
 * The three key arrays are parallel u16 tables indexed by BTPSeqRecord. */
typedef struct BTP_File {
    u16 numFrames;
    u16 numTexRecords;
    BTPNameRecord *texRecords;
    u16 numPalRecords;
    u16 pad_0a;
    BTPNameRecord *palRecords;
    u16 *keyFrames;
    u16 *keyTexIndices;
    u16 *keyPalIndices;
    u16 numSeqRecords;
    u16 pad_1e;
    BTPSeqRecord *seqRecords;
} BTP_File;

#ifndef SM64DS_PLATFORM_PC
typedef char BTPNameRecord_size_must_be_8[sizeof(BTPNameRecord) == 8 ? 1 : -1];
typedef char BTPSeqRecord_size_must_be_0xc[sizeof(BTPSeqRecord) == 0xc ? 1 : -1];
typedef char BTP_File_size_must_be_0x24[sizeof(BTP_File) == 0x24 ? 1 : -1];
#endif

#endif
