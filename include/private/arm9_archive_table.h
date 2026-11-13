#ifndef ARM9_ARCHIVE_TABLE_H
#define ARM9_ARCHIVE_TABLE_H

/* The game's thirteen NARC archives, data_0208ecf4. File ids from 0x8000 up
 * live in them: an entry holds [firstFileId, endFileId), endFileId being
 * firstFileId plus the archive's file count. The first ids step by 0x400;
 * that is only the spacing of the ranges, not their size. The ROM's
 * initialised table of first ids, in order:
 *
 *   ar0 0x8000  en1 0x8400  vs1 0x8800  vs2 0x8c00  vs3 0x9000  vs4 0x9400
 *   c2d 0x9800  ar1 0x9c00  cee 0xa000  cef 0xa400  ceg 0xa800  cei 0xac00
 *   ces 0xb000
 *
 * each with its path "/ARCHIVE/<file>.narc" (arc0.narc for ar0). LoadArchive
 * reads the file at path into one allocation from the heap at data_020a0ea0
 * and mounts it under name (func_02018934); UnloadArchive and UnloadArchives
 * give it back (func_02018908). Ids below 0x8000 are plain FAT ids
 * (func_020185c0). Plain C types: the consumers are C. */
typedef struct ArchiveTableEntry {
    /* 0x00 -- the mounted archive, 0 while unloaded: a 0x60-byte head (the
     * FSArchive, then the NARC, FATB and FIMG pointers; SoundEngine.cpp's
     * struct S82) and the file image after it */
    void *archive;
    void *heap;                 /* 0x04 -- the heap archive came from */
    unsigned short firstFileId; /* 0x08 */
    unsigned short endFileId;   /* 0x0a -- one past the last */
    char *name;                 /* 0x0c -- the archive name, "ar0" */
    char *path;                 /* 0x10 -- "/ARCHIVE/arc0.narc" */
} ArchiveTableEntry;

enum { ARCHIVE_TABLE_COUNT = 13 };

typedef char ArchiveTableEntry_size_must_be_0x14[sizeof(ArchiveTableEntry) == 0x14 ? 1 : -1];

extern ArchiveTableEntry data_0208ecf4[];

/* The archive a file lookup had to mount itself (func_020185c0,
 * func_020186c0), -1 for none; func_02018770 unloads it. */
extern signed char data_0208eb54;

#endif
