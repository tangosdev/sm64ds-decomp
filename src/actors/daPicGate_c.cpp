//cpp
/* ov080/daPicGate_c -- picture gate (PICTURE_GATE 307).
 *
 * param1 low nibble is the width in 100-unit steps, minus one. The next
 * nibble is the height. Bits 8-12 are the picture id. Bits 13-14 pick one
 * of the four rows in the state table.
 *
 * State 0 is the ripple mesh: fill the grid, count the wave down, draw a
 * triangle strip. State 1 fills that mesh and pulses the center, then each
 * frame slides picture 7 from mClosedPosX by 2050 once star 1 of level
 * 0x12 is in, and latches bit 0x40000 of data_0209caa0[2]. The same shift
 * is what daChScene_c applies to an exit whose high param byte is 0x12.
 * Picture 4 in state 1 clears the two clip-enable flags. States 2 and 3
 * are the flat 2x2 quad; their other handlers are outside this run.
 *
 * The grid is square (mCols is copied from mRows) and indexed with mRows
 * as the stride. Vertex::color is the packed GX normal, not a color: the
 * strip writer stores it at 0x04000484.
 *
 * The five handlers are daPicGate_c members bound through the four
 * data_ov080_02128628 state rows; they keep func_ov080_* address-label
 * names. #pragma defer_codegen off keeps .text in
 * source order. The destructor pair and g_profile_PICTURE_GATE are outside
 * this run. The abutting registry factory daPicGate_c_classInit
 * (0x02126f8c) is written last, built by hand (see the note above it).
 * SetRanges is the int adapter; dActor_c.h does not declare the member.
 *
 * deslop leftovers:
 * - func_ov080_021264ec and func_ov080_021269b8: mWavePhase += phaseStep
 *   differs by 6 words. The add that matches reuses the r5+0x100 base
 *   already formed for mNumCells; += addresses mWavePhase at 0x1b4.
 * - InitResources: a picture-id local, and declaring the default wave
 *   row as a WaveParams, shrank the function from 0x2ec to 0x2b4. The
 *   bit extracts stay written out, and the row stays an untyped object
 *   whose address is cast.
 * - func_ov080_0212677c: for-loops over the columns and the rows grew
 *   it from 0x23c to 0x24c. The entry test is still n = mCols; i = 0;
 *   n = n - 1, so the subtract consumes that n, and both loops stay
 *   do/while.
 */

#pragma defer_codegen off

#include "daPicGate_c.h"
#include "SharedFilePtr.h"
#include "common.h"

/* Geometry command ports. Plain stores, not volatile: a volatile port
 * reloads where the cartridge keeps the value in a register. */
#define G3_MTX_MODE     (*(int *)0x04000440)
#define G3_MTX_PUSH     (*(int *)0x04000444)
#define G3_MTX_POP      (*(int *)0x04000448)
#define G3_MTX_SCALE    (*(int *)0x0400046c)
#define G3_NORMAL       (*(int *)0x04000484)
#define G3_TEXCOORD     (*(int *)0x04000488)
#define G3_VTX_16       (*(int *)0x0400048c)
#define G3_LIGHT_VECTOR (*(int *)0x040004c8)
#define G3_LIGHT_COLOR  (*(int *)0x040004cc)
#define G3_BEGIN        (*(int *)0x04000500)
#define G3_END          (*(int *)0x04000504)

enum {
    kCell = 0x64000,         /* 100.0, one width or height step */
    kSlide = 0x802000,       /* 2050.0 */
    kSlideStep = 0x13e72,
    kOpened = 0x40000,
    kFlatNormal = 0x1ff00000,
    kTexSpan = 0x80000,
    kRangePad = 0xc8000,     /* 200.0, added to the clip radius */
    kFar = 0x1964000,        /* 6500.0, clip distance and far distance */
    kPicSlide = 7,
    kPicUnclipped = 4,
    kGateLevel = 0x12,
    kGateStar = 1,
    kMtxPosVec = 2,          /* GX_MTXMODE_POSITION_VECTOR */
    kMtxPos = 1,             /* GX_MTXMODE_POSITION */
    kScale = 0x20000,        /* 32.0, the three MTX_SCALE parameters */
    kTriStrip = 2,
    kClipEnable = 3
};

