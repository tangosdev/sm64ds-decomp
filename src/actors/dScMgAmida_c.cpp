//cpp
/* dScMgAmida_c -- the ladder-lottery (Amida) minigame, ov006, 13 functions
 * (.text 0x020d3624..0x020d5a54).
 *
 * This is the upper part of the class's run: the unmatched function just
 * below 0x020d3624 splits it, and the destructor pair, which owns the
 * vtable, lives with the lower part. Functions run in ROM order here, lowest
 * address first, under `#pragma defer_codegen off`.
 *
 * Still raw: the func_ and data_ helpers are unnamed in symbols.txt; the
 * pattern table is five separate data symbols (0x0212e1a8..0x0212e1b8), so
 * it is read by byte offset rather than as one array of structs; the words
 * at 0x5370 and 0x5378..0x539b are padding in dScMgAmida_c.h; and the
 * factory builds the object by hand because the class has no constructor
 * declared yet.
 */

#include "types.h"
#include "decl_common.h"
#include "dScMgAmida_c.h"


extern "C" {
/* --- shared ov004 and main helpers --- */
extern void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
extern void func_ov004_020b1e34(void *a0, int a1, int a2, int a3);
extern int  func_ov004_020ae5c4(void *a, int b, int c, int d, int e, int f, int g);
extern void MultiCopyHalf(void *dst, void *src, int nbytes);
extern void *_ZN2G212GetBG3ScrPtrEv(void);  /* decl_common.h declares a data array named G2 */
extern void MultiStore16(u16 val, void *dst, int nbytes);
extern int  RandomIntInternal(int *seed);
extern int  Vec2_Len(int *v);
extern void FreeGfxSlotsById(int a);
extern u32  LoadCompressedFileAt(u16 fileID, void *target);
extern int  LoadFile(int handle);
extern void _ZN3G2x13SetBlendAlphaEPVttttj(void *p, u16 a, u16 b, u16 c, u16 d);
extern void SetSubBg2Offset(int a, int b);
extern void *_ZN7fBase_cnwEj(unsigned int sz);
extern void __cxa_vec_ctor(void *obj, int a, int b, void *cb1, void *cb2);
extern void func_0203d738(void *p);

/* --- the class's own functions that live OUTSIDE this licensed range --- */
extern void func_ov006_020d122c(void *sb, int v);
extern void func_ov006_020d1958(char *c);
extern void func_ov006_020d1a3c(char *c);
extern void func_ov006_020d1ba0(char *c);
extern void func_ov006_020d27dc(char *c);

/* --- the class's own functions defined below --- */
extern void func_ov006_020d3624(void);
extern void func_ov006_020d3668(void *sb);
extern void func_ov006_020d36a4(dScMgAmida_c *self);
extern void func_ov006_020d452c(void *thiz);
extern void func_ov006_020d47f4(char *c);
extern void func_ov006_020d5a50(void);
extern void *dScMgAmida_c_classInit(void);

/* --- data --- */
extern s16 data_02082214[];
extern u8  data_0209d45c;
extern u8  data_0209d454;
extern u8  data_020a0e40[];
extern u8  data_020a0de8[];
extern u8  data_020a0de9[];
extern s32 data_0208ee44;
extern char data_ov006_0212e1a8[];
extern char data_ov006_0212e1ac[];
extern char data_ov006_0212e1b0[];
extern char data_ov006_0212e1b4[];
extern char data_ov006_0212e1b8[];
extern void *data_ov006_0213a32c;
extern void *data_ov006_0213a35c;
extern int  data_ov006_0213a338[];
extern void *data_ov006_0213a390[];
extern void *data_ov006_0213a458[];
extern void *data_ov006_0213a4b0[];
extern int  data_ov006_0213a4c0[];
extern int  data_ov006_0213a568[];
extern int  data_ov006_0213b84c[13];
extern s32  data_ov006_02141640[];
extern s32  data_ov006_02141650[];
}

namespace CP15 {
    void FlushAndInvalidateDataCache(u32 addr, u32 size);
}

namespace GX {
    void LoadBGPltt(const void *data, u32 offset, u32 size);
    void LoadOBJPltt(const void *data, u32 offset, u32 size);
}

namespace GXS {
    void LoadBGPltt(const void *data, u32 offset, u32 size);
    void LoadOBJPltt(const void *data, u32 offset, u32 size);
}

namespace G2S {
    char *GetBG0CharPtr();
    void *GetBG2ScrPtr();
    void *GetBG3ScrPtr();
    char *GetBG3CharPtr();
}

namespace Memory {
    void *Allocate(unsigned int size);
    void Deallocate(void *ptr);
}

typedef struct { int a; int b; } Pair;
extern Pair data_ov006_0213b8b8[11];

typedef struct { int v[14]; } Buf14;
extern Buf14 data_ov006_0213b880;

/* The scene seen through a char pointer, for the free helpers below. */
#define AMIDA(p) ((dScMgAmida_c *)(p))
#define PIECE_AT(p) ((dScMgAmida_c_Piece *)((p) + 0x4768))

/* Launder helpers for Behavior's piece walk: AT makes the compiler
   materialise each address on its own instead of folding it into a
   neighbouring add, which is what the ROM does there. */
