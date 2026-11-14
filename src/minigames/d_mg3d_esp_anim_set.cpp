//cpp
/* The MG_3DESP minigame scene (scene 0x185): dScMg3DEsp_c and
 * the two helper classes it embeds, dMg3DEspModel_c and dMg3DEspAnimSet_c.
 *
 * This TU is the minigame's whole linker unit: 68 functions, .text
 * 0x020e7660..0x020ea280. It opens with dScMg3DEsp_c's destructor, which
 * the header declares first and out of line, so this file owns the key
 * function and emits the vtable and the RTTI chain. Then come
 * dMg3DEspAnimSet_c (three animated models), dMg3DEspModel_c (an animated
 * model that dispatches through a member-function state), the scene's
 * slot, row and card state handlers and its round phases, and it closes
 * with dScMg3DEsp_c's own members, OnNewRecord through InitResources.
 * dScMg3DEsp_c_classInit, the scene factory, follows at 0x020ea1f0..0x020ea280
 * and ends the unit. Functions run in ROM order under
 * `#pragma defer_codegen off`; do not reorder.
 *
 * deslop leftovers:
 * - The scene's cards, rows and slots are the header's Card, Row and Slot
 *   arrays, indexed by element.
 * - Ent_8e10 stands in for a card sprite's part record; RenderCards walks
 *   the list until f6 is 0xffff.
 * - dMg3DEspModel_c's unk_000..unk_008 are set by its constructor
 *   (0x8c000, 0x8c000, 0) and read nowhere in this unit, so they stay
 *   unnamed.
 */

#pragma defer_codegen off

#include "common.h"
#include "nitro/hw/registers.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/gx_load.h"
#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "nitro/gx/g2_oam.h"
#include "SharedFilePtr.h"
#include "dScMg3DEsp_c.h"
#include "dMg3DEspModel_c.h"
#include "dMg3DEspAnimSet_c.h"
#include "dExtAnmTexSRT_c.h"
#include "dExtAnmTexPat_c.h"
#include "dExtAnmMaterial_c.h"
#include "nitro/gx/g3x.h"
#include "Sound.h"
#include "PlayerInput.h"
#include "dExtSimpleModel_c.h"
#include "cstd.h"
#include "math/Approach.h"
#include "LevelRng.h"
#include "math/CalcLib.h"
extern "C" void Mg_DrawScoreTally(void *, int, int, int);
extern "C" void Mg_ShowBanner(int, int, int, int, int, short);
extern "C" void Mg_SetSharedState(s32);
extern "C" void FreeGfxSlotsById(int);
extern "C" void Mg_SetTopScreenYOffset(int);
extern "C" void Mg_RenderSubSpriteMtx(void *a, int, int, void *m);
extern "C" int Mg_RenderSubSpriteScaled(void *, int, int, int, int, int, unsigned short);
extern "C" void Mg_SubmitScore(int);
void Mg_SetNextBanner(int);

struct BMD_File;
struct BTP_File;
struct BTA_File;
struct ModelComponents;

/* Retail constructs these 8-byte handles in this order and registers
 * their destructors. The wrapper names are local; the ctor/dtor
 * addresses are the ROM resource-family functions. The wrappers are
 * declared, never defined: the manifest aliases their undefined members
 * onto the ROM veneers. */
struct EspAnimSetModelFilePtr : SharedFilePtr {
    EspAnimSetModelFilePtr(u32 fileID);
    ~EspAnimSetModelFilePtr();
};

struct EspAnimSetAnimationFileHandle : SharedFilePtr {
    EspAnimSetAnimationFileHandle(u32 fileID);
    ~EspAnimSetAnimationFileHandle();
};

struct EspAnimSetTexSequenceFilePtr : SharedFilePtr {
    EspAnimSetTexSequenceFilePtr(u32 fileID);
    ~EspAnimSetTexSequenceFilePtr();
};

/* The file handles: word 1 of a SharedFilePtr is its loaded file. */
extern EspAnimSetModelFilePtr gEspCardModelFile;

extern EspAnimSetAnimationFileHandle gEspCardStartAnimFile;

extern EspAnimSetTexSequenceFilePtr gEspCardTexSeqFile;

extern EspAnimSetAnimationFileHandle gEspHamonAnimFile;

extern EspAnimSetModelFilePtr gIrieBackModelFile;

extern EspAnimSetAnimationFileHandle gEspCardWaitAnimFile;

extern EspAnimSetAnimationFileHandle gEspCardOpenWaitAnimFile;

extern EspAnimSetAnimationFileHandle gEspCardOpenAnimFile;

extern EspAnimSetModelFilePtr gEspHamonModelFile;

extern EspAnimSetModelFilePtr gEspIrieModelFile;

extern Matrix4x3 data_020a0e68;
extern "C" {
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
}

void func_02046208(void *a, unsigned int b, int c);

int func_020179b4(SharedFilePtr *r0, dExtModel_c *r1, int r2);
extern "C" {
void Mg_SetNextBanner(int);
}

extern "C" void InitialiseVramGlobals(void);

extern "C" void Deallocate(void *);
extern "C" {
void *LoadFile(int handle);

void DecompressLZ16(void *src, void *dst);
}

extern "C" void GXS_LoadBG2Scr(const void *, u32, u32);

extern int sEsp3DMaterialAnim;

extern int pmf_dMg3DEspModel_c_Idle_2[];

extern dMg3DEspModel_c::State pmf_dMg3DEspModel_c_Idle;

extern dMg3DEspModel_c::State pmf_dMg3DEspModel_c_Hit;

extern dMg3DEspModel_c::State pmf_dMg3DEspModel_c_Intro;

extern dMg3DEspModel_c::State pmf_dMg3DEspModel_c_Wait;

extern dMg3DEspModel_c::State pmf_dMg3DEspModel_c_Idle_3;

extern void *sEsp3DTextureAnim;

extern unsigned char gMainScreenLayers;

extern unsigned char gSubScreenLayers;

extern int sMgClearBannerX;

extern int sMgClearBannerY;

extern Matrix4x3 data_ov006_0213c85c;