extern "C" {
extern int IsStarCollectedInLevel(signed char levelID, int starID);
extern int data_0209caa0[];
extern void MulMat4x3Mat4x3(const int *a, const int *b, int *out);
extern Matrix4x3 data_0209b3ec;
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(
    void *self, int offsetY, int radius, int clip, int far);
extern u8 data_ov080_02127714[];
extern void func_020553a4(void *mtx);
extern void *data_ov080_02127834;
extern void func_0203cbc0(void *a);
/* daPicGate_c_classInit builds the object by hand; see the note above it. */
extern void *_ZN8dActor_cC2Ev(void *self);
}

/* The cartridge's vtable label is the address point (slot 0), and the
   vtable itself is emitted with the destructor, outside this file. */
extern int _ZTV11daPicGate_c[];
namespace Memory { void *operator_new2(unsigned int size); }

/* Call-site names. The state-table handlers keep their func_ov080_* names;
 * the helpers are daPicGate_c members declared on the class and defined in
 * src/game/actors/d_a_pic_gate.cpp. */
#define HitTest func_ov080_021256f8
#define RippleHeight func_ov080_02125bb0
#define BuildNormals func_ov080_02125940
#define FlattenFrame func_ov080_02125af0
#define PlaceCorners func_ov080_02126124
#define BeginWave func_ov080_02125de0
#define DrawFlat func_ov080_02125fd0
#define LoadMaterial func_ov080_02125460
#define BuildGateMatrix func_ov080_0212555c
#define LoadTexture func_ov080_02125630
#define LoadMtx43 func_020553a4
#define SetRanges _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_
#define kSave data_0209caa0
#define kStates data_ov080_02128628
#define kDefaultWave data_ov080_02127834

extern daPicGate_c::State data_ov080_02128628[];
int ApproachLinear(int &ref, int target, int step);

/* A picture's texture/collision handle. The nineteen instances below are
   constructed by __sinit_daPicGate_c.cpp and reached through the rodata
   pointer table data_ov080_0212775c, indexed by picture id. The declared
   ctor/dtor stand for the cartridge's func_020178cc/func_020178b4 pair;
   words[] is the two words the ROM object carries. */
struct PicGateFilePtr : SharedFilePtr {
    u32 words[2];

    PicGateFilePtr(u32 fileID);
    ~PicGateFilePtr();
};

/* Rows for widths 3, 4, 5, 6, 7, 8 and 16: 13, 14, 16, 20, 20, 25, 30. */
#define kGridRows data_ov080_02127714

/* One vertex of a strip: texcoord, packed normal, then the 4.12 position
 * as two VTX_16 writes. vx/vy/vz/s0/s1/s2 are the caller's locals. */
#define EMIT_VTX(v) \
    G3_TEXCOORD = (v)->texCoord; \
    G3_NORMAL = (v)->color; \
    vx = (v)->x; \
    vy = (v)->y; \
    vz = (v)->z; \
    s0 = (s16)(vx >> 8); \
    s1 = (s16)(vy >> 8); \
    s2 = (s16)(vz >> 8); \
    G3_VTX_16 = (u16)s0 | ((u16)s1 << 16); \
    G3_VTX_16 = (u16)s2

// @symbol _ZN11daPicGate_c19func_ov080_021264ecEv
/* State 1 behavior. Picture 7 slides open; every picture then ripples. */
void daPicGate_c::func_ov080_021264ec()
{
    if ((u8)((this->param1 >> 8) & 0x1f) == kPicSlide &&
        !(kSave[2] & kOpened) &&
        IsStarCollectedInLevel(kGateLevel, kGateStar)) {
        if (ApproachLinear(this->mPosX, this->mClosedPosX + kSlide, kSlideStep))
            kSave[2] |= kOpened;
        this->BuildGateMatrix();
    }
    this->HitTest();
    int i;
    for (i = 0; i < this->mNumCells; i++) {
        daPicGate_c::Vertex *v = &this->mCells[i];
        v->z = this->RippleHeight(v->dist);
    }
    this->BuildNormals();
    this->FlattenFrame();
    this->mWavePhase = this->mWavePhase + this->mWaveParams->phaseStep;
}