#define AT(p,off) ((void*)(int)((char*)(p)+(off)))
#define IP(p,o) (*(int*)((char*)(p)+(o)))
#define IAP(p,o) (*(int*)AT(p,(o)))
#define MULFX(a,b) ((int)(((s64)(a)*(b)+0x800)>>12))

#pragma defer_codegen off

// @symbol func_ov006_020d3624
extern "C" void func_ov006_020d3624(void) {
  char *mainChars = (char *)func_02054efc();
  MultiCopyHalf((char *)func_02054efc() + 0xc000, mainChars, 0x6000);
  char *subChars = G2S::GetBG0CharPtr();
  MultiCopyHalf(G2S::GetBG0CharPtr() + 0x6000, subChars, 0x6000);
}

// @symbol func_ov006_020d3668
/* The scene pointer its caller passes is never read. */
extern "C" void func_ov006_020d3668(void *) {
  char *mainChars = (char *)func_02054efc() + 0xc000;
  MultiCopyHalf((char *)func_02054efc(), mainChars, 0x6000);
  char *subChars = G2S::GetBG0CharPtr() + 0x6000;
  MultiCopyHalf(G2S::GetBG0CharPtr(), subChars, 0x6000);
}

// @symbol func_ov006_020d36a4
extern "C" void func_ov006_020d36a4(dScMgAmida_c *self)
{
    int i;
    int j;
    int k;
    int pick;
    int tmp;
    u32 rand;
    int again;
    u8 seenNow[4];
    u8 seenBefore[4];

    self->unk_53dd = 0;
    if (self->Unk36() != 0) {
        again = 1;
        for (i = 0; i < 4; i++) {
            self->unk_4694[i] = i;
        }
        do {
            for (k = 0; k < 4; k++) {
                rand = (u32)RandomIntInternal(&data_0209d4b8) >> 16;
                pick = ((rand & 0x7fff) * 4) >> 15;
                tmp = self->unk_4694[k];
                self->unk_4694[k] = self->unk_4694[pick];
                self->unk_4694[pick] = tmp;
            }
            if (self->mRoundCount == 0) {
                again = 0;
                switch (self->mPatternIndex) {
                case 0:
                    self->unk_46a4[0] = 0;
                    self->unk_46a4[1] = 0;
                    self->unk_46a4[2] = 3;
                    self->unk_46a4[3] = 3;
                    break;
                case 1:
                case 3:
                case 5:
                    self->unk_46a4[0] = 0;
                    self->unk_46a4[1] = 0;
                    self->unk_46a4[2] = 1;
                    self->unk_46a4[3] = 3;
                    break;
                default:
                    for (i = 0; i < 4; i++) {
                        self->unk_46a4[i] = i;
                    }
                    break;
                }
                for (j = 0; j < 4; j++) {
                    data_ov006_02141640[self->unk_4694[j]] = self->unk_46a4[j];
                }
            } else {
                for (i = 0; i < 4; i++) {
                    data_ov006_02141650[i] = data_ov006_02141640[i];
                }
                for (j = 0; j < 4; j++) {
                    data_ov006_02141640[self->unk_4694[j]] = self->unk_46a4[j];
                }
                for (j = 0; j < 4; j++) {
                    if (data_ov006_02141650[j] != data_ov006_02141640[j]) {
                        again = 0;
                        break;
                    }
                }
            }
        } while (again == 1);
    } else if (self->unk_46c8 == 1) {
        if (self->mRoundCount == 0) {
            pick = ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 4) >> 15;
            tmp = self->unk_4694[0];
            self->unk_4694[0] = self->unk_4694[pick];
            self->unk_4694[pick] = tmp;
        } else {
            pick = (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3) >> 15) + 1;
            tmp = self->unk_4694[0];
            self->unk_4694[0] = self->unk_4694[pick];
            self->unk_4694[pick] = tmp;
        }
    } else {
        again = 1;
        for (int n = 0; n < 4; n++) {
            seenBefore[n] = 0;
        }
        for (j = 0; j < self->unk_46c8; j++) {
            seenBefore[self->unk_4694[j]] = 1;
        }
        do {
            for (int n = 0; n < 4; n++) {
                seenNow[n] = 0;
            }
            for (i = 0; i < 4; i++) {
                self->unk_4694[i] = i;
            }
            for (k = 0; k < 4; k++) {
                rand = (u32)RandomIntInternal(&data_0209d4b8) >> 16;
                pick = ((rand & 0x7fff) * 4) >> 15;
                tmp = self->unk_4694[k];
                self->unk_4694[k] = self->unk_4694[pick];
                self->unk_4694[pick] = tmp;
            }
            for (j = 0; j < self->unk_46c8; j++) {
                seenNow[self->unk_4694[j]] = 1;
            }
            if (self->mRoundCount == 0) {
                again = 0;
            } else {
                for (int n = 0; n < 4; n++) {
                    if (seenNow[n] != seenBefore[n]) {
                        again = 0;
                        break;
                    }
                }
            }
        } while (again == 1);
    }

    for (i = 0; i < 4; i++) {
        if (self->Unk36() != 0) {
            self->unk_46b8[i] = 0;
        } else {
            self->unk_46b8[i] = i * *(s32 *)(data_ov006_0212e1a8 + self->mPatternIndex * 0x1c + 0x14) * 0x3c;
        }
        self->unk_4660[i][0] = (self->unk_4694[i] << 6) + 0x20;
        if (self->Unk36() != 0) {
            self->unk_4660[i][1] = -0xcc;
        } else {
            self->unk_4660[i][1] = -0xd4;
        }
        self->unk_4684[i] = -1;
        self->unk_4680[i] = 0;
        self->unk_46b4[i] = 0;
    }
    self->unk_46cc = 0;
    self->mRoundTimer = 0;
}