/* Both models' starting matrix. Declaring it non-volatile adds 32 bytes to
   dScMg3DEsp_c::InitResources, and copying it without the stack temporary
   removes 32. */
extern volatile Matrix4x3 sEsp3DIdentityMtx;

/* ---- dScMg3DEsp_c: the destructor (key function) ---------------------- */

/* The body is empty: the members' destructors (reverse declaration order),
   the vtable stores, mParticle's destruction and the chain to
   ~dScMgBase_c are all compiler-generated; the deleting variant reaches
   dScMgBase_c's operator delete. */
dScMg3DEsp_c::~dScMg3DEsp_c()
{
}

/* ---- dMg3DEspAnimSet_c ------------------------------------------------- */

void dMg3DEspAnimSet_c::Render()
{
    int i;
    Matrix4x3_FromTranslation(&data_020a0e68, 0x8c000, 0x80000, 0x40000);
    for (i = 0; i < 3; i++) {
        if (mActive[i] != 0) {
            mModels[i].mat4x3 = data_020a0e68;
            mMaterialChangers[i].Update(mModels[i].data);
            mModels[i].Render(0);
        }
    }
}

void dMg3DEspAnimSet_c::Behavior() {
    int start = 0;
    int i;
    if (mRepeats != 0) {
        if (ApproachLinear2(mPhase, 0, 1)) {
            mPhase = mPeriod;
            start = 1;
            if (mRepeats > 0) {
                mRepeats -= 1;
            }
        }
    }
    for (i = 0; i < 3; i++) {
        if (mActive[i] != 0) {
            mModels[i].Advance();
            mMaterialChangers[i].Advance();
            if (mModels[i].Finished()) {
                mActive[i] = 0;
            }
        } else if (start == 1) {
            mActive[i] = 1;
            mModels[i].currFrame = 0;
            start = 0;
            mMaterialChangers[i].currFrame = 0;
        }
    }
}

void dMg3DEspAnimSet_c::Reset()
{
    mRepeats = 0;
    mPeriod = 30;
    mPhase = 0;
}

void dMg3DEspAnimSet_c::SetRepeats(s16 v)
{
    mRepeats = v;
    mPhase = 0;
}

void dMg3DEspAnimSet_c::SetPeriod(s16 v)
{
    mPeriod = v;
}

void dMg3DEspAnimSet_c::SetSpeed(s32 v)
{
    mSpeed = v;
}

void dMg3DEspAnimSet_c::InitResources()
{
    BMD_File *bmd;
    BCA_File *bca;
    int i;
    mSpeed = 0x800;
    bmd = dExtSimpleModel_c::LoadFile(gEspHamonModelFile);
    bca = dExtFrameCtrl_c::LoadFile(gEspHamonAnimFile);
    dExtAnmMaterial_c::Prepare(*bmd, *(BMA_File *)&sEsp3DMaterialAnim);
    for (i = 0; i < 3; i++) {
        mModels[i].SetFile(bmd, 1, -1);
        mModels[i].SetAnim(bca, 0x40000000, mSpeed, 0);
        mMaterialChangers[i].SetFile(*(BMA_File *)&sEsp3DMaterialAnim, 0x40000000, mSpeed, 0);
        mModels[i].SetPolygonID((i + 1) & 0xff);
        mActive[i] = 0;
    }
    mRepeats = 0;
    mPhase = 0;
}

dMg3DEspAnimSet_c::~dMg3DEspAnimSet_c()
{
    gEspHamonModelFile.Release();
    gEspHamonAnimFile.Release();
}

dMg3DEspAnimSet_c::dMg3DEspAnimSet_c() {}

/* ---- dMg3DEspModel_c --------------------------------------------------- */

void dMg3DEspModel_c::ResetTransform()
{
    mAnmModel.mat4x3 = data_ov006_0213c85c;
}

void dMg3DEspModel_c::Render()
{
    int *p = (int *)&mState;
    int* d = pmf_dMg3DEspModel_c_Idle_2;
    if (p[0] == d[0]) {
        if (p[1] == d[1]) return;
        if (*(int *)&mState == 0)
            return;
    }

    func_02046208((char *)&mAnmModel.data, mPolygonID, 0);

    mTextureSequence.currFrame = (unsigned)((mTextureFrame) << 0x11) >> 4;
    mTextureSequence.Update(mAnmModel.data);

    ResetTransform();

    mAnmModel.Render(0);

    mAnimSet.Render();
}

void dMg3DEspModel_c::Behavior()
{
    /* mwcc's member-pointer null representation makes the second word
     * irrelevant when the function word is null. Spell that comparison out
     * to preserve the original early-return shape. */
    s32 *state = (s32 *)&mState;
    s32* idleState = (s32*)&pmf_dMg3DEspModel_c_Idle;
    if (state[0] == idleState[0]) {
        if (state[1] == idleState[1]) return;
        if (*(s32 *)&mState == 0)
            return;
    }

    (this->*mState)();

    mAnmModel.Advance();
    mAnimSet.Behavior();

    if (mShowSparks == 0) return;

    mParticleSys =
        Particle::System::New(mParticleSys, kPtclMg3DEspSparks, 0x48c000, 0x140000, 0x200000, 0, 0);
}

void dMg3DEspModel_c::Hit()
{
    if (mAnmModel.Finished() != 0 && mAnmModel.file == (BCA_File *)gEspCardOpenAnimFile.filePtr) {
        mAnmModel.SetAnim((BCA_File *)gEspCardOpenWaitAnimFile.filePtr, 0, 0x800, 0);
        return;
    }
    if (mAnmModel.WillHitFrame(0x10) == 0 && mAnmModel.WillHitFrame(0x50) == 0)
        return;
    if (mMuted != 0)
        return;
    Sound::PlayBank2_2D(0x18f);
}

void dMg3DEspModel_c::StartHit()
{
    mAnmModel.SetAnim((BCA_File *)gEspCardOpenAnimFile.filePtr, 0x40000000, 0x800, 0);
    if (mMuted == 0)
        Sound::PlayBank2_2D(0x191);
    mState = pmf_dMg3DEspModel_c_Hit;
}