// @symbol _ZN11daPicGate_c19func_ov080_021265ecEv
/* State 1 init. Even grid, flat +Z normal, then a wave at the centre. */
void daPicGate_c::func_ov080_021265ec()
{
    int x = 0, y = 0, row = 0, z = 0;
    int n = this->mRows;
    int rows;
    if (n > 0) {
        int color = kFlatNormal;
        do {
            int col = 0;
            int m = this->mCols;
            if (m > 0) {
                do {
                    daPicGate_c::Vertex *v = &this->mCells[row * this->mRows + col];
                    int cols;
                    v->x = x; v->y = y; v->z = z; v->color = color;
                    cols = this->mCols;
                    if (col == cols - 2) x = ((u8)(this->param1 & 0xf) + 1) * kCell;
                    else x += ((u8)(this->param1 & 0xf) + 1) * kCell / (cols - 1);
                    col++;
                } while (col < this->mCols);
            }
            rows = this->mRows;
            x = 0;
            if (row == rows - 2) y = ((u8)((this->param1 >> 4) & 0xf) + 1) * kCell;
            else y += ((u8)((this->param1 >> 4) & 0xf) + 1) * kCell / (rows - 1);
            row++;
        } while (row < rows);
    }
    this->PlaceCorners();
    {
        int width = ((u8)(this->param1 & 0xf) + 1) * kCell;
        int height = ((u8)((this->param1 >> 4) & 0xf) + 1) * kCell;
        this->BeginWave(width / 2, height / 2, 0);
    }
}

// @symbol _ZN11daPicGate_c19func_ov080_0212677cEv
/* State 0 render. A dead wave draws the flat quad; otherwise a strip per column. */
void daPicGate_c::func_ov080_0212677c()
{
    int tmp[12];
    int i, j, n;
    int z = 0;

    if (this->mWaveTimer == 0) {
        this->DrawFlat();
        return;
    }

    G3_MTX_PUSH = z;
    MulMat4x3Mat4x3(this->mMtx, data_0209b3ec.m, tmp);
    G3_MTX_MODE = kMtxPosVec;
    LoadMtx43(tmp);
    G3_MTX_MODE = kMtxPos;
    LoadMtx43(tmp);

    G3_LIGHT_VECTOR = 0xe0000000;
    G3_LIGHT_COLOR = 0xc0007fff;
    this->LoadMaterial();

    G3_MTX_SCALE = kScale;
    G3_MTX_SCALE = kScale;
    G3_MTX_SCALE = kScale;

    n = (int)this->mCols;
    i = z; /* zero the column before subtracting, so the sub consumes n */
    n = n - 1;
    if (n > 0) {
        do {
            G3_BEGIN = kTriStrip;
            j = z;
            if ((int)this->mRows > 0) {
                do {
                    daPicGate_c::Vertex *base = this->mCells;
                    int rows = (int)this->mRows;
                    daPicGate_c::Vertex *v1 = &base[i + j * rows];
                    daPicGate_c::Vertex *v2 = &base[(i + 1) + j * rows];
                    int vx, vy, vz;
                    s16 s0, s1, s2;

                    EMIT_VTX(v1);
                    EMIT_VTX(v2);

                    j++;
                } while (j < (int)this->mRows);
            }
            G3_END = z;
            i++;
        } while (i < (int)this->mCols - 1);
    }
    G3_MTX_POP = 1;
}

// @symbol _ZN11daPicGate_c19func_ov080_021269b8Ev
/* State 0 behavior. Ripples only while mWaveTimer is still counting. */
void daPicGate_c::func_ov080_021269b8()
{
    int i;
    daPicGate_c::Vertex *v;

    this->HitTest();
    if (DecIfAbove0_Short(&this->mWaveTimer) == 0) return;

    for (i = 0; i < this->mNumCells; i++) {
        v = &this->mCells[i];
        v->z = this->RippleHeight(v->dist);
    }

    this->BuildNormals();
    this->FlattenFrame();

    this->mWavePhase = this->mWavePhase + this->mWaveParams->phaseStep;
}

