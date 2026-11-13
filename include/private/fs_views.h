#ifndef PRIVATE_FS_VIEWS_H
#define PRIVATE_FS_VIEWS_H

/* NitroSDK FSFile / FSArchive / FSDirPos / FSFileID, this ROM's SDK layout, as
 * the FS_* routines in src/ (src/engine/fs/FS_Archive.cpp and
 * FS_CommandDefault.cpp read every field) use them. One plain-C definition
 * shared by the C and C++ files. This SDK is older than pret's: its thread
 * queues are 16-bit masks of thread ids, so FSArchive is 0x50 bytes and
 * FSFile 0x44. */
struct FSArchive;
struct FSFile;
struct FSDirEntry;

typedef struct FSFile FSFile;

typedef int (*FS_ARCHIVE_READ_FUNC)(struct FSArchive *p_arc, void *dst, unsigned int pos,
                                    unsigned int size);
typedef int (*FS_ARCHIVE_WRITE_FUNC)(struct FSArchive *p_arc, const void *src, unsigned int pos,
                                     unsigned int size);
typedef int (*FS_ARCHIVE_PROC_FUNC)(struct FSFile *p_file, int command);

typedef struct FSFileLink {
    struct FSFile *prev; /* 0x00 */
    struct FSFile *next; /* 0x04 -- an archive's command list: its head */
} FSFileLink;

typedef struct FSArchive {
    union {
        char ptr[4];
        unsigned int pack;
    } name;                           /* 0x00 -- up to three characters, zero padded */
    struct FSArchive *next;           /* 0x04 */
    struct FSArchive *prev;           /* 0x08 */
    unsigned short sync_q;            /* 0x0c -- OSThreadQueue: a synchronous caller waits here */
    unsigned short stat_q;            /* 0x0e -- woken once a suspend takes effect */
    unsigned int flag;                /* 0x10 */
    FSFileLink list;                  /* 0x14 */
    unsigned int base;                /* 0x1c */
    unsigned int fat;                 /* 0x20 */
    unsigned int fat_size;            /* 0x24 */
    unsigned int fnt;                 /* 0x28 */
    unsigned int fnt_size;            /* 0x2c */
    unsigned int fat_bak;             /* 0x30 */
    unsigned int fnt_bak;             /* 0x34 */
    void *load_mem;                   /* 0x38 */
    FS_ARCHIVE_READ_FUNC read_func;   /* 0x3c */
    FS_ARCHIVE_WRITE_FUNC write_func; /* 0x40 */
    FS_ARCHIVE_READ_FUNC table_func;  /* 0x44 */
    FS_ARCHIVE_PROC_FUNC proc;        /* 0x48 */
    unsigned int proc_flag;           /* 0x4c -- which commands go to proc */
} FSArchive;

typedef char FSArchive_size_must_be_0x50[sizeof(FSArchive) == 0x50 ? 1 : -1];

typedef struct FSDirPos {
    FSArchive *arc;        /* 0x00 */
    unsigned short own_id; /* 0x04 */
    unsigned short index;  /* 0x06 */
    unsigned int pos;      /* 0x08 */
} FSDirPos;

typedef struct FSFileID {
    FSArchive *arc;       /* 0x00 */
    unsigned int file_id; /* 0x04 */
} FSFileID;

typedef struct FSReadFileInfo {
    void *dst;            /* 0x00 */
    unsigned int len_org; /* 0x04 -- the length asked for */
    unsigned int len;     /* 0x08 -- the length after clamping */
} FSReadFileInfo;

typedef struct FSWriteFileInfo {
    const void *src;      /* 0x00 */
    unsigned int len_org; /* 0x04 */
    unsigned int len;     /* 0x08 */
} FSWriteFileInfo;

typedef struct FSSeekDirInfo {
    FSDirPos pos; /* 0x00 */
} FSSeekDirInfo;

typedef struct FSReadDirInfo {
    struct FSDirEntry *p_entry; /* 0x00 */
    int skip_string;            /* 0x04 */
} FSReadDirInfo;

typedef struct FSFindPathInfo {
    FSDirPos pos;       /* 0x00 -- where the search starts */
    const char *path;   /* 0x0c */
    int find_directory; /* 0x10 */
    union {
        FSFileID *file; /* 0x14 -- filled when find_directory is 0 */
        FSDirPos *dir;  /* 0x14 -- filled when find_directory is 1 */
    } result;
} FSFindPathInfo;

typedef char FSFindPathInfo_size_must_be_0x18[sizeof(FSFindPathInfo) == 0x18 ? 1 : -1];

typedef struct FSGetPathInfo {
    unsigned char *buf;       /* 0x00 */
    unsigned int buf_len;     /* 0x04 */
    unsigned short total_len; /* 0x08 -- 0 until measured */
    unsigned short dir_id;    /* 0x0a */
} FSGetPathInfo;

typedef struct FSOpenFileFastInfo {
    FSFileID id; /* 0x00 */
} FSOpenFileFastInfo;

typedef struct FSOpenFileDirectInfo {
    unsigned int top;    /* 0x00 */
    unsigned int bottom; /* 0x04 */
    unsigned int index;  /* 0x08 */
} FSOpenFileDirectInfo;

typedef struct FSFile {
    FSFileLink link;      /* 0x00 */
    FSArchive *arc;       /* 0x08 */
    unsigned int stat;    /* 0x0c */
    int command;          /* 0x10 */
    int error;            /* 0x14 */
    unsigned short queue; /* 0x18 -- OSThreadQueue: a synchronous caller sleeps here */
    union {
        struct {
            unsigned int own_id; /* 0x1c */
            unsigned int top;    /* 0x20 */
            unsigned int bottom; /* 0x24 */
            unsigned int pos;    /* 0x28 */
        } file;
        struct {
            FSDirPos pos;        /* 0x1c */
            unsigned int parent; /* 0x28 -- read from the root: the directory count */
        } dir;
    } prop; /* 0x1c */
    union {
        FSReadFileInfo readfile;
        FSWriteFileInfo writefile;
        FSSeekDirInfo seekdir;
        FSReadDirInfo readdir;
        FSFindPathInfo findpath;
        FSGetPathInfo getpath;
        FSOpenFileFastInfo openfilefast;
        FSOpenFileDirectInfo openfiledirect;
    } arg; /* 0x2c */
} FSFile;

/* FS_SeekFile's origin. */
typedef enum FSSeekFileMode { FS_SEEK_SET, FS_SEEK_CUR, FS_SEEK_END } FSSeekFileMode;

typedef char FSFile_size_must_be_0x44[sizeof(struct FSFile) == 0x44 ? 1 : -1];

#endif