void dMg3DEspModel_c::Intro()
{
    if (mAnmModel.Finished()) {
        if (mMuted == 0)
            Sound::PlayBank2_2D(0x18f);
        StartWait();
        return;
    }
    if (mAnmModel.WillHitFrame(0x1e) == 0)
        return;
    mAnimSet.SetPeriod(0x1e);
    mAnimSet.SetSpeed(0x800);
    mAnimSet.SetRepeats(-1);
    mShowSparks = 1;
}

void dMg3DEspModel_c::StartIntro()
{
    mAnmModel.SetAnim((BCA_File *)gEspCardStartAnimFile.filePtr, 0x40000000, 0x800, 0);
    mAnimSet.SetPeriod(0xf);
    mAnimSet.SetSpeed(0x1000);
    mAnimSet.SetRepeats(3);
    if (mMuted == 0)
        Sound::PlayBank2_2D(0x18e);
    mState = pmf_dMg3DEspModel_c_Intro;
}

void dMg3DEspModel_c::Wait()
{
    if (mMuted)
        return;
    if (!mAnmModel.WillHitFrame(0)) {
        if (!mAnmModel.WillHitFrame(0x39))
            return;
    }
    Sound::PlayBank2_2D(0x18f);
}

/* Keep the scalar-speed boundary used by the existing SetAnim definition.
   The measured native Fix12-by-value call grows this function by eight
   bytes. */
void dMg3DEspModel_c::StartWait()
{
    mAnmModel.SetAnim((BCA_File *)gEspCardWaitAnimFile.filePtr, 0, 0x800, 0);
    mState = pmf_dMg3DEspModel_c_Wait;
}

void dMg3DEspModel_c::Idle()
{
}

void dMg3DEspModel_c::Reset()
{
    mAnimSet.Reset();
    mShowSparks = 0;
    mState = pmf_dMg3DEspModel_c_Idle_3;
}

int dMg3DEspModel_c::InitResources()
{
    if (func_020179b4(&gEspCardModelFile, &mAnmModel, 1) == 0)
        return 0;
    dExtAnmTexPat_c::LoadFile(gEspCardTexSeqFile);
    dExtFrameCtrl_c::LoadFile(gEspCardStartAnimFile);
    dExtFrameCtrl_c::LoadFile(gEspCardWaitAnimFile);
    dExtFrameCtrl_c::LoadFile(gEspCardOpenAnimFile);
    dExtFrameCtrl_c::LoadFile(gEspCardOpenWaitAnimFile);
    dExtAnmTexPat_c::Prepare(**(BMD_File **)((void **)&gEspCardModelFile + 1),
                             **(BTP_File **)((void **)&gEspCardTexSeqFile + 1));
    mTextureSequence.SetFile(**(BTP_File **)((void **)&gEspCardTexSeqFile + 1), 0, 0x1000, 0);
    mAnmModel.SetAnim(*((BCA_File **)&gEspCardStartAnimFile + 1), 0x40000000, 0x800, 0);
    mAnimSet.InitResources();
    ResetTransform();
    Reset();
    mShowSparks = 0;
    return 1;
}

dMg3DEspModel_c::~dMg3DEspModel_c()
{
    gEspCardModelFile.Release();
    gEspCardStartAnimFile.Release();
    gEspCardWaitAnimFile.Release();
    gEspCardOpenAnimFile.Release();
    gEspCardOpenWaitAnimFile.Release();
    gEspCardTexSeqFile.Release();
}

dMg3DEspModel_c::dMg3DEspModel_c()
    : unk_000(0x8c000),
      unk_004(0x8c000),
      unk_008(0),
      mMuted(0),
      mTextureFrame(0),
      mPolygonID(0x1f)
{
}

/* ---- the round: helpers and states ------------------------------------- */

/* The scene keeps its state machines in pointer-to-member tables the static
   init copies into data_ov006_02141fxx: the three round states for
   Behavior, four play phases for Play, five card states, three row states,
   two slot states, four slot type handlers and three flash modes. */
typedef void (dScMg3DEsp_c::*dScMg3DEsp_cState)();
typedef void (dScMg3DEsp_c::*dScMg3DEsp_cObjState)(int);
extern dScMg3DEsp_cState s3DEspRoundTable[];
extern dScMg3DEsp_cState s3DEspPhaseTable[];
extern dScMg3DEsp_cState s3DEspFlashTable[];
extern dScMg3DEsp_cObjState s3DEspSlotFallTable[];
extern dScMg3DEsp_cObjState s3DEspCardTable[];
extern dScMg3DEsp_cObjState s3DEspRowTable[];
extern dScMg3DEsp_cObjState s3DEspSlotRiseTable[];
extern /* The ROM PMF constants the seven state tables copy from (unlicensed
 * .data), grouped by destination table in retail copy order. */
    dScMg3DEsp_cState pmf_dScMg3DEsp_c_Wait;
extern dScMg3DEsp_cState pmf_dScMg3DEsp_c_Play;
extern dScMg3DEsp_cState pmf_dScMg3DEsp_c_Results;
extern dScMg3DEsp_cState pmf_dScMg3DEsp_c_Deal;
extern dScMg3DEsp_cState pmf_dScMg3DEsp_c_Fade;
extern dScMg3DEsp_cState pmf_dScMg3DEsp_c_Await;
extern dScMg3DEsp_cState pmf_dScMg3DEsp_c_Resolve;
extern dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_CardExit;
extern dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_CardTouch;
extern dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_CardSeek;
extern dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_RowWait;
extern dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_RowDecay;
extern dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_RowFade;
extern dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_SlotWait;
extern dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_SlotFall;
extern dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_SlotSetup;
extern dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_SlotRise;
extern dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_SlotKick;
extern dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_SlotDampen;
extern dScMg3DEsp_cState pmf_dScMg3DEsp_c_FlashOnce;
extern dScMg3DEsp_cState pmf_dScMg3DEsp_c_FlashSync;
extern dScMg3DEsp_cState pmf_dScMg3DEsp_c_FlashQuick;

void dScMg3DEsp_c::DrawBanner()
{
    if (mDone != 0)
        Mg_DrawScoreTally(this, 0xe0, 0x14, 1);
}