// @symbol _ZN11daPicGate_c19func_ov080_02126a54Ev
/* State 0 init. Same grid as state 1, plus a texcoord, and no wave yet. */
void daPicGate_c::func_ov080_02126a54()
{
    int x;
    int y;
    int row;
    int n;
    int col;
    x = 0;
    y = 0;
    row = 0;
    if ((int)this->mRows > 0) {
        do {
            col = 0;
            if ((int)this->mCols > 0) {
                do {
                    daPicGate_c::Vertex *v = &this->mCells[row * (int)this->mRows + col];
                    v->x = x;
                    v->y = y;
                    v->z = 0;
                    v->color = kFlatNormal;
                    int du = kTexSpan / ((int)this->mCols - 1);
                    int dv = kTexSpan / ((int)this->mRows - 1);
                    int s = du * col;
                    int t = kTexSpan - dv * row;
                    v->texCoord = (u16)(s16)(s >> 8) | (((u16)(s16)(t >> 8) << 1) << 15);
                    n = (int)this->mCols;
                    if (col == n - 2)
                        x = (u8)(this->param1 & 0xf) * kCell + kCell;
                    else
                        x += ((u8)(this->param1 & 0xf) * kCell + kCell) / (n - 1);
                    col++;
                } while (col < n);
            }
            n = (int)this->mRows;
            x = 0;
            if (row == n - 2)
                y = (u8)((this->param1 >> 4) & 0xf) * kCell + kCell;
            else
                y += ((u8)((this->param1 >> 4) & 0xf) * kCell + kCell) / (n - 1);
            row++;
        } while (row < n);
    }
    this->PlaceCorners();
}

// @symbol _ZN11daPicGate_c16CleanupResourcesEv
s32 daPicGate_c::CleanupResources() {
    func_0203cbc0(mCells);
    return 1;
}

// @symbol _ZN11daPicGate_c16OnPendingDestroyEv
void daPicGate_c::OnPendingDestroy() {
}

// @symbol _ZN11daPicGate_c6RenderEv
s32 daPicGate_c::Render() {
    (this->*mState->render)();
    return 1;
}

// @symbol _ZN11daPicGate_c8BehaviorEv
s32 daPicGate_c::Behavior() {
    (this->*mState->behavior)();
    return 1;
}

// @symbol _ZN11daPicGate_c13InitResourcesEv
s32 daPicGate_c::InitResources() {
    unsigned int state = (unsigned char)((param1 >> 0xd) & 3);

    if (state >= 2) {
        mRows = 2;
        mCols = mRows;
        mNumCells = (u16)(mCols * mRows);
    } else {
        unsigned int size = (unsigned char)(param1 & 0xf) + 1;
        switch (size) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            mRows = kGridRows[0];
            mCols = mRows;
            break;
        case 4:
            mRows = kGridRows[1];
            mCols = mRows;
            break;
        case 5:
            mRows = kGridRows[2];
            mCols = mRows;
            break;
        case 6:
            mRows = kGridRows[3];
            mCols = mRows;
            break;
        case 7:
            mRows = kGridRows[4];
            mCols = mRows;
            break;
        case 8:
            mRows = kGridRows[5];
            mCols = mRows;
            break;
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            break;
        case 16:
            mRows = kGridRows[6];
            mCols = mRows;
            break;
        }
        mNumCells = (u16)(mCols * mRows);
    }

    mCells = (Vertex *)Memory::operator_new2((unsigned)mNumCells * 0x18u);
    mTexRecord = (s32)this->LoadTexture((int)((unsigned char)((param1 >> 8) & 0x1f)));
    mWaveParams = (const daPicGate_c::WaveParams *)&kDefaultWave;
    mState = &kStates[(unsigned char)((param1 >> 0xd) & 3)];
    (this->*mState->init)();

    {
        int width = (int)((unsigned char)(param1 & 0xf) + 1) * kCell;
        int height = (int)((unsigned char)((param1 >> 4) & 0xf) + 1) * kCell;
        SetRanges(this, height / 2, width / 2 + kRangePad, kFar, kFar);
    }
    if ((unsigned char)((param1 >> 8) & 0x1f) == kPicUnclipped) {
        if ((unsigned char)((param1 >> 0xd) & 3) == 1)
            mFlags &= ~kClipEnable;
    }
    if ((unsigned char)((param1 >> 8) & 0x1f) == kPicSlide) {
        if ((unsigned char)((param1 >> 0xd) & 3) == 1) {
            mClosedPosX = mPosX;
            if (kSave[2] & kOpened)
                mPosX += kSlide;
        }
    }
    this->BuildGateMatrix();
    return 1;
}