// @symbol func_ov006_020d3ba0
#pragma push
#pragma opt_strength_reduction off
extern "C" void func_ov006_020d3ba0(char *raw)
{
    int level;
    int shuffle;
    Pair rungs[11];
    int skip[11];

    AMIDA(raw)->mFinished = 0;
    AMIDA(raw)->unk_46d5 = 0;

    if (AMIDA(raw)->mScore > 0x270f) {
        AMIDA(raw)->mScore = 0x270f;
    }
    shuffle = 0;

    level = AMIDA(raw)->unk_0bc;
    if (AMIDA(raw)->Unk36() != 0) {
        if ((u32) AMIDA(raw)->unk_0bc >= 7) {
            level = 7;
            shuffle = 1;
        }
    } else if (level >= 0xa) {
        shuffle = 1;
        level = (((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5) >> 15) + 4;
        if (level == AMIDA(raw)->mPatternIndex) {
            level = 9;
        }
    }
    AMIDA(raw)->mPatternIndex = level;

    if (AMIDA(raw)->Unk36() != 0) {
        AMIDA(raw)->unk_46c8 = 4;
    } else {
        AMIDA(raw)->unk_46c8 = *(int *)(data_ov006_0212e1a8 + AMIDA(raw)->mPatternIndex * 0x1c);
    }

    AMIDA(raw)->mScrollSpeed = ((*(int *)(data_ov006_0212e1b0 + AMIDA(raw)->mPatternIndex * 0x1c) - 1) * 5) + 0xb;

    {
        u32 stage = AMIDA(raw)->unk_0bc;
        if (stage >= 0x13) {
            AMIDA(raw)->mScrollSpeed = (int)((stage / 10 - 1) * 5) + AMIDA(raw)->mScrollSpeed;
            if (AMIDA(raw)->mScrollSpeed > 0x64) {
                AMIDA(raw)->mScrollSpeed = 0x64;
            }
        }
    }

    {
        char *d;
        volatile u16 v1;
        volatile u16 v2;
        d = (char *)func_02054efc();
        v1 = 0;
        MultiStore16(v1, d, 0x6000);
        d = G2S::GetBG0CharPtr();
        v2 = 0;
        MultiStore16(v2, d, 0x6000);
    }

    AMIDA(raw)->mLineStart.x = -1;
    AMIDA(raw)->mLineStart.y = -1;
    AMIDA(raw)->mLineEnd.x = -1;
    AMIDA(raw)->mLineEnd.y = -1;
    AMIDA(raw)->mPen.x = -1;
    AMIDA(raw)->mPen.y = -1;
    AMIDA(raw)->mPenStart.x = -1;
    AMIDA(raw)->mPenStart.y = -1;

    {
        int z = 0;
        AMIDA(raw)->mLineStartSet = z;
        AMIDA(raw)->mLineEndSet = z;
        AMIDA(raw)->mProbeMode = z;
        AMIDA(raw)->mProbeHit = z;
        AMIDA(raw)->mPenTapped = z;
        AMIDA(raw)->mPenSoundHandle = z;
        do {
            AMIDA(raw)->unk_4694[z] = z;
            z += 1;
        } while (z < 4);
    }

    AMIDA(raw)->mRoundCount = 0;
    func_ov006_020d36a4(AMIDA(raw));
    func_ov006_020d122c(raw, *(int *)(data_ov006_0212e1ac + AMIDA(raw)->mPatternIndex * 0x1c));
    func_ov004_020b04d0(0x20);

    AMIDA(raw)->unk_4707 = 1;
    AMIDA(raw)->mLineCount = 1;
    /* Both buffers are cleared through a raw read of the pointer: indexing
       unk_470c and unk_4710 as members changes the code. */
    {
        int i, j, off;
        for (i = 0, off = 0; i < 0x100; i++, off += 0x158) {
            for (j = 0; j < 0x158; j++) {
                *(*(u8 **)(raw + 0x470c) + off + j) = 0;
                *(*(u8 **)(raw + 0x4710) + off + j) = 0;
            }
        }
    }

    if (AMIDA(raw)->Unk36() != 0) {
        func_ov004_020ae5c4(raw, 0x20, -0xb4, 0x20, AMIDA(raw)->mLineEndY, AMIDA(raw)->unk_53e4, 1);
        func_ov004_020ae5c4(raw, 0x60, -0xb4, 0x60, AMIDA(raw)->mLineEndY, AMIDA(raw)->unk_53e4, 1);
        func_ov004_020ae5c4(raw, 0xa0, -0xb4, 0xa0, AMIDA(raw)->mLineEndY, AMIDA(raw)->unk_53e4, 1);
        func_ov004_020ae5c4(raw, 0xe0, -0xb4, 0xe0, AMIDA(raw)->mLineEndY, AMIDA(raw)->unk_53e4, 1);
    } else {
        func_ov004_020ae5c4(raw, 0x20, -0xd4, 0x20, AMIDA(raw)->mLineEndY, AMIDA(raw)->unk_53e4, 1);
        func_ov004_020ae5c4(raw, 0x60, -0xd4, 0x60, AMIDA(raw)->mLineEndY, AMIDA(raw)->unk_53e4, 1);
        func_ov004_020ae5c4(raw, 0xa0, -0xd4, 0xa0, AMIDA(raw)->mLineEndY, AMIDA(raw)->unk_53e4, 1);
        func_ov004_020ae5c4(raw, 0xe0, -0xd4, 0xe0, AMIDA(raw)->mLineEndY, AMIDA(raw)->unk_53e4, 1);
    }

    AMIDA(raw)->mLineCount++;
    {
        typedef struct { Pair e[11]; } Blk88;
        *(Blk88 *)rungs = *(Blk88 *)data_ov006_0213b8b8;
    }

    {
        int i;
        for (i = 0; i < 0xb; i++) {
            skip[i] = 0;
        }
    }

    if (shuffle == 1) {
        if (AMIDA(raw)->Unk36() != 0) {
            int msk = 0x7fff;
            int cnt = 3;
            do {
                int idx = ((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & msk) * 0xb) >> 15;
                if (skip[idx] == 0) {
                    cnt--;
                    skip[idx] = 1;
                }
            } while (cnt > 0);
        } else {
            int sel = *(int *)(data_ov006_0212e1b4 + AMIDA(raw)->mPatternIndex * 0x1c);
            switch (sel) {
            case 0:
                break;
            case 1:
                skip[((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5) >> 15] = 1;
                break;
            case 2: {
                int cnt = (((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3) >> 15) + 1;
                int j = 0;
                if (cnt > 0) {
                    int *seedp2 = &data_0209d4b8;
                    do {
                        skip[((u32)(((u32)RandomIntInternal(seedp2) >> 16) & 0x7fff) * 0xb) >> 15] = 1;
                        j++;
                    } while (j < cnt);
                }
                break;
            }
            default:
                break;
            }
        }
    }

    {
        int sel2;
        if (AMIDA(raw)->Unk36() != 0) {
            switch (AMIDA(raw)->mPatternIndex) {
            case 0:
            case 1:
            case 2:
                sel2 = 0;
                break;
            case 3:
            case 4:
                sel2 = 1;
                break;
            default:
                sel2 = 2;
                break;
            }
        } else {
            sel2 = *(int *)(data_ov006_0212e1b4 + AMIDA(raw)->mPatternIndex * 0x1c);
        }

        switch (sel2) {
        case 0:
            break;
        case 1: {
            int i;
            for (i = 0; i < 5; i++) {
                if (skip[i] == 0) {
                    int x = rungs[i].a;
                    int y = rungs[i].b - 0x20;
                    func_ov004_020ae5c4(raw, x, y, x + 0x40, y, AMIDA(raw)->unk_53e4, 1);
                }
            }
            break;
        }
        case 2:
        case 3: {
            int i;
            for (i = 0; i < 0xb; i++) {
                if (skip[i] == 0) {
                    int x = rungs[i].a;
                    int y = rungs[i].b - 0x20;
                    func_ov004_020ae5c4(raw, x, y, x + 0x40, y, AMIDA(raw)->unk_53e4, 1);
                }
            }
            break;
        }
        default:
            break;
        }
    }

    AMIDA(raw)->mLineCount++;
    if (AMIDA(raw)->Unk36() == 0) {
        int v = *(int *)(data_ov006_0212e1b8 + AMIDA(raw)->mPatternIndex * 0x1c);
        if (v != 0 && v == 1) {
            func_ov004_020ae5c4(raw, 0x60, 0x2d, 0xa0, 0x2d, AMIDA(raw)->unk_53e4, 1);
            func_ov004_020ae5c4(raw, 0x20, 0x5a, 0x60, 0x5a, AMIDA(raw)->unk_53e4, 1);
            func_ov004_020ae5c4(raw, 0xa0, 0x5a, 0xe0, 0x5a, AMIDA(raw)->unk_53e4, 1);
        }
    }

    AMIDA(raw)->unk_4707 = 0;
    AMIDA(raw)->mLineCount++;
    {
        int i, j, off;
        for (i = 0, off = 0; i < 0x100; i++, off += 0x158) {
            for (j = 0; j < 0x158; j++) {
                *(*(u8 **)(raw + 0x470c) + off + j) = 0;
            }
        }
    }

    func_ov006_020d3668(raw);
    *(s32 *)(raw + 0x5370) = 1;
    *(volatile u16 *)0x04000050 = 0;
    _ZN3G2x13SetBlendAlphaEPVttttj((void *)0x04001050, 4, 8, 6, 0x10);
    func_ov004_020b0cac(0xd, 0x80, 0x60, 1, -1, 0xd);

    AMIDA(raw)->mStartBannerTimer = 0x3c;
    AMIDA(raw)->mState = 1;
    {
        int i = 0;
        for (; i < 4; i++) {
            ((s32 *)(raw + 0x5378))[i] = 0;
            ((s32 *)(raw + 0x5388))[i] = 0;
            ((u8 *)(raw + 0x5398))[i] = 0;
            AMIDA(raw)->mLaneAnimTimer[i] = 0;
            AMIDA(raw)->mLaneAnimFrame[i] = 0;
        }
        AMIDA(raw)->mBgScrollPhase = 0;
        AMIDA(raw)->mEndDelayTimer = 0;
        *(s32 *)(raw + 0x53d8) = 0;
        AMIDA(raw)->unk_53dc = 0;
        AMIDA(raw)->unk_53dd = 0;
        AMIDA(raw)->unk_53de = 0;
        {
            int z = 0;
            int xw[2];
            xw[0] = 0x20;
            xw[1] = 0;
            for (; z < 4; z++) {
                AMIDA(raw)->mLanePos[z][0] = xw[0] << 12;
                AMIDA(raw)->mLanePos[z][1] = 0xb0000;
                AMIDA(raw)->mLaneVel[z][0] = xw[1];
                AMIDA(raw)->mLaneVel[z][1] = xw[1];
                xw[0] += 0x40;
            }
            AMIDA(raw)->unk_4764 = xw[1];
            {
                int v = 0;
                for (; v < 0x80; v++) {
                    AMIDA(raw)->mPieces[0].active = 0;
                    AMIDA(raw)->mPieces[0].timer = 0;
                    raw += 0x18;
                }
            }
        }
    }
}
#pragma pop

// @symbol func_ov006_020d452c
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
extern "C" void func_ov006_020d452c(void *scene)
{
    typedef struct { int a[13]; } T13;
    unsigned char *raw = (unsigned char *)scene;
    int noPalette = -1;
    int i;
    int y;

    i = 0;
    y = 0x20;
    for (; i < 4; y += 0x40, i++) {
        int *lane = (int *)(raw + i * 4);
        if (*(int *)((char *)lane + 0x4714) != 0) {
            int *animTimer = (int *)(((int)lane + 0x539c));
            (*animTimer)++;
            if (*animTimer >= 6) {
                int *animFrame = (int *)(((int)lane + 0x53ac));
                *animTimer = 0;
                (*animFrame)++;
                if (*animFrame >= 14)
                    *animFrame = 0;
            }
            func_ov004_020afdd0(data_ov006_0213a458[AMIDA(raw)->mLaneAnimFrame[i]],
                                AMIDA(raw)->mLanePos[i][0] >> 12,
                                (AMIDA(raw)->mLanePos[i][1] >> 12) - 4,
                                noPalette, 0);
        } else {
            int *timer;
            int *frame;
            int frames[13];
            if (*(raw + i + 0x5398) == 0) {
                timer = (int *)(((int)lane + 0x5378));
                frame = (int *)(((int)lane + 0x5388));
                (*timer)++;
                if (*frame >= 12) {
                    if (*timer >= 6) {
                        *timer = 0;
                        *frame = 0;
                    }
                } else if (*timer >= 4) {
                    *timer = 0;
                    (*frame)++;
                }
            } else {
                timer = (int *)(((int)lane + 0x5378));
                frame = (int *)(((int)lane + 0x5388));
                (*timer)++;
                if (*frame >= 12) {
                    if (*timer >= 5) {
                        *timer = 0;
                        *frame = 0;
                    }
                } else if (*timer >= 2) {
                    *timer = 0;
                    (*frame)++;
                }
            }
            *(T13 *)frames = *(T13 *)data_ov006_0213b84c;
            func_ov004_020afdd0(data_ov006_0213a390[frames[*frame]],
                                y, 0xb8, noPalette, 0);
        }
    }
    {
        int j;
        for (j = 0; j < AMIDA(raw)->unk_46c8; j++) {
            func_ov004_020afdd0(data_ov006_0213a32c,
                                AMIDA(raw)->unk_4660[j][0],
                                AMIDA(raw)->unk_4660[j][1],
                                -1, 0);
        }
    }
    func_ov004_020afdd0(data_ov006_0213a35c, 8, 0x60, -1, 0);
    func_ov004_020afdd0(data_ov006_0213a35c, 0xf8, 0x60, -1, 0);
}
#pragma pop

// @symbol func_ov006_020d47f4
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
extern "C" void func_ov006_020d47f4(char *raw){
  int noPalette = -1;
  int i;
  int j;
  int x;
  if(AMIDA(raw)->unk_53dd==0){
    func_ov004_020afdd0((void*)data_ov006_0213a338[0],0x80,0xb0,noPalette,0);
  }
  for(i=0;i<4;i++){
    func_ov004_020afdd0((void*)data_ov006_0213a568[AMIDA(raw)->unk_46a4[i]],
                        AMIDA(raw)->unk_4660[i][0],
                        AMIDA(raw)->unk_4660[i][1],
                        noPalette,0);
  }
  j = 0;
  x = 0x20;
  for(; j < 4; x += 0x40, j++){
    func_ov004_020afdd0((void*)data_ov006_0213a4c0[AMIDA(raw)->unk_4714[j]],
                        x,0x78,noPalette,0);
  }
}
#pragma pop

// @symbol _ZN12dScMgAmida_c6RenderEv
/* These two pragmas are load-bearing: without them Render compiles 0xc
 * bytes larger than the ROM (0x2ac vs 0x2a0) and rombuild drops to 102/106. */
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
s32 dScMgAmida_c::Render()
{
    char *raw = (char *)this;

    mBgScrollPhase += 0xc0;
    {
        u16 idx = mBgScrollPhase;
        int sine = data_02082214[(idx >> 4) << 1];
        int off = (sine + (int)((unsigned)(sine >> 7) >> 24)) >> 8;
        SetSubBg2Offset(off, 0);
    }
    func_ov004_020b1e34(raw, 0xe0, 0x14, 1);

    if (mState == 3 && mEndDelayTimer == 0) {
        data_0209d45c &= ~1;
        data_0209d454 &= ~1;
        if (Unk36() == 0 && mFinished == 1) {
            int i;
            for (i = 0; i < 4; i++) {
                if (unk_4714[i] != 0) {
                    int *counterA = (int*)(raw + i * 4 + 0x539c);
                    (*counterA)++;
                    Buf14 periods = data_ov006_0213b880;
                    if (*counterA >= periods.v[i]) {
                        int *counterB;
                        *counterA = 0;
                        counterB = (int*)(raw + i * 4 + 0x53ac);
                        (*counterB)++;
                        if (*counterB >= 0xe)
                            *counterB = 0;
                    }
                    func_ov004_020afdd0(
                        data_ov006_0213a458[mLaneAnimFrame[i]],
                        mLanePos[i][0] >> 12,
                        (mLanePos[i][1] >> 12) - 4,
                        -1,
                        0);
                }
            }

            int piece = 0;
            int zero = 0;
            int noPalette = -1;
            for (; piece < 0x80; piece++, raw += 0x18) {
                if (*(u8*)(raw + 0x477c) != 0) {
                    int age = *(int*)(raw + 0x4778);
                    int x = *(int*)(raw + 0x4768);
                    int idx = age / 4;
                    int y = *(int*)(raw + 0x476c);
                    func_ov004_020afdd0(data_ov006_0213a4b0[idx], x >> 12, y >> 12, noPalette, zero);
                }
            }

        }
        return 1;
    } else {
        data_0209d45c |= 1;
        data_0209d454 |= 1;
        if (Unk36() != 0) {
            func_ov006_020d47f4(raw);
        } else {
            func_ov006_020d452c(raw);
        }
        return 1;
    }
}
#pragma pop

// @symbol _ZN12dScMgAmida_c8BehaviorEv
#pragma push
#pragma opt_strength_reduction off
s32 dScMgAmida_c::Behavior()
{
    char *raw = (char *)this;
    int piece;
    char *piecePtr;
    int *vel;
    int speed;
    int steps;
    int k;
    int accum;
    int touch, j4, touched;
    int lane;
    int j;
    int slot;
    char *scan;
    int randA, randB;
    u32 bv, av;
    s16 cA;
    int idxA;
    int cosB, sinA;

    switch (mState) {
    case 0:
        if (mFinished == 1) {
            func_ov006_020d3ba0((char *)this);
        } else if (unk_46d5 == 1) {
            func_ov006_020d3ba0((char *)this);
        } else {
            func_ov006_020d36a4(this);
        }
        mState = 1;
        // fall through
    case 1:
        func_ov006_020d1a3c(raw);
        func_ov006_020d1958(raw);
        func_ov006_020d1ba0(raw);
        if (Unk36() != 0) {
            if (unk_53dd == 1) {
                mScrollAccum += mRoundCount * 5 + 0x20;
            }
        } else if (unk_53dc == 1) {
            mScrollAccum += 0x64;
        } else {
            mScrollAccum += mScrollSpeed;
        }
        accum = mScrollAccum;
        mScrollAccum &= 0xf;
        steps = accum / 16;
        if (Unk36() == 0 || unk_53dd != 0) {
            if (mRoundTimer > 0) {
                mRoundTimer -= 1;
                if (mRoundTimer == 0) {
                    if (unk_46d5 == 1) {
                        mResultWaitTimer = 0x3c;
                        mState = 2;
                    } else if (mRoundCount < 5) {
                        mState = 0;
                    } else {
                        mResultWaitTimer = 0x3c;
                        mState = 2;
                        mFinished = 1;
                    }
                }
            }
            k = 0;
            if (steps > 0) {
                do {
                    if (mState != 1)
                        break;
                    func_ov006_020d27dc(raw);
                    k++;
                } while (k < steps);
            }
        }
        if (mStartBannerTimer > 0) {
            mStartBannerTimer -= 1;
            if (mStartBannerTimer <= 0) {
                FreeGfxSlotsById(0xd);
                if (mPromptBlinkCount == 0) {
                    mPromptEnabled = 1;
                    mPromptBlinkCount = 1;
                    mPromptBlinkTimer = 0;
                }
            }
        }
        func_ov004_020adb1c(mScore);
        mHudScore = mScore;
        break;
    case 2:
        unk_53dc = 0;
        if (Unk36() != 0) {
            if (mResultWaitTimer > 0)
                mResultWaitTimer -= 1;
        } else {
            if (mResultWaitTimer > 0)
                mResultWaitTimer -= 1;
        }
        if (mResultWaitTimer != 0)
            break;
        mPromptEnabled = 0;
        if (mFinished == 1) {
            mState = 3;
            func_ov004_020b0a54(0);
        } else {
            mState = 3;
            func_ov004_020b0a54(0x12);
            mEndDelayTimer = 0xb4;
            unk_4764 = 0;
        }
        break;
    case 3:
        if (mEndDelayTimer > 0)
            mEndDelayTimer -= 1;
        touched = 0;
        touch = data_020a0e40[0];
        j4 = touch * 4;
        if (data_020a0de8[touch * 4] != 0) {
            if (data_020a0de9[j4] != 0)
                touched = 1;
        }
        if (touched != 0)
            mEndDelayTimer = 0;
        if (Unk36() != 0)
            break;
        if (mFinished != 1)
            break;
        unk_4764 += 1;
        lane = 0;
        do {
            mLaneVel[lane][1] -= 0x100;
            mLanePos[lane][0] += mLaneVel[lane][0];
            mLanePos[lane][1] += mLaneVel[lane][1];
            lane++;
        } while (lane < 4);

        piecePtr = raw;
        vel = &mPieces[0].velX;
        for (piece = 0; piece < 0x80; piece++) {
            if (PIECE_AT(piecePtr)->active != 0) {
                IAP(piecePtr, 0x4768) += IP(piecePtr, 0x4770);
                IAP(piecePtr, 0x476c) += IP(piecePtr, 0x4774);
                speed = Vec2_Len(vel) * 7 / 8;
                if (func_0203d434(vel) != 0)
                    func_0203d630(vel, speed);
                *(int*)AT(piecePtr, 0x4778) = *(int*)AT(piecePtr, 0x4778) + 1;
                {
                    if (PIECE_AT(piecePtr)->timer >= 0x10)
                        PIECE_AT(piecePtr)->active = 0;
                }
            }
            piecePtr += 0x18;
            vel += 6;
        }

        for (j = 0; j < 4; j++) {
            if (unk_4714[j] != 0) {
                slot = 0;
                scan = raw;
                do {
                    if (PIECE_AT(scan)->active != 1) {
                        randA = RandomIntInternal(&data_0209d4b8);
                        randB = RandomIntInternal(&data_0209d4b8);
                        bv = ((u32)randB >> 16) & 0x7fff;
                        av = ((u32)randA >> 16) & 0x7fff;
                        cosB = data_02082214[((int)((bv << 15) >> 16) >> 4) * 2 + 1];
                        idxA = ((int)((av << 17) >> 16) >> 4) * 2;
                        cA = data_02082214[idxA + 1];
                        {
                            mPieces[slot].posX = MULFX(cosB, (int)((0x8000LL * cA + 0x800) >> 12));
                            sinA = data_02082214[idxA];
                            mPieces[slot].posY = MULFX(cosB, (int)((0x8000LL * sinA + 0x800) >> 12));
                            mPieces[slot].posX += mLanePos[j][0];
                            mPieces[slot].posY += mLanePos[j][1];
                            mPieces[slot].velX = MULFX(cosB, (int)((0x1000LL * (int)(cA) + 0x800) >> 12));
                            mPieces[slot].velY = MULFX(cosB, (int)((0x1000LL * sinA + 0x800) >> 12));
                        }
                        mPieces[slot].velY -= 0x400;
                        mPieces[slot].active = 1;
                        mPieces[slot].timer = 0;
                        break;
                    }
                    slot++;
                    scan += 0x18;
                } while (slot < 0x80);
            }
        }
        break;
    }
    return 1;
}
#pragma pop

// @symbol _ZN12dScMgAmida_c13OnYoshiTryEatEi
/* Slot 18: counts a win (arg 0) or resets the score, then sets up the next board. */
void dScMgAmida_c::OnYoshiTryEat(int arg)
{
    if (arg == 0) {
        if (unk_0bc >= 0x7cf)
            goto final;
        unk_0bc = unk_0bc + 1;
        if (unk_0bc > 0x270e)
            unk_0bc = 0x270e;
    } else {
        mScore = 0;
        unk_0bc = 0;
        if (unk_0bc > 0x270e)
            unk_0bc = 0x270e;
        func_ov004_020adb1c(mScore);
        mHudScore = mScore;
    }
final:
    func_ov006_020d3ba0((char *)this);
}

// @symbol _ZN12dScMgAmida_c13InitResourcesEv
s32 dScMgAmida_c::InitResources()
{
    volatile u16 zeroMain;
    volatile u16 zeroSub;
    void *file;

    unk_470c = (u8 *)Memory::Allocate(0x15800);
    unk_4710 = (u8 *)Memory::Allocate(0x15800);

    if (Unk36() != 0) {
        mLineEndY = 0x78;
        unk_53e4 = 2;
    } else {
        mLineEndY = 0x98;
        unk_53e4 = 2;
    }

    data_0208ee44 = 1;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & 0x43) | 0x3218;
    *(volatile u16 *)0x4000008 &= ~0x40;
    *(volatile s32 *)0x4000010 = 0;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & ~3) | 1;

    file = func_02054efc();
    zeroMain = 0;
    MultiStore16((u16)zeroMain, file, 0x6000);

    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x2214;
    *(volatile u16 *)0x4001008 &= ~0x40;
    *(volatile s32 *)0x4001010 = 0;
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & ~3) | 1;

    file = G2S::GetBG0CharPtr();
    zeroSub = 0;
    MultiStore16((u16)zeroSub, file, 0x6000);

    func_ov004_020af2f8((char *)this, 1, 0, 2);

    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x140c;
    *(volatile u16 *)0x400000e &= ~0x40;
    *(volatile s32 *)0x400001c = 0;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 3;

    if (Unk36() != 0) {
        LoadCompressedFileAt(0x1f, (void *)func_02054d88());
        file = (void *)LoadFile(0x20);
        CP15::FlushAndInvalidateDataCache((u32)file, 0x1a0);
        GX::LoadBGPltt(file, 0x60, 0x1a0);
        Deallocate(file);
        LoadCompressedFileAt(0x21, _ZN2G212GetBG3ScrPtrEv());
    } else {
        LoadCompressedFileAt(0x15, (void *)func_02054d88());
        file = (void *)LoadFile(0x16);
        CP15::FlushAndInvalidateDataCache((u32)file, 0x1a0);
        GX::LoadBGPltt(file, 0x60, 0x1a0);
        Deallocate(file);
        LoadCompressedFileAt(0x17, _ZN2G212GetBG3ScrPtrEv());
    }

    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x40c;
    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile s32 *)0x4001018 = 0;
    *(volatile u16 *)0x400100c &= ~3;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x60c;
    *(volatile u16 *)0x400100e &= ~0x40;
    *(volatile s32 *)0x400101c = 0;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 3;

    if (Unk36() != 0) {
        LoadCompressedFileAt(0x1c, G2S::GetBG3CharPtr());
        file = (void *)LoadFile(0x1d);
        GXS::LoadBGPltt(file, 0x60, 0x1a0);
        Deallocate(file);
        LoadCompressedFileAt(0x1e, G2S::GetBG3ScrPtr());
        data_0209d45c |= 9;
        data_0209d454 |= 9;
    } else {
        LoadCompressedFileAt(0x1a, G2S::GetBG3CharPtr());
        file = (void *)LoadFile(0x1b);
        GXS::LoadBGPltt(file, 0x60, 0x1a0);
        Deallocate(file);
        LoadCompressedFileAt(0x19, G2S::GetBG2ScrPtr());
        LoadCompressedFileAt(0x18, G2S::GetBG3ScrPtr());
        *(volatile s16 *)0x4000050 = 0;
        _ZN3G2x13SetBlendAlphaEPVttttj((void *)0x4001050, 4, 8, 6, 0x10);
        data_0209d45c |= 9;
        data_0209d454 |= 0xd;
    }

    if (Unk36() != 0) {
        if (GetOwnerLanguage() == 5) {
            LoadCompressedFileAt(0xf, (void *)0x6400000);
            LoadCompressedFileAt(0xf, (void *)0x6600000);
        } else if (GetOwnerLanguage() == 4) {
            LoadCompressedFileAt(0xc, (void *)0x6400000);
            LoadCompressedFileAt(0xc, (void *)0x6600000);
        } else if (GetOwnerLanguage() == 3) {
            LoadCompressedFileAt(0xb, (void *)0x6400000);
            LoadCompressedFileAt(0xb, (void *)0x6600000);
        } else if (GetOwnerLanguage() == 2) {
            LoadCompressedFileAt(0xa, (void *)0x6400000);
            LoadCompressedFileAt(0xa, (void *)0x6600000);
        } else {
            LoadCompressedFileAt(0xd, (void *)0x6400000);
            LoadCompressedFileAt(0xd, (void *)0x6600000);
        }
        file = (void *)LoadFile(0xe);
        GX::LoadOBJPltt(file, 0, 0x100);
        GXS::LoadOBJPltt(file, 0, 0x100);
        Deallocate(file);
    } else {
        LoadCompressedFileAt(0x10, (void *)0x6400000);
        LoadCompressedFileAt(0x10, (void *)0x6600000);
        file = (void *)LoadFile(0x11);
        GX::LoadOBJPltt(file, 0, 0x100);
        GXS::LoadOBJPltt(file, 0, 0x100);
        Deallocate(file);
    }

    mPatternIndex = 0;
    mScore = unk_0bc * 5;
    func_ov006_020d3ba0((char *)this);
    return 1;
}