/* Early-out once mFlashStep is set; otherwise set the embedded model's
   mPolygonID to 0x1e and step mFlashStep. */
void dScMg3DEsp_c::FlashQuick() {
    if (mFlashStep)
        return;
    mEspModel.mPolygonID = 0x1e;
    mFlashStep += 1;
}

void dScMg3DEsp_c::FlashSync() {
    if (mFlashStep == 0) {
        mEspModel.mPolygonID = 0x1e;
        mFlashStep++;
        return;
    }
    if (mFlashStep != 1)
        return;
    if (mEspModel.mAnmModel.WillHitFrame(0))
        mFlashBeats++;
    if (mFlashBeats < 1)
        return;
    mFlashStep++;
    mEspModel.mPolygonID = 0x1f;
}

void dScMg3DEsp_c::FlashOnce() {
    if (mFlashStep != 0)
        return;
    mFlashStep += 1;
    mEspModel.mPolygonID = 0x1f;
}

void dScMg3DEsp_c::UpdateFlash()
{
    unsigned char idx = mFlashMode;
    if (idx >= 3) return;
    (this->*s3DEspFlashTable[idx])();
}

extern void *sEsp3DAnimSetSlotsSprites[];

void dScMg3DEsp_c::RenderSlots() {
    int i;
    for (i = 0; i < 20; i++) {
        if (mSlots[i].mVisible) {
            int x = mSlots[i].mX >> 12;
            int y = mSlots[i].mY >> 12;
            void *sprite = sEsp3DAnimSetSlotsSprites[mSlots[i].mFrame];
            Mg_RenderSubSpriteMtx(sprite, x, y, 0);
        }
    }
}

void dScMg3DEsp_c::SlotDampen(int i) {
    mSlots[i].mX += mSlots[i].mVelX;
    mSlots[i].mY += mSlots[i].mVelY;
    if (mSlots[i].mVelX > 0) {
        mSlots[i].mVelX -= ((mSlots[i].mAccel * 16) + 0x10);
        if ((short)mSlots[i].mVelX < 0) {
            mSlots[i].mVelX = 0;
        }
    } else if (mSlots[i].mVelX < 0) {
        mSlots[i].mVelX += ((mSlots[i].mAccel * 16) + 0x10);
        if ((short)mSlots[i].mVelX > 0) {
            mSlots[i].mVelX = 0;
        }
    } else {
        mSlots[i].mType = 0;
    }
}

void dScMg3DEsp_c::SlotKick(int i)
{
    mSlots[i].mX += mSlots[i].mVelX;
    mSlots[i].mY += mSlots[i].mVelY;
    if (mSlots[i].mDelay != 0) {
        mSlots[i].mDelay -= 1;
        if ((s16)mSlots[i].mDelay < 0)
            mSlots[i].mDelay = 0;
        return;
    }
    mSlots[i].mVelX -= (mSlots[i].mAccel << 4) + 0x10;
    if (mSlots[i].mPushTime != 0) {
        mSlots[i].mPushTime -= 1;
        if ((s16)mSlots[i].mPushTime < 0)
            mSlots[i].mPushTime = 0;
        return;
    }
    mSlots[i].mDelay = (u8)(MG_RAND(16) + 0x10);
    mSlots[i].mType = 3;
}

void dScMg3DEsp_c::SlotRise(int i)
{
    mSlots[i].mX += mSlots[i].mVelX;
    mSlots[i].mY += mSlots[i].mVelY;

    if (mSlots[i].mDelay != 0) {
        mSlots[i].mDelay -= 1;
        {
            short s = mSlots[i].mDelay;
            if (s < 0)
            {
                s = 0;
                mSlots[i].mDelay = s;
            }
        }
        return;
    }

    {
        unsigned char b = mSlots[i].mAccel;
        mSlots[i].mVelX += (b << 4) + 0x10;
    }

    if (mSlots[i].mPushTime != 0) {
        mSlots[i].mPushTime -= 1;
        {
            short s = mSlots[i].mPushTime;
            if (s < 0)
            {
                s = 0;
                mSlots[i].mPushTime = s;
            }
        }
        return;
    }

    mSlots[i].mDelay = (u8)(MG_RAND(16) + 0x10);
    mSlots[i].mType = 3;
}

extern unsigned char data_ov006_0212e57c[];

void dScMg3DEsp_c::SlotSetup(int idx) {
    mSlots[idx].mVelX = 0;
    mSlots[idx].mAccel = MG_RAND(8);
    mSlots[idx].mGravity = MG_RAND(8);
    mSlots[idx].mVisible = 1;
    mSlots[idx].mDelay = (u8)(MG_RAND(16) + 8);
    mSlots[idx].mPushTime = (u8)(MG_RAND(32) + 0x10);
    mSlots[idx].mType = data_ov006_0212e57c[MG_RAND(2)];
}

void dScMg3DEsp_c::SlotFall(int idx)
{
    unsigned char b = mSlots[idx].mType;
    (this->*s3DEspSlotRiseTable[b])(idx);

    unsigned char c = mSlots[idx].mGravity;
    mSlots[idx].mVelY -= (c << 3) + 0x10;
    int sh = mSlots[idx].mLife >> 3;
    if (sh >= 7)
        sh = 7;
    mSlots[idx].mFrame = 7 - sh;
    if (mSlots[idx].mLife != 0) {
        mSlots[idx].mLife -= 1;
        if ((s16)mSlots[idx].mLife < 0)
            mSlots[idx].mLife = 0;
    } else {
        mSlots[idx].mState = 0;
    }
}

void dScMg3DEsp_c::SlotWait(int idx)
{
    if (mSlots[idx].mWait != 0) {
        mSlots[idx].mWait -= 1;
        if ((s16)mSlots[idx].mWait < 0)
            mSlots[idx].mWait = 0;
    } else {
        mSlots[idx].mLife = MG_RAND(32) + 0x40;
        mSlots[idx].mState = 1;
        mSlots[idx].mType = 0;
        mSlots[idx].mVisible = 0;
        mSlots[idx].mX = MG_RAND(32) << 15;
        mSlots[idx].mY = ((MG_RAND(16) << 3) + 0x40) << 12;
        mSlots[idx].mVelY = -0x200;
        mSlots[idx].mFrame = 0;
    }
}

