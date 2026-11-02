#ifndef PRIVATE_FS_VIEWS_H
#define PRIVATE_FS_VIEWS_H

/* NitroSDK FSFile / FSArchive / FSDirPos, this ROM's SDK layout, as far as the
 * FS_* routines in src/ read or write them. One plain-C definition shared by
 * the C and C++ files; unnamed bytes stay padding. */
struct FSArchive;
struct FSFile;

typedef int (*FS_ARCHIVE_PROC_FUNC)(struct FSFile *p_file, int command);

typedef struct FSArchive {
    char pad_00[0xc];
    unsigned short sync_q; /* 0x0c -- OSThreadQueue: a synchronous caller waits here */
    unsigned short stat_q; /* 0x0e */
    unsigned int flag;     /* 0x10 */
    char pad_14[0x48 - 0x14];
    FS_ARCHIVE_PROC_FUNC proc; /* 0x48 */
    unsigned int proc_flag;    /* 0x4c -- which commands go to proc */
} FSArchive;

typedef char FSArchive_size_must_be_0x50[sizeof(FSArchive) == 0x50 ? 1 : -1];

typedef struct FSDirPos {
    FSArchive *arc; /* 0x00 */
    short own_id;   /* 0x04 */
    short index;    /* 0x06 */
    int pos;        /* 0x08 */
} FSDirPos;

typedef struct FSOpenFileDirectArg {
    unsigned int top;    /* 0x00 */
    unsigned int bottom; /* 0x04 */
    unsigned int index;  /* 0x08 */
} FSOpenFileDirectArg;

typedef struct FSFindPathArg {
    FSDirPos pos;       /* 0x00 */
    char *path;         /* 0x0c */
    int find_directory; /* 0x10 */
    int result;         /* 0x14 -- the FSFileID or FSDirPos to fill */
} FSFindPathArg;

struct FSFile {
    struct FSFile *prev;  /* 0x00 -- link.prev */
    struct FSFile *next;  /* 0x04 -- link.next */
    FSArchive *arc;       /* 0x08 */
    unsigned int stat;    /* 0x0c */
    int command;          /* 0x10 */
    int error;            /* 0x14 */
    unsigned short queue; /* 0x18 -- OSThreadQueue: a synchronous caller sleeps here */
    char pad_1a[0x2c - 0x1a];
    union {
        FSOpenFileDirectArg openfiledirect;
        FSFindPathArg findpath;
    } arg; /* 0x2c */
};

#endif
