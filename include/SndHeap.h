#ifndef SNDHEAP_H
#define SNDHEAP_H

#include "types.h"
#include "nnsys/fnd.h"
#include "nitro/fs/file.h"

/* A block func_020511d4 carves from a sound heap: its link in the heap's
   newest level, then what freeing the level calls back with. The caller's
   memory starts at 0x20. */
typedef void (*SndHeapDisposeCallback)(void *mem, u32 size, u32 data1, u32 data2);
struct SndHeapBlock {
    NNSFndLink link;                 /* 0x00 */
    u32 size;                        /* 0x08 */
    SndHeapDisposeCallback callback; /* 0x0c */
    u32 data1;                       /* 0x10 */
    u32 data2;                       /* 0x14 */
    u8 pad18[8];
    u8 mem[1]; /* 0x20 */
};

/* A sound heap (func_0205130c): a frame heap and the stack of levels
   func_02050fd4 pushes. */
struct SndHeap {
    NNSFndHeapHandle frmHeap; /* 0x0 */
    NNSFndList levels;        /* 0x4 */
    int func_02050fd4();
    int func_02051024(NNSFndHeapHandle frmHeap);
    void func_02051244();
    void func_020512f0();
    u32 func_02051074();
    void func_020510a4(int level);
    int func_0205117c();
    void *func_020511d4(u32 size, SndHeapDisposeCallback callback, u32 data1, u32 data2);
    int func_02052008();
};

/* The Nitro binary file header every NARC, SDAT, SSEQ and SSAR file opens
   with. */
struct NitroFileHeader {
    u32 magic;      /* 0x00 */
    u16 byteOrder;  /* 0x04 */
    u16 version;    /* 0x06 */
    u32 fileSize;   /* 0x08 */
    u16 headerSize; /* 0x0c */
    u16 numBlocks;  /* 0x0e */
    int func_0204ef9c();
};

struct Hdr84;

/* The sound archive data_020a5bb8 points at, opened by func_02050f34. */
struct data_020a5bb8_t {
    /* 0x00 */ NitroFileHeader fileHeader;
    /* 0x10 */ int symbolOffset;
    /* 0x14 */ int symbolSize;
    /* 0x18 */ int infoOffset;
    /* 0x1c */ int infoSize;
    /* 0x20 */ int fileTableOffset;
    /* 0x24 */ int fileTableSize;
    /* 0x28 */ int fileImageOffset;
    /* 0x2c */ int fileImageSize;
    /* 0x30 */ FSFile file;
    /* 0x74 */ FSFileID fileId;
    /* 0x7c */ struct Hdr84 *fileTable;
    /* 0x80 */ void *symbols;
    /* 0x84 */ void *info;
    int func_02050d54(struct SndHeap *heap, int loadSymbols);
    void func_02050f34(const char *path, struct SndHeap *heap, int loadSymbols);
};

#endif