void dScMg3DEsp_c::UpdateSlots() {
    for (int i = 0; i < 0x14; i++) {
        if (mSlots[i].mActive) {
            u8 k = mSlots[i].mState;
            (this->*s3DEspSlotFallTable[k])(i);
        }
    }
}

void dScMg3DEsp_c::ResetSlots() {
    int i;
    for (i = 0; i < 0x14; i++) {
        mSlots[i].mActive = 0;
        mSlots[i].mVisible = 0;
    }

    for (i = 0; i < 0x14; i++) {
        mSlots[i].mActive = 1;
        mSlots[i].mState = 0;
        mSlots[i].mType = 0;
        mSlots[i].mLife = 0;
        mSlots[i].mWait = ((i & 0xf) << 3) << 2;
    }
}

extern int sEsp3DAnimSetRowsSprites;

void dScMg3DEsp_c::RenderRows()
{
    int i;
    for (i = 0; i < 3; i++) {
        if (mRows[i].mVisible != 0) {
            int x = mRows[i].mX >> 12;
            int y = mRows[i].mY >> 12;
            int priority = mRows[i].mPriority;
            int scale = mRows[i].mScale;
            GXOamAttr *oam = (GXOamAttr *)Mg_RenderSubSpriteScaled((void *)sEsp3DAnimSetRowsSprites,
                                                                   x, y, -1, priority, scale, 0);
            if (mRows[i].mState == 2) {
                G2_SetOBJMode(oam, GX_OAM_MODE_XLU, oam->cParam);
            }
        }
    }
}

void dScMg3DEsp_c::RowFade(int i)
{
    int x = mRows[i].mScale;
    x = x - (x >> 7);
    mRows[i].mScale = x;
    mFade += 1;
    {
        unsigned char ip = mFade;
        G2S_SetBlendAlpha(0, 4, 0x10 - ip, ip);
    }
    if (mFade < 0x10)
        return;
    mFade = 0;
    mRows[i].mVisible = 0;
    mRows[i].mActive = 0;
}

void dScMg3DEsp_c::RowDecay(int i)
{
    int x = mRows[i].mScale;
    x = x - (x >> 6);
    mRows[i].mScale = x;
    if (mRows[i].mScale <= 0x900) {
        mRows[i].mState = 2;
    }
}

void dScMg3DEsp_c::RowWait(int i)
{
    if (mRows[i].mTimer != 0) {
        mRows[i].mTimer -= 1;
        if ((s16)mRows[i].mTimer < 0)
            mRows[i].mTimer = 0;
        return;
    }

    mRows[i].mVisible = 1;
    mRows[i].mState = 1;
}

void dScMg3DEsp_c::UpdateRows()
{
    int i;
    for (i = 0; i < 3; i++) {
        if (mRows[i].mActive) {
            (this->*s3DEspRowTable[mRows[i].mState])(i);
        }
    }
}

extern unsigned short data_ov006_0212e58c[];

void dScMg3DEsp_c::SpawnRows(int idx) {
    int i;
    for (i = 0; i < 3; i++) {
        mRows[i].mActive = 1;
        mRows[i].mX = mCards[idx].mX;
        mRows[i].mY = mCards[idx].mY;
        mRows[i].mVisible = 0;
        mRows[i].mState = 0;
        mRows[i].mTimer = data_ov006_0212e58c[i];
        mRows[i].mPriority = 0;
        mRows[i].mScale = 0x1400;
    }
}

struct Ent_8e10 { u16 f0, f2, f4, f6; };
extern Ent_8e10 *data_ov006_02133f24[];

void dScMg3DEsp_c::RenderCards() {
    int i;
    for (i = 0; i < 5; i++) {
        if (mCards[i].mVisible != 0) {
            int x = mCards[i].mX >> 12;
            int y = mCards[i].mY >> 12;
            int priority = mCards[i].mPriority;
            Ent_8e10 *r7 = data_ov006_02133f24[mCards[i].mFace];
            for (;;) {
                GXOamAttr *oam =
                    (GXOamAttr *)Mg_RenderSubSpriteScaled(r7, x, y, -1, priority, 0x1000, 0);
                if (mFade && !mCards[i].mState) {
                    G2_SetOBJMode(oam, GX_OAM_MODE_XLU, oam->cParam);
                }
                if (r7->f6 == 0xffff)
                    break;
                r7++;
            }
        }
    }
}

void dScMg3DEsp_c::CardSeek(int i) {
    u16 timer;
    s16 cosine;
    s16 sine;
    int speed2;
    int speed1;

    timer = mCards[i].mTimer;
    if (timer != 0) {
        mCards[i].mTimer = timer - 1;
        if ((s16)mCards[i].mTimer <= 0) {
            mCards[i].mTimer = 0;
        }
        return;
    }

    speed1 = mCards[i].mSpeed;
    cosine = FX_CosIdx(mCards[i].mAngle >> 4);
    mCards[i].mX += FX_MUL(cosine, speed1);

    sine = FX_SinIdx(mCards[i].mAngle >> 4);
    speed2 = mCards[i].mSpeed;
    mCards[i].mY += FX_MUL(sine, speed2);
    mCards[i].mSpeed += 0x300;

    {
        int dx = 0x80 - (mCards[i].mX >> 0xc);
        int dz = 0x30 - (mCards[i].mY >> 0xc);
        if (dx < -6)
            return;
        if (dx > 6)
            return;
        if (dz < -6)
            return;
        if (dz > 6)
            return;
    }

    mCards[i].mX = 0x80000;
    mCards[i].mY = 0x30000;
    mCards[i].mActive = 0;

    if (mFlashMode == 0) {
        if (mTarget == mCards[i].mFace) {
            mCorrect = 1;
        } else {
            if (MG_RAND(2) == 0) {
                mCorrect = 0;
            } else {
                mTarget = mCards[i].mFace;
                mEspModel.mTextureFrame = mCards[i].mFace;
                mCorrect = 1;
            }
        }
    } else {
        if (mTarget == mCards[i].mFace) {
            mCorrect = 1;
        } else {
            mCorrect = 0;
        }
    }

    mRoundState = 2;
    mPhase = 0;
    mTimer = 0x90;
    if (mCorrect == 0) {
        mTimer += 0x40;
    }

    mEspModel.StartHit();
}

