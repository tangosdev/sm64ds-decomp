// Gate-3b smoke: real game assets load through real game code.
//
// SharedFilePtr::Construct/LoadFile/Release from src/ run over the host
// card seam (port/hal/fs.cpp), allocating from the game's own root heap
// (gate 3a). Expected values below were computed by an INDEPENDENT Python
// LZ77 decoder over the extracted files, so the HAL's C decompressor is
// cross-checked against a second implementation, not against itself.
//
//   handle 2 -> FAT 1628  MG/d_2d_blue_shadow_nsc.bin   raw, 2048 bytes
//   handle 0 -> FAT 1626  MG/casino_back.bmd            LZ77, 18822 -> 35600
#include <stdio.h>
#include <string.h>

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

struct SharedFilePtr { u16 fileID; u8 numRefs; void *filePtr; };
struct HeapS;
extern "C" {
HeapS *_ZN4Heap13SetupRootHeapEv(void);
SharedFilePtr *_ZN13SharedFilePtr9ConstructEj(SharedFilePtr *self, u32 ov0FileID);
void *_ZN13SharedFilePtr8LoadFileEv(SharedFilePtr *self);
void _ZN13SharedFilePtr7ReleaseEv(SharedFilePtr *self);
}
#define Construct _ZN13SharedFilePtr9ConstructEj
#define LoadFile  _ZN13SharedFilePtr8LoadFileEv
#define Release   _ZN13SharedFilePtr7ReleaseEv

static int g_failures;
#define CHECK(cond) \
    do { if (!(cond)) { ++g_failures; \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } } while (0)

static u32 sum32(const void *p, u32 n)
{
    const u8 *b = (const u8 *)p;
    u32 s = 0;
    for (u32 i = 0; i < n; ++i) s += b[i];
    return s;
}

int main(void)
{
    CHECK(_ZN4Heap13SetupRootHeapEv() != NULL);

    /* raw file: bytes arrive exactly as on disk */
    SharedFilePtr raw;
    Construct(&raw, 2);
    CHECK(raw.fileID == 1628);          /* handle->FAT map ran in the ctor */
    CHECK(raw.numRefs == 0 && raw.filePtr == NULL);
    u8 *rp = (u8 *)LoadFile(&raw);
    CHECK(rp != NULL);
    CHECK(raw.numRefs == 1);
    static const u8 raw_first8[8] = {0x9d,0xe3,0x9d,0xe3,0x9d,0xe3,0x9d,0xe3};
    CHECK(memcmp(rp, raw_first8, 8) == 0);
    CHECK(sum32(rp, 2048) == 0x48000u);

    /* compressed file: LZ77 stream decodes to the independent expectation */
    SharedFilePtr bmd;
    Construct(&bmd, 0);
    CHECK(bmd.fileID == 1626);
    u8 *bp = (u8 *)LoadFile(&bmd);
    CHECK(bp != NULL);
    static const u8 bmd_first8[8] = {0x04,0x00,0x00,0x00,0x01,0x00,0x00,0x00};
    static const u8 bmd_last8[8]  = {0xff,0x7f,0xff,0x7f,0xff,0x7f,0xff,0x7f};
    CHECK(memcmp(bp, bmd_first8, 8) == 0);
    CHECK(memcmp(bp + 35600 - 8, bmd_last8, 8) == 0);
    CHECK(sum32(bp, 35600) == 0x2128efu);

    /* refcounting: shared loads alias, release tears down at zero */
    u8 *bp2 = (u8 *)LoadFile(&bmd);
    CHECK(bp2 == bp);
    CHECK(bmd.numRefs == 2);
    Release(&bmd);
    CHECK(bmd.numRefs == 1 && bmd.filePtr == bp);
    Release(&bmd);
    CHECK(bmd.numRefs == 0 && bmd.filePtr == NULL);
    Release(&raw);
    CHECK(raw.filePtr == NULL);

    /* the heap survived: a fresh load still works after the churn */
    SharedFilePtr again;
    Construct(&again, 0);
    u8 *ap = (u8 *)LoadFile(&again);
    CHECK(ap != NULL);
    CHECK(sum32(ap, 35600) == 0x2128efu);
    Release(&again);

    if (g_failures) {
        fprintf(stderr, "smoke_fs: %d FAILURE(S)\n", g_failures);
        return 1;
    }
    printf("smoke_fs: all checks passed (raw + LZ77 assets loaded through "
           "SharedFilePtr on the game heap, refcounts exact)\n");
    return 0;
}