// @symbol _ZN12dScMgAmida_c21AfterCleanupResourcesEj
/* Returns before the base call too when vfSuccess != 2, as the ROM does. */
void dScMgAmida_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess != 2)
        return;
    Memory::Deallocate(unk_470c);
    Memory::Deallocate(unk_4710);
    dScMgBase_c::AfterCleanupResources(vfSuccess);
}

// @symbol dScMgAmida_c_classInit
/* Builds the scene by hand (operator new, base constructor, vtable,
   member arrays) because the class has no constructor declared yet.
   The vtable address is stored as data: this TU does not own it. */
extern "C" void *dScMgAmida_c_classInit(void) {
    char *scene = (char *)_ZN7fBase_cnwEj(0x53fc);
    if (scene != 0) {
        _ZN11dScMgBase_cC2Ev(scene);
        *(int *)scene = (int)&data_ov006_0213b918;
        __cxa_vec_ctor(scene + 0x4660, 4, 8, (void *)func_0203d738, (void *)NullDestructor_0203d47c);
        __cxa_vec_ctor(scene + 0x4724, 4, 8, (void *)func_0203d738, (void *)NullDestructor_0203d47c);
        __cxa_vec_ctor(scene + 0x4744, 4, 8, (void *)func_0203d738, (void *)NullDestructor_0203d47c);
        __cxa_vec_ctor(scene + 0x4768, 0x80, 0x18, (void *)func_ov006_020d5a50, (void *)func_ov006_020d116c);
    }
    return scene;
}

// @symbol func_ov006_020d5a50
/* Element constructor for the 0x80 falling pieces. */
extern "C" void func_ov006_020d5a50(void)
{
}