void dScMg3DEsp_c::CardTouch(int idx) {
    unsigned int i = gActivePlayerSlot;
    int ok = 0;
    int v, w;

    if (gTouchHeld[i * 4] != 0) {
        if (gTouchEdge[i * 4] != 0) {
            ok = 1;
        }
    }
    if (ok == 0) return;

    v = gTouchX[gActivePlayerSlot * 4] - (mCards[idx].mX >> 12);
    w = gTouchY[gActivePlayerSlot * 4] - (mCards[idx].mY >> 12);

    if (v < -0xa) return;
    if (v > 0xa) return;
    if (w < -0x18) return;
    if (w > 0x18) return;

    mTouched += 1;
    mCards[idx].mTimer = 0x20;
    mCards[idx].mSpeed = 0;
    mCards[idx].mState = 2;

    int dy = 0x30 - (mCards[idx].mY >> 12);
    int dx = 0x80 - (mCards[idx].mX >> 12);
    mCards[idx].mAngle = cstd::atan2(dy, dx);

    SpawnRows(idx);
    Sound::PlayBank2_2D(0x190);
}

void dScMg3DEsp_c::CardExit(int i) {
    unsigned char st = mFade;
    if (st == 1) {
        if (mCards[i].mPriority == 1)
            mCards[i].mPriority = 0;
    }
    if (mFade >= 0x10) {
        mCards[i].mState = 1;
    }
}

void dScMg3DEsp_c::UpdateCards()
{
    int i;
    for (i = 0; i < 5; i++) {
        if (mCards[i].mActive) {
            (this->*s3DEspCardTable[mCards[i].mState])(i);
        }
    }
}

extern unsigned char sEsp3DCardsPerDifficulty[];
extern int volatile data_ov006_0213c818[];

void dScMg3DEsp_c::DealCards() {
    int a[5];
    int b[5];
    int n;
    int i;
    int k;
    int l;
    int m;
    int j;
    int v;
    int o;

    n = sEsp3DCardsPerDifficulty[mDifficulty];
    mTarget = MG_RAND(5);
    for (i = 0; i < 5; i++) {
        a[i] = 0;
        b[i] = 0;
    }
    for (i = 0; i < 5; i++) {
        do {
            k = MG_RAND(5);
        } while (b[k] != 0);
        b[k] = 1;
        a[i] = k & 0xff;
    }
    m = MG_RAND(n);
    v = mTarget;
    for (j = 0; j < 5; j++) {
        if (v == a[j]) {
            a[j] = a[m];
            a[m] = v;
            break;
        }
    }
    l = 0;
    if (l < n) {
        o = 0;
        do {
            mCards[l].mX = ((unsigned short *)data_ov006_0213c818[n - 2])[o] << 12;
            mCards[l].mY = ((unsigned short *)data_ov006_0213c818[n - 2])[l * 2 + 1] << 12;
            mCards[l].mActive = 1;
            mCards[l].mVisible = 1;
            mCards[l].mPriority = 1;
            mCards[l].mFace = (unsigned char)a[l];
            mCards[l].mState = 0;
            o += 2;
            l++;
        } while (l < n);
    }
    mEspModel.mTextureFrame = mTarget;
}

void dScMg3DEsp_c::ClearCards() {
    int i;
    for (i = 0; i < 5; i++) {
        mCards[i].mX = 0;
        mCards[i].mY = 0;
        mCards[i].mTimer = 0;
        mCards[i].mActive = 0;
        mCards[i].mVisible = 0;
        mCards[i].mFace = 0;
        mCards[i].mPriority = 0;
    }
}

void dScMg3DEsp_c::SetDifficulty()
{
    int v = mLevel;
    int mult = 2;
    if (v >= 0xf) {
        mDifficulty = 3;
    } else if (v >= 0xa) {
        mDifficulty = 2;
    } else if (v >= 5) {
        mDifficulty = 1;
    } else {
        mDifficulty = 0;
        mult = 3;
    }
    mFlashMode = MG_RAND(mult);
}

void dScMg3DEsp_c::Resolve()
{
    UpdateRows();
    UpdateCards();
}

void dScMg3DEsp_c::Await() {
    int i;
    UpdateCards();
    if (mTouched == 0)
        return;
    mPhase = 3;
    for (i = 0; i < 5; i++) {
        if (mCards[i].mActive != 0) {
            if (mCards[i].mState != 2) {
                mCards[i].mActive = 0;
                mCards[i].mVisible = 0;
            }
        }
    }
}

void dScMg3DEsp_c::Fade()
{
    mFadeTick += 1;
    if (mFadeTick >= 4) {
        mFadeTick = 0;
        mFade += 1;
        G2S_SetBlendAlpha(0, 4, mFade, 0x10 - mFade);
    }
    UpdateCards();
    if (mFade < 0x10)
        return;
    mFade = 0;
    mFadeTick = 0;
    mPhase = 2;
    mTimer = 0;
}

void dScMg3DEsp_c::Deal() {
    if (mTimer != 0) {
        mTimer -= 1;
        if ((s16)mTimer < 0)
            mTimer = 0;
        return;
    }
    SetDifficulty();
    mEspModel.StartIntro();
    DealCards();
    mPhase = 1;
    FreeGfxSlotsById(0x1d);
    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }
}

void dScMg3DEsp_c::ResetRound() {
    ClearCards();
    mTimer = 0;
    mFadeTick = 0;
    mFade = 0;
    mTouched = 0;
    mCorrect = 0;
    mFlashMode = 0xff;
    mFlashStep = 0;
    mIdleTimer = 0;
    mFlashBeats = 0;
    mDone = 1;
}
extern unsigned int sEsp3DResultSfx[];

