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
 * 0x12 is in, and latches bit 0x40000 of data_0209caa0.file.flags2. The same shift
 * is what daChScene_c applies to an exit whose high param byte is 0x12.
 * Picture 4 in state 1 clears the two clip-enable flags. States 2 and 3
 * are the flat 2x2 quad; their other handlers are outside this run.
 *
 * The grid is square (mCols is copied from mRows) and indexed with mRows
 * as the stride. Vertex::normal is the packed GX normal (10 bits per axis): the
 * strip writer stores it to REG_NORMAL.
 *
 * The five handlers are daPicGate_c members bound through the four
 * data_ov080_02128628 state rows. Row 0 is N64's proximity-triggered image
 * painting (wall_painting_proximity_rippling, painting_ripple_image), row 1
 * its continuous, environment-mapped one (wall_painting_continuous_rippling,
 * painting_ripple_env_mapped); picture 7's slide is N64's move_ddd_painting.
 * #pragma defer_codegen off keeps .text in
 * source order. The destructor pair and g_profile_PICTURE_GATE are outside
 * this run. The abutting registry factory daPicGate_c_classInit
 * (0x02126f8c) is written last.
 * SetRanges is a real dActor_c method now (dActor_c.h declares it).
 *
 * deslop leftovers:
 * - UpdateContinuousRipple and UpdateProximityRipple: mWavePhase += phaseStep
 *   differs by 6 words. The add that matches reuses the r5+0x100 base
 *   already formed for mNumCells; += addresses mWavePhase at 0x1b4.
 * - InitResources: a picture-id local, and declaring the default wave
 *   row as a WaveParams, shrank the function from 0x2ec to 0x2b4. The
 *   bit extracts stay written out, and the row stays an untyped object
 *   whose address is cast.
 * - DrawRippleImage: for-loops over the columns and the rows grew
 *   it from 0x23c to 0x24c. The entry test is still n = mCols; i = 0;
 *   n = n - 1, so the subtract consumes that n, and both loops stay
 *   do/while.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daPicGate_c_AssetId {
    ASSET_FOR_BH_BMD = 0x04ab,
    ASSET_FOR_BK_BMD = 0x04ac,
    ASSET_FOR_CT_BMD = 0x04ad,
    ASSET_FOR_CV_EX5_BMD = 0x04ae,
    ASSET_FOR_DL_BMD = 0x04af,
    ASSET_FOR_EX_LUIGI_BMD = 0x04b0,
    ASSET_FOR_EX_MARIO_BMD = 0x04b1,
    ASSET_FOR_EX_WARIO_BMD = 0x04b2,
    ASSET_FOR_FL_BMD = 0x04b3,
    ASSET_FOR_HM_BMD = 0x04b4,
    ASSET_FOR_HS_BMD = 0x04b5,
    ASSET_FOR_KI_BMD = 0x04b6,
    ASSET_FOR_SL_BMD = 0x04b7,
    ASSET_FOR_SM_BMD = 0x04b8,
    ASSET_FOR_TD_TT_BMD = 0x04b9,
    ASSET_FOR_VS_CROSS_BMD = 0x04ba,
    ASSET_FOR_VS_ISLAND_BMD = 0x04bb,
    ASSET_FOR_WC_BMD = 0x04bc,
    ASSET_FOR_WL_BMD = 0x04bd
};

#pragma defer_codegen off

#include "daPicGate_c.h"
#include "SharedFilePtr.h"
#include "common.h"
#include "math/Approach.h"
#include "math/MtxStack.h"
#include "SaveBlock.h"
#include "nitro/gx/g3imm_cmd.h"
#include "nitro/gx/g3imm_light.h"
#include "math/CalcLib.h"

enum {
    kCell = 0x64000,         /* 100.0, one width or height step */
    kSlide = 0x802000,       /* 2050.0 */
    kSlideStep = 0x13e72,
    kOpened = 0x40000,
    kFlatNormal = GX_VECFX10(0, 0, 0xfff),
    kTexSpan = 0x80000,
    kRangePad = 0xc8000,     /* 200.0, added to the clip radius */
    kFar = 0x1964000,        /* 6500.0, clip distance and far distance */
    kPicSlide = 7,
    kPicUnclipped = 4,
    kGateLevel = 0x12,
    kGateStar = 1,
    kClipEnable = 3
};

int IsStarCollectedInLevel(signed char levelID, int starID);
extern Matrix4x3 data_0209b3ec;
extern u8 data_ov080_02127714[];
extern "C" void G3_LoadMtx43(void *mtx);
extern void *data_ov080_02127834;

/* The cartridge's vtable label is the address point (slot 0), and the
   vtable itself is emitted with the destructor, outside this file. */
/* The helpers are daPicGate_c members declared on the class and defined in
 * src/game/actors/d_a_pic_gate.cpp. */
#define LoadMtx43 G3_LoadMtx43
#define kStates data_ov080_02128628
#define kDefaultWave data_ov080_02127834

extern daPicGate_c::State data_ov080_02128628[];