/* daPicGate_c_classInit is not `new daPicGate_c()`. ~daPicGate_c() is the
 * key function and is defined outside this file, so the new-expression's
 * vptr store reaches the undefined _ZTV11daPicGate_c with addend 8 (the
 * start-of-object spelling); the cartridge stores the address-point label
 * itself, and production isolation refuses an undefined vtable reference
 * with a nonzero addend (measured: the rombuild link control fails on it).
 * The factory keeps the hand-built sequence: operator new(0x1bc), the
 * dActor_c constructor, the vtable store. */
// @symbol daPicGate_c_classInit
extern "C" daPicGate_c *daPicGate_c_classInit()
{
    int *p = (int *)_ZN7fBase_cnwEj(sizeof(daPicGate_c));
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV11daPicGate_c;
    }
    return (daPicGate_c *)p;
}

/* Source order is construction order: the nineteen picture file handles
 * first (data_ov080_0212775c indexes them by picture id), then the four
 * state rows. __sinit_daPicGate_c.cpp emits the constructions, the
 * destructor registrations and the record copies; the registration nodes
 * are compiler temporaries. */
PicGateFilePtr data_ov080_0212851c(0x4ab);
PicGateFilePtr data_ov080_02128524(0x4ac);
PicGateFilePtr data_ov080_021284ac(0x4b6);
PicGateFilePtr data_ov080_021284fc(0x4b8);
PicGateFilePtr data_ov080_0212850c(0x4ae);
PicGateFilePtr data_ov080_021284cc(0x4b3);
PicGateFilePtr data_ov080_021284b4(0x4af);
PicGateFilePtr data_ov080_02128514(0x4bd);
PicGateFilePtr data_ov080_021284f4(0x4b7);
PicGateFilePtr data_ov080_0212852c(0x4bc);
PicGateFilePtr data_ov080_021284dc(0x4b4);
PicGateFilePtr data_ov080_021284e4(0x4b5);
PicGateFilePtr data_ov080_021284ec(0x4b9);
PicGateFilePtr data_ov080_021284bc(0x4ad);
PicGateFilePtr data_ov080_02128504(0x4b1);
PicGateFilePtr data_ov080_0212853c(0x4b0);
PicGateFilePtr data_ov080_021284d4(0x4b2);
PicGateFilePtr data_ov080_02128534(0x4ba);
PicGateFilePtr data_ov080_021284c4(0x4bb);

daPicGate_c::State data_ov080_02128628[4] = {
    { &daPicGate_c::func_ov080_02126a54, &daPicGate_c::func_ov080_021269b8,
      &daPicGate_c::func_ov080_0212677c },
    { &daPicGate_c::func_ov080_021265ec, &daPicGate_c::func_ov080_021264ec,
      &daPicGate_c::func_ov080_021261f4 },
    { &daPicGate_c::func_ov080_02126124, &daPicGate_c::func_ov080_02126120,
      &daPicGate_c::func_ov080_02125fd0 },
    { &daPicGate_c::func_ov080_02125f00, &daPicGate_c::func_ov080_02126120,
      &daPicGate_c::func_ov080_02125fd0 },
};