void dScMg3DEsp_c::Results() {
    UpdateRows();
    if (mTimer != 0) {
        mTimer--;
        if (mTimer == 0x40) {
            if (mCorrect != 0) {
                int idx = gMgScene != 0 ? gMgScene->mHudScore : 0;
                while (idx >= 5)
                    idx -= 5;
                if (gMgScene != 0) {
                    dScMgBase_c *g = gMgScene;
                    if (g->mHudScore < 9999)
                        g->mHudScore++;
                    if (g->mHudScore > g->mBestHudScore)
                        g->mBestHudScore = g->mHudScore;
                }
                Sound::PlayBank2_2D(sEsp3DResultSfx[idx]);
            } else {
                func_02012790(0xe);
            }
        }
        if ((s16)mTimer > 0)
            return;
        if (mCorrect != 0) {
            int rem, quot;
            int score = gMgScene != 0 ? gMgScene->mHudScore : 0;
            Mg_SubmitScore(score);
            rem = gMgScene != 0 ? gMgScene->mHudScore : 0;
            quot = 0;
            while (rem >= 5) {
                rem -= 5;
                quot++;
            }
            if (rem == 0 && quot != 0) {
                if (quot <= 2)
                    Mg_SetNextBanner(0x14);
                else
                    Mg_SetNextBanner(0x15);
                Mg_SetSharedState(MG_STATE_ROUND_CLEAR);
            } else {
                mLevel++;
                if (mLevel > 0x270e)
                    mLevel = 0x270e;
                StartRound(-1);
            }
        } else {
            mIdleTimer = 0;
            mDone = 0;
            Mg_ShowBanner(8, 0x80, 0xa0, 1, -1, 0xd);
            Mg_SetSharedState(MG_STATE_GAME_OVER);
            mEspModel.mMuted = 1;
        }
        mPromptEnabled = 0;
        mTimer = 0;
    } else {
        int b;
        u8 i;
        if (mCorrect != 0)
            return;
        b = 0;
        i = gActivePlayerSlot;
        if (gTouchHeld[i * 4] != 0) {
            if (gTouchEdge[i * 4] != 0)
                b = 1;
        }
        if (b != 0)
            ClearCards();
        mIdleTimer++;
        if (mIdleTimer == 0xb4)
            ClearCards();
        if (mIdleTimer >= 0xb5)
            mIdleTimer = 0xb5;
    }
}

void dScMg3DEsp_c::Play()
{
    (this->*s3DEspPhaseTable[mPhase])();
    UpdateFlash();
}

void dScMg3DEsp_c::Wait()
{
    if (mTimer != 0) {
        mTimer -= 1;
        if ((s16)mTimer < 0)
            mTimer = 0;
        return;
    }
    mRoundState = 1;
    mTimer = 0x40;
}

/* ---- dScMg3DEsp_c ------------------------------------------------------ */

/* Vtable slot 20, OnNewRecord: frees the banner graphic in slot 8 when the
   shared results state shows a new record. See dScMgBase_c.h. */
void dScMg3DEsp_c::OnNewRecord()
{
    FreeGfxSlotsById(8);
}

/* Vtable slot 18. The name is a placeholder borrowed from dActor_c by slot
   index; see dScMgBase_c.h. The signature repeats the base declaration, or
   mwcc appends a slot instead of overriding. */
void dScMg3DEsp_c::StartRound(int a)
{
    ResetRound();
    mRoundState = 0;
    if (a == 0) {
        mLevel++;
        if (mLevel > 9998) mLevel = 9998;
    } else if (a == 0x12) {
        if (gMgScene != 0) gMgScene->mHudScore = 0;
        mLevel = 0;
        if (mLevel > 9998) mLevel = 9998; /* the ROM keeps this clamp; 16 bytes */
    }
    mEspModel.mMuted = 0;
    mEspModel.Reset();
    Mg_ShowBanner(0xd, 0x80, 0xa8, 1, -1, 0xd);
}

/* Vtable slot 3. Releases the files of the two models; InitResources loads
   mModel1 from the first handle and mModel2 from the second. */
s32 dScMg3DEsp_c::CleanupResources()
{
    gEspIrieModelFile.Release();
    gIrieBackModelFile.Release();
    return 1;
}

/* Vtable slot 9. Calls four helpers, resets the camera, then draws the
   first model, the dMg3DEspModel_c block and the second model. */
s32 dScMg3DEsp_c::Render()
{
    DrawBanner();
    RenderCards();
    RenderRows();
    RenderSlots();
    mCamera.mLookAt.x = 0;
    mCamera.mLookAt.y = 0xd0000;
    mCamera.mLookAt.z = 0x40000;
    mCamera.mPos.x = 0xffed3000;
    mCamera.mPos.y = 0xe0000;
    mCamera.mPos.z = 0x40000;
    mCamera.mFovAngle = 0xc00;
    Camera_UpdateMatrices(&mCamera);
    /* dExtSimpleModel_c::data, at +8. Naming the member makes mwcc add the 8 in a separate instruction. */
    mTextureTransformer.Update(mModel1.data);
    mModel1.Render(0);
    mEspModel.Render();
    mModel2.Render(0);
    return 1;
}

/* Vtable slot 6. Runs the current state from the member-function table,
   indexed by mRoundState, then steps the slot table, the texture animation
   and the dMg3DEspModel_c block. */
s32 dScMg3DEsp_c::Behavior()
{
    (this->*s3DEspRoundTable[mRoundState])();
    UpdateSlots();
    mTextureTransformer.Advance();
    mEspModel.Behavior();
    return 1;
}

/* Vtable slot 0. Sets up the camera and both display engines, loads the two
   models, the texture animation and the dMg3DEspModel_c block, then calls
   StartRound(-1). */