/* A picture's texture/collision handle. The nineteen instances below are
   constructed by __sinit_d_a_pic_gate_methods.cpp and reached through the rodata
   pointer table data_ov080_0212775c, indexed by picture id. The declared
   ctor/dtor stand for the cartridge's func_020178cc/func_020178b4 pair;
   words[] is the two words the ROM object carries. */
struct PicGateFilePtr : SharedFilePtr {
    PicGateFilePtr(u32 fileID);
    ~PicGateFilePtr();
};

/* Rows for widths 3, 4, 5, 6, 7, 8 and 16: 13, 14, 16, 20, 20, 25, 30. */
#define kGridRows data_ov080_02127714

/* State 1 behavior. Picture 7 slides open; every picture then ripples. */
void daPicGate_c::UpdateContinuousRipple() {
    if ((u8)((this->param1 >> 8) & 0x1f) == kPicSlide && !(data_0209caa0.file.flags2 & kOpened) &&
        IsStarCollectedInLevel(kGateLevel, kGateStar)) {
        if (ApproachLinear(this->mPosX, this->mClosedPosX + kSlide, kSlideStep))
            data_0209caa0.file.flags2 |= kOpened;
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

/* State 1 init. Even grid, flat +Z normal, then a wave at the centre. */
void daPicGate_c::InitContinuousRipple() {
    int x = 0;
    int y = 0;
    int row;
    int col;
    for (row = 0; row < this->mRows; row++) {
        for (col = 0; col < this->mCols; col++) {
            daPicGate_c::Vertex *v = &this->mCells[row * this->mRows + col];
            v->x = x;
            v->y = y;
            v->z = 0;
            v->normal = kFlatNormal;
            if (col == this->mCols - 2)
                x = ((u8)(this->param1 & 0xf) + 1) * kCell;
            else
                x += ((u8)(this->param1 & 0xf) + 1) * kCell / (this->mCols - 1);
        }
        x = 0;
        if (row == this->mRows - 2)
            y = ((u8)((this->param1 >> 4) & 0xf) + 1) * kCell;
        else
            y += ((u8)((this->param1 >> 4) & 0xf) + 1) * kCell / (this->mRows - 1);
    }
    this->PlaceCorners();
    {
        int width = ((u8)(this->param1 & 0xf) + 1) * kCell;
        int height = ((u8)((this->param1 >> 4) & 0xf) + 1) * kCell;
        this->BeginWave(width / 2, height / 2, 0);
    }
}

/* State 0 render. A dead wave draws the flat quad; otherwise a strip per column. */
void daPicGate_c::DrawRippleImage() {
    Matrix4x3 tmp;
    int i, j;

    if (this->mWaveTimer == 0) {
        this->DrawFlat();
        return;
    }

    G3_PushMtx();
    MTX_Concat43(&this->mMtx, &data_0209b3ec, &tmp);
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    LoadMtx43(&tmp);
    G3_MtxMode(GX_MTXMODE_POSITION);
    LoadMtx43(&tmp);

    G3_LightVector(GX_LIGHTID_3, 0, 0, -0x1000);
    G3_LightColor(GX_LIGHTID_3, GX_RGB(31, 31, 31));
    this->LoadMaterial();

    G3_Scale(0x20000, 0x20000, 0x20000);

    for (i = 0; i < this->mCols - 1; i++) {
        G3_Begin(GX_BEGIN_TRIANGLE_STRIP);
        for (j = 0; j < this->mRows; j++) {
            daPicGate_c::Vertex *v1 = &this->mCells[i + j * this->mRows];
            daPicGate_c::Vertex *v2 = &this->mCells[(i + 1) + j * this->mRows];

            REG_TEXCOORD = v1->texCoord;
            REG_NORMAL = v1->normal;
            G3_Vtx(v1->x >> 8, v1->y >> 8, v1->z >> 8);
            REG_TEXCOORD = v2->texCoord;
            REG_NORMAL = v2->normal;
            G3_Vtx(v2->x >> 8, v2->y >> 8, v2->z >> 8);
        }
        G3_End();
    }
    G3_PopMtx(1);
}

/* State 0 behavior. Ripples only while mWaveTimer is still counting. */
void daPicGate_c::UpdateProximityRipple() {
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

/* State 0 init. Same grid as state 1, plus a texcoord, and no wave yet. */
void daPicGate_c::InitProximityRipple() {
    int x = 0;
    int y = 0;
    int row;
    int col;
    for (row = 0; row < this->mRows; row++) {
        for (col = 0; col < this->mCols; col++) {
            daPicGate_c::Vertex *v = &this->mCells[row * this->mRows + col];
            v->x = x;
            v->y = y;
            v->z = 0;
            v->normal = kFlatNormal;
            int du = kTexSpan / (this->mCols - 1);
            int dv = kTexSpan / (this->mRows - 1);
            int s = du * col;
            int t = kTexSpan - dv * row;
            v->texCoord = GX_ST(s, t);
            if (col == this->mCols - 2)
                x = (u8)(this->param1 & 0xf) * kCell + kCell;
            else
                x += ((u8)(this->param1 & 0xf) * kCell + kCell) / (this->mCols - 1);
        }
        x = 0;
        if (row == this->mRows - 2)
            y = (u8)((this->param1 >> 4) & 0xf) * kCell + kCell;
        else
            y += ((u8)((this->param1 >> 4) & 0xf) * kCell + kCell) / (this->mRows - 1);
    }
    this->PlaceCorners();
}

s32 daPicGate_c::CleanupResources() {
    ::operator delete[](mCells);
    return 1;
}

void daPicGate_c::OnPendingDestroy() {
}

s32 daPicGate_c::Render() {
    (this->*mState->render)();
    return 1;
}

s32 daPicGate_c::Behavior() {
    (this->*mState->behavior)();
    return 1;
}

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

    mCells = (Vertex *)::operator new[]((unsigned)mNumCells * 0x18u);
    mTexRecord = (s32)this->LoadTexture((int)((unsigned char)((param1 >> 8) & 0x1f)));
    mWaveParams = (const daPicGate_c::WaveParams *)&kDefaultWave;
    mState = &kStates[(unsigned char)((param1 >> 0xd) & 3)];
    (this->*mState->init)();

    {
        int width = (int)((unsigned char)(param1 & 0xf) + 1) * kCell;
        int height = (int)((unsigned char)((param1 >> 4) & 0xf) + 1) * kCell;
        SetRanges(height / 2, width / 2 + kRangePad, kFar, kFar);
    }
    if ((unsigned char)((param1 >> 8) & 0x1f) == kPicUnclipped) {
        if ((unsigned char)((param1 >> 0xd) & 3) == 1)
            mFlags &= ~kClipEnable;
    }
    if ((unsigned char)((param1 >> 8) & 0x1f) == kPicSlide) {
        if ((unsigned char)((param1 >> 0xd) & 3) == 1) {
            mClosedPosX = mPosX;
            if (data_0209caa0.file.flags2 & kOpened)
                mPosX += kSlide;
        }
    }
    this->BuildGateMatrix();
    return 1;
}

daPicGate_c *daPicGate_c_classInit() {
    return new daPicGate_c;
}

/* Source order is construction order: the nineteen picture file handles
 * first (data_ov080_0212775c indexes them by picture id), then the four
 * state rows. __sinit_d_a_pic_gate_methods.cpp emits the constructions, the
 * destructor registrations and the record copies; the registration nodes
 * are compiler temporaries. */
PicGateFilePtr gForBhModelFile(ASSET_FOR_BH_BMD);
PicGateFilePtr gForBkModelFile(ASSET_FOR_BK_BMD);
PicGateFilePtr gForKiModelFile(ASSET_FOR_KI_BMD);
PicGateFilePtr gForSmModelFile(ASSET_FOR_SM_BMD);
PicGateFilePtr gForCvEx5ModelFile(ASSET_FOR_CV_EX5_BMD);
PicGateFilePtr gForFlModelFile(ASSET_FOR_FL_BMD);
PicGateFilePtr gForDlModelFile(ASSET_FOR_DL_BMD);
PicGateFilePtr gForWlModelFile(ASSET_FOR_WL_BMD);
PicGateFilePtr gForSlModelFile(ASSET_FOR_SL_BMD);
PicGateFilePtr gForWcModelFile(ASSET_FOR_WC_BMD);
PicGateFilePtr gForHmModelFile(ASSET_FOR_HM_BMD);
PicGateFilePtr gForHsModelFile(ASSET_FOR_HS_BMD);
PicGateFilePtr gForTdTtModelFile(ASSET_FOR_TD_TT_BMD);
PicGateFilePtr gForCtModelFile(ASSET_FOR_CT_BMD);
PicGateFilePtr gForExMarioModelFile(ASSET_FOR_EX_MARIO_BMD);
PicGateFilePtr gForExLuigiModelFile(ASSET_FOR_EX_LUIGI_BMD);
PicGateFilePtr gForExWarioModelFile(ASSET_FOR_EX_WARIO_BMD);
PicGateFilePtr gForVsCrossModelFile(ASSET_FOR_VS_CROSS_BMD);
PicGateFilePtr gForVsIslandModelFile(ASSET_FOR_VS_ISLAND_BMD);

daPicGate_c::State data_ov080_02128628[4] = {
    {&daPicGate_c::InitProximityRipple, &daPicGate_c::UpdateProximityRipple,
     &daPicGate_c::DrawRippleImage},
    {&daPicGate_c::InitContinuousRipple, &daPicGate_c::UpdateContinuousRipple,
     &daPicGate_c::DrawRippleEnvMapped},
    {&daPicGate_c::PlaceCorners, &daPicGate_c::UpdateFlat, &daPicGate_c::DrawFlat},
    {&daPicGate_c::PlaceCornersMirrored, &daPicGate_c::UpdateFlat, &daPicGate_c::DrawFlat},
};