s32 dScMg3DEsp_c::InitResources()
{
    Matrix4x3 tmp;
    void *f;
    void *r5v;
    void *r4v;

    gMainScreenLayers = DISPLAY_LAYER_BG0 | DISPLAY_LAYER_OBJ;
    gSubScreenLayers = 0x10;
    G3X_SetFog(false, 0, 2, 0x1000);
    InitialiseVramGlobals();

    /* main BG0CNT: priority 1 */
    G2_SetBG0Priority(1);
    Mg_SetTopScreenYOffset(0x10);

    mCamera.mLookAt.x = 0;
    mCamera.mLookAt.y = 0xd0000;
    mCamera.mLookAt.z = 0x40000;
    mCamera.mPos.x = 0xffed3000;
    mCamera.mPos.y = 0xe0000;
    mCamera.mPos.z = 0x40000;
    mCamera.mFovAngle = 0xc00;
    Camera_UpdateMatrices(&mCamera);

    if (func_020179b4(&gEspIrieModelFile, &mModel1, 1) == 0)
        return 0;

    dExtAnmTexSRT_c::Prepare(**(BMD_File **)((void **)&gEspIrieModelFile + 1),
                             *(BTA_File *)&sEsp3DTextureAnim);
    mTextureTransformer.SetFile(*(BTA_File *)&sEsp3DTextureAnim, 0, 0x1000, 0);

    if (mEspModel.InitResources() == 0)
        return 0;

    if (func_020179b4(&gIrieBackModelFile, &mModel2, 1) == 0)
        return 0;

    tmp = (Matrix4x3 &)sEsp3DIdentityMtx;
    mModel1.mat4x3 = (Matrix4x3 &)sEsp3DIdentityMtx;
    mModel2.mat4x3 = tmp;

    gSubScreenLayers |= 4;
    /* sub BG2CNT, then the sub BG2 scroll offsets */
    G2S_SetBG2Priority(0);
    G2S_BG2Mosaic(0);
    G2S_SetBG2Offset(0, 0);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1000,
                          GX_BG_CHARBASE_0x10000);

    f = LoadFile(0x12);
    DecompressLZ16(f, (void *)((unsigned)G2S_GetBG2CharPtr() + 0x4000));
    Deallocate(f);

    f = LoadFile(0x13);
    GXS_LoadBGPltt(f, 0x1e0, 0x20);
    Deallocate(f);

    f = LoadFile(0x14);
    GXS_LoadBG2Scr(f, 0, 0x800);
    Deallocate(f);

    r5v = LoadFile(8);
    r4v = LoadFile(9);
    DecompressLZ16(r5v, G2S_GetOBJCharPtr());
    GXS_LoadOBJPltt(r4v, 0, 0x100);
    Deallocate(r5v);
    Deallocate(r4v);

    Mg_SetTopScreenYOffset(0x20);

    StartRound(-1);

    mTimer = 0x40;

    mCredits = 3;
    mCreditsShown = mCredits;
    mRoundState = 1;

    ResetSlots();

    mRunDuringFade = 1;

    Mg_ShowBanner(0xd, 0x80, 0xa8, 1, -1, 0xd);

    sMgClearBannerX = 0x80;
    sMgClearBannerY = ~0x3f;

    mHudScore = 0;

    return 1;
}

/* Reconstructed source-style name: SM64DS proves dScMg3DEsp_c through RTTI,
 * allocation size, vtable identity, and the MG_3DESP registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgPsycheOut_Spawn. */
dScMg3DEsp_c *dScMg3DEsp_c_classInit(void) {
    return new dScMg3DEsp_c;
}

/* Static-init globals (was the handwritten __sinit_ov006_02130a08 shard).
 * Retail order: the four model handles, the five animation handles, the
 * texture sequence, then the seven PMF state tables plain-copied from
 * the ROM PMF constants. */
EspAnimSetModelFilePtr gEspIrieModelFile(0x1ef);
EspAnimSetModelFilePtr gIrieBackModelFile(0x202);
EspAnimSetModelFilePtr gEspCardModelFile(0x1e7);
EspAnimSetModelFilePtr gEspHamonModelFile(0x1ee);
EspAnimSetAnimationFileHandle gEspCardStartAnimFile(0x1eb);
EspAnimSetAnimationFileHandle gEspCardWaitAnimFile(0x1ec);
EspAnimSetAnimationFileHandle gEspCardOpenAnimFile(0x1e9);
EspAnimSetAnimationFileHandle gEspCardOpenWaitAnimFile(0x1ea);
EspAnimSetAnimationFileHandle gEspHamonAnimFile(0x1ed);
EspAnimSetTexSequenceFilePtr gEspCardTexSeqFile(0x1e8);

dScMg3DEsp_cState s3DEspRoundTable[3] = {
    pmf_dScMg3DEsp_c_Wait,
    pmf_dScMg3DEsp_c_Play,
    pmf_dScMg3DEsp_c_Results,
};
dScMg3DEsp_cState s3DEspPhaseTable[4] = {
    pmf_dScMg3DEsp_c_Deal,
    pmf_dScMg3DEsp_c_Fade,
    pmf_dScMg3DEsp_c_Await,
    pmf_dScMg3DEsp_c_Resolve,
};
dScMg3DEsp_cObjState s3DEspCardTable[3] = {
    pmf_dScMg3DEsp_c_CardExit,
    pmf_dScMg3DEsp_c_CardTouch,
    pmf_dScMg3DEsp_c_CardSeek,
};
dScMg3DEsp_cObjState s3DEspRowTable[3] = {
    pmf_dScMg3DEsp_c_RowWait,
    pmf_dScMg3DEsp_c_RowDecay,
    pmf_dScMg3DEsp_c_RowFade,
};
dScMg3DEsp_cObjState s3DEspSlotFallTable[2] = {
    pmf_dScMg3DEsp_c_SlotWait,
    pmf_dScMg3DEsp_c_SlotFall,
};
dScMg3DEsp_cObjState s3DEspSlotRiseTable[4] = {
    pmf_dScMg3DEsp_c_SlotSetup,
    pmf_dScMg3DEsp_c_SlotRise,
    pmf_dScMg3DEsp_c_SlotKick,
    pmf_dScMg3DEsp_c_SlotDampen,
};
dScMg3DEsp_cState s3DEspFlashTable[3] = {
    pmf_dScMg3DEsp_c_FlashOnce,
    pmf_dScMg3DEsp_c_FlashSync,
    pmf_dScMg3DEsp_c_FlashQuick,
};
