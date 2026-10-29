//cpp
struct Vector3_16f;
namespace Particle {
struct Callback;
struct System {
    static int New(unsigned, unsigned, int, int, int, const ::Vector3_16f *, Callback *);
};
}
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
 * Each pragma bracket below carries the file-global pragma of the file
 * that function came from; the brackets bind only under defer_codegen off.
 *
 * deslop leftovers:
 * - The scene's cards, rows and slots are the header's Card, Row and Slot
 *   arrays. RenderRows indexes mRows directly; RenderCards still reads its
 *   cards through EspAt5000 and FlashSync keeps the raw this+0x5034.
 * - Ent_8e10 stands in for a card sprite's part record; RenderCards walks
 *   the list until f6 is 0xffff.
 * - Obj_777c is a vtable shim so AnimSet::Render can virtual-call the
 *   dExtAnmModel_c method that takes a void* argument; naming the real slot
 *   needs the dExtAnmModel_c vtable recovered.
 * - dMg3DEspModel_c's unk_000..unk_008 are set by its constructor
 *   (0x8c000, 0x8c000, 0) and read nowhere in this unit, so they stay
 *   unnamed.
 */

#pragma defer_codegen off

#include "common.h"
#include "G2.h"
#include "G2x.h"
#include "GX.h"
#include "types.h"
#include "nitro/fx/fx.h"
#include "SharedFilePtr.h"
#include "dScMg3DEsp_c.h"
#include "dMg3DEspModel_c.h"
#include "dMg3DEspAnimSet_c.h"
#include "dExtAnmTexSRT_c.h"
#include "dExtAnmTexPat_c.h"
#include "dExtAnmMaterial_c.h"
#include "G3X.h"
#include "Sound.h"
#include "PlayerInput.h"
#include "dExtSimpleModel_c.h"
#include "cstd.h"
#include "math/Approach.h"

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

extern "C" {
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
void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
void func_02046208(char* a, int b, int c);
int func_020179b4(void* r0, void* r1, int r2);
s16 atan2_hold_d_mg3d_esp_anim_set_1(s32 y, s32 x);
int RandomIntInternal(int *seed);
extern int data_0209d4b8;
void FreeGfxSlotsById(int a);
void Camera_UpdateMatrices(void* self);
int Mg_RenderSubSpriteScaled(int a0, int a1, int a2, int a3, int a4, int a5, u16 a6);
void Mg_RenderSubSpriteMtx(void* a, int b, int c, void* m);
void Mg_DrawScoreTally(void *c, int a, int b, int d);
void Mg_SubmitScore(int);
void Mg_SetNextBanner(int);
void Mg_SetSharedState(int);
void Mg_ShowBanner(int, int, int, int, int, short);
void Mg_SetTopScreenYOffset(int);
void InitialiseVramGlobals(void);
void Deallocate(void*);
int LoadFile(int handle);
void DecompressLZ16(void *src, void *dst);
void func_02056374(const void*, u32, u32);

extern int sEsp3DMaterialAnim;
extern int pmf_dMg3DEspModel_c_Idle_2[];
extern dMg3DEspModel_c::State pmf_dMg3DEspModel_c_Idle;
extern dMg3DEspModel_c::State pmf_dMg3DEspModel_c_Hit;
extern dMg3DEspModel_c::State pmf_dMg3DEspModel_c_Intro;
extern dMg3DEspModel_c::State pmf_dMg3DEspModel_c_Wait;
extern dMg3DEspModel_c::State pmf_dMg3DEspModel_c_Idle_3;
extern void *sEsp3DTextureAnim;
extern s16 data_02082214[];
extern unsigned char data_0209d45c;
extern unsigned char data_0209d454;
extern int sMgClearBannerX;
extern int sMgClearBannerY;
}

extern Matrix4x3 data_ov006_0213c85c;

/* Both models' starting matrix. Declaring it non-volatile adds 32 bytes to
   dScMg3DEsp_c::InitResources, and copying it without the stack temporary
   removes 32. */
extern volatile Matrix4x3 sEsp3DIdentityMtx;

/* ---- dScMg3DEsp_c: the destructor (key function) ---------------------- */

/* The body is empty: the members' destructors (reverse declaration order),
   the vtable stores, mSysTracker's destruction and the chain to
   ~dScMgBase_c are all compiler-generated; the deleting variant reaches
   dScMgBase_c's operator delete. */
// @symbol _ZN12dScMg3DEsp_cD1Ev
// @symbol _ZN12dScMg3DEsp_cD0Ev
dScMg3DEsp_c::~dScMg3DEsp_c()
{
}

/* ---- dMg3DEspAnimSet_c ------------------------------------------------- */

#pragma push
#pragma opt_strength_reduction off

struct Obj_777c {
    void* vt;
    virtual void v0(); virtual void v1(); virtual void v2();
    virtual void v3(); virtual void v4();
    virtual void m(void* arg);
};

// @symbol _ZN17dMg3DEspAnimSet_c6RenderEv
void dMg3DEspAnimSet_c::Render()
{
    char *c = (char *)this;
    int i;
    char *r5;
    char *r4;
    void *zero;
    Matrix4x3_FromTranslation(&data_020a0e68, 0x8c000, 0x80000, 0x40000);
    r5 = c;
    r4 = (char *)mMaterialChangers;
    i = 0;
    zero = (void*)i;
    do {
        if (mActive[i] != 0) {
            ((dExtAnmModel_c *)r5)->mat4x3 = data_020a0e68;
            ((dExtAnmMaterial_c *)r4)->Update(*(ModelComponents *)(r5 + 8));
            ((Obj_777c*)r5)->m(zero);
        }
        i++;
        r5 += 0x64;
        r4 += 0x14;
    } while (i < 3);
}

// @symbol _ZN17dMg3DEspAnimSet_c8BehaviorEv
void dMg3DEspAnimSet_c::Behavior() {
    char *self = (char *)this;
    int sb = 0;
    if (mRepeats != 0) {
        if (ApproachLinear2(mPhase, 0, 1)) {
            mPhase = mPeriod;
            sb = 1;
            if (mRepeats > 0) {
                mRepeats -= 1;
            }
        }
    }
    {
        int zero = 0;
        int i;
        char *a = self;
        char *b = (char *)mMaterialChangers;
        /* cc steps by one dExtAnmMaterial_c: mMaterialChangers[i] grows
           Behavior from 0xf8 to 0xfc bytes. */
        char *cc = self;
        for (i = 0; i < 3; i++) {
            if (mActive[i] != 0) {
                ((dExtAnmModel_c *)a)->Advance();
                ((dExtAnmMaterial_c *)b)->Advance();
                if (((dExtAnmModel_c *)a)->Finished()) {
                    mActive[i] = 0;
                }
            } else {
                if (sb == 1) {
                    mActive[i] = 1;
                    ((dExtAnmModel_c *)a)->currFrame = zero;
                    sb = zero;
                    ((dMg3DEspAnimSet_c *)cc)->mMaterialChangers[0].currFrame = zero;
                }
            }
            a += 0x64;
            b += 0x14;
            cc += 0x14;
        }
    }
}

// @symbol _ZN17dMg3DEspAnimSet_c5ResetEv
void dMg3DEspAnimSet_c::Reset()
{
    mRepeats = 0;
    mPeriod = 30;
    mPhase = 0;
}

// @symbol _ZN17dMg3DEspAnimSet_c10SetRepeatsEs
void dMg3DEspAnimSet_c::SetRepeats(s16 v)
{
    mRepeats = v;
    mPhase = 0;
}

// @symbol _ZN17dMg3DEspAnimSet_c9SetPeriodEs
void dMg3DEspAnimSet_c::SetPeriod(s16 v)
{
    mPeriod = v;
}

// @symbol _ZN17dMg3DEspAnimSet_c8SetSpeedEi
void dMg3DEspAnimSet_c::SetSpeed(s32 v)
{
    mSpeed = v;
}

// @symbol _ZN17dMg3DEspAnimSet_c13InitResourcesEv
void dMg3DEspAnimSet_c::InitResources()
{
    char *o = (char *)this;
    int bca;
    int m1 = -1;
    int z1 = 0;
    int z2 = 0;
    int bmd;
    int i;
    char *w;
    char *w2;
    mSpeed = 0x800;
    bmd = (int)dExtSimpleModel_c::LoadFile(gEspHamonModelFile);
    bca = (int)dExtFrameCtrl_c::LoadFile(gEspHamonAnimFile);
    dExtAnmMaterial_c::Prepare(*(BMD_File *)bmd, *(BMA_File *)&sEsp3DMaterialAnim);
    w = o;
    w2 = (char *)mMaterialChangers;
    for (i = 0; i < 3; i++) {
        ((dExtModel_c *)w)->SetFile((BMD_File *)bmd, 1, m1);
        ((dExtAnmModel_c *)w)->SetAnim((BCA_File *)bca, 0x40000000, mSpeed, z1);
        ((dExtAnmMaterial_c *)w2)
            ->SetFile(*(BMA_File *)&sEsp3DMaterialAnim, 0x40000000, mSpeed, z2);
        ((dExtSimpleModel_c *)w)->SetPolygonID((i + 1) & 0xff);
        mActive[i] = 0;
        w += 0x64;
        w2 += 0x14;
    }
    mRepeats = 0;
    mPhase = 0;
}

// @symbol _ZN17dMg3DEspAnimSet_cD1Ev
dMg3DEspAnimSet_c::~dMg3DEspAnimSet_c()
{
    gEspHamonModelFile.Release();
    gEspHamonAnimFile.Release();
}

// @symbol _ZN17dMg3DEspAnimSet_cC1Ev
dMg3DEspAnimSet_c::dMg3DEspAnimSet_c() {}

#pragma pop

/* ---- dMg3DEspModel_c --------------------------------------------------- */

// @symbol _ZN15dMg3DEspModel_c14ResetTransformEv
void dMg3DEspModel_c::ResetTransform()
{
    mAnmModel.mat4x3 = data_ov006_0213c85c;
}

// @symbol _ZN15dMg3DEspModel_c6RenderEv
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

// @symbol _ZN15dMg3DEspModel_c8BehaviorEv
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
        (u32)Particle::System::New(mParticleSys, 0xec, 0x48c000, 0x140000, 0x200000, 0, 0);
}

// @symbol _ZN15dMg3DEspModel_c3HitEv
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

// @symbol _ZN15dMg3DEspModel_c8StartHitEv
void dMg3DEspModel_c::StartHit()
{
    mAnmModel.SetAnim((BCA_File *)gEspCardOpenAnimFile.filePtr, 0x40000000, 0x800, 0);
    if (mMuted == 0)
        Sound::PlayBank2_2D(0x191);
    mState = pmf_dMg3DEspModel_c_Hit;
}

// @symbol _ZN15dMg3DEspModel_c5IntroEv
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

// @symbol _ZN15dMg3DEspModel_c10StartIntroEv
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

// @symbol _ZN15dMg3DEspModel_c4WaitEv
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
// @symbol _ZN15dMg3DEspModel_c9StartWaitEv
void dMg3DEspModel_c::StartWait()
{
    mAnmModel.SetAnim((BCA_File *)gEspCardWaitAnimFile.filePtr, 0, 0x800, 0);
    mState = pmf_dMg3DEspModel_c_Wait;
}

// @symbol _ZN15dMg3DEspModel_c4IdleEv
void dMg3DEspModel_c::Idle()
{
}

// @symbol _ZN15dMg3DEspModel_c5ResetEv
void dMg3DEspModel_c::Reset()
{
    mAnimSet.Reset();
    mShowSparks = 0;
    mState = pmf_dMg3DEspModel_c_Idle_3;
}

// @symbol _ZN15dMg3DEspModel_c13InitResourcesEv
int dMg3DEspModel_c::InitResources()
{
    char* c = (char*)this;
    if (func_020179b4(&gEspCardModelFile, (char *)&mAnmModel, 1) == 0)
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

// @symbol _ZN15dMg3DEspModel_cD1Ev
dMg3DEspModel_c::~dMg3DEspModel_c()
{
    gEspCardModelFile.Release();
    gEspCardStartAnimFile.Release();
    gEspCardWaitAnimFile.Release();
    gEspCardOpenAnimFile.Release();
    gEspCardOpenWaitAnimFile.Release();
    gEspCardTexSeqFile.Release();
}

// @symbol _ZN15dMg3DEspModel_cC1Ev
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
extern "C" dScMg3DEsp_cState s3DEspRoundTable[];
extern "C" dScMg3DEsp_cState s3DEspPhaseTable[];
extern "C" dScMg3DEsp_cState s3DEspFlashTable[];
extern "C" dScMg3DEsp_cObjState s3DEspSlotFallTable[];
extern "C" dScMg3DEsp_cObjState s3DEspCardTable[];
extern "C" dScMg3DEsp_cObjState s3DEspRowTable[];
extern "C" dScMg3DEsp_cObjState s3DEspSlotRiseTable[];

/* The ROM PMF constants the seven state tables copy from (unlicensed
 * .data), grouped by destination table in retail copy order. */
extern "C" dScMg3DEsp_cState pmf_dScMg3DEsp_c_Wait;
extern "C" dScMg3DEsp_cState pmf_dScMg3DEsp_c_Play;
extern "C" dScMg3DEsp_cState pmf_dScMg3DEsp_c_Results;
extern "C" dScMg3DEsp_cState pmf_dScMg3DEsp_c_Deal;
extern "C" dScMg3DEsp_cState pmf_dScMg3DEsp_c_Fade;
extern "C" dScMg3DEsp_cState pmf_dScMg3DEsp_c_Await;
extern "C" dScMg3DEsp_cState pmf_dScMg3DEsp_c_Resolve;
extern "C" dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_CardExit;
extern "C" dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_CardTouch;
extern "C" dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_CardSeek;
extern "C" dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_RowWait;
extern "C" dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_RowDecay;
extern "C" dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_RowFade;
extern "C" dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_SlotWait;
extern "C" dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_SlotFall;
extern "C" dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_SlotSetup;
extern "C" dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_SlotRise;
extern "C" dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_SlotKick;
extern "C" dScMg3DEsp_cObjState pmf_dScMg3DEsp_c_SlotDampen;
extern "C" dScMg3DEsp_cState pmf_dScMg3DEsp_c_FlashOnce;
extern "C" dScMg3DEsp_cState pmf_dScMg3DEsp_c_FlashSync;
extern "C" dScMg3DEsp_cState pmf_dScMg3DEsp_c_FlashQuick;

// @symbol _ZN12dScMg3DEsp_c10DrawBannerEv
void dScMg3DEsp_c::DrawBanner()
{
    char *c = (char*)this;
    if (mDone != 0)
        Mg_DrawScoreTally(c, 0xe0, 0x14, 1);
}

/* Early-out once mFlashStep is set; otherwise set the embedded model's
   mPolygonID to 0x1e and step mFlashStep. */
// @symbol _ZN12dScMg3DEsp_c10FlashQuickEv
void dScMg3DEsp_c::FlashQuick() {
    if (mFlashStep)
        return;
    mEspModel.mPolygonID = 0x1e;
    mFlashStep += 1;
}

// @symbol _ZN12dScMg3DEsp_c9FlashSyncEv
void dScMg3DEsp_c::FlashSync()
{
    char* c = (char*)this;
    u8 state = mFlashStep;
    if (state == 0) {
        u8 *p = &mFlashStep;
        mEspModel.mPolygonID = 0x1e;
        (*p)++;
        return;
    }
    if (state != 1)
        return;
    /* c + 0x5034 is mEspModel's mAnmModel frame control
       (0x4fd8 + 0xc + 0x50). The ROM pool-loads 0x5034 as one constant;
       spelling it as that member grows FlashSync from 0xb4 to 0xb8 bytes. */
    if (((dExtFrameCtrl_c *)(c + 0x5034))->WillHitFrame(0)) {
        u8 *p = &mFlashBeats;
        (*p)++;
    }
    if (mFlashBeats < 1)
        return;
    {
        u8 *p = &mFlashStep;
        (*p)++;
    }
    mEspModel.mPolygonID = 0x1f;
}

// @symbol _ZN12dScMg3DEsp_c9FlashOnceEv
void dScMg3DEsp_c::FlashOnce() {
    unsigned char *p;
    unsigned char k;

    if (mFlashStep != 0) {
        return;
    }
    p = &mFlashStep;
    *p = (unsigned char)(*p + 1);
    k = 0x1f;
    mEspModel.mPolygonID = k;
}

// @symbol _ZN12dScMg3DEsp_c11UpdateFlashEv
void dScMg3DEsp_c::UpdateFlash()
{
    unsigned char idx = mFlashMode;
    if (idx >= 3) return;
    (this->*s3DEspFlashTable[idx])();
}

extern void *sEsp3DAnimSetSlotsSprites[];

// @symbol _ZN12dScMg3DEsp_c11RenderSlotsEv
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

// @symbol _ZN12dScMg3DEsp_c10SlotDampenEi
void dScMg3DEsp_c::SlotDampen(int i) {
    char *c = (char *)this;
    unsigned long new_var;
    /* b steps `this` by one slot, so b->mSlots[0] is mSlots[i]. Spelled as
     mSlots[i] the code is the same, but without b the file's @NNN labels
     renumber by one. */
    dScMg3DEsp_c *b = (dScMg3DEsp_c *)(c + (i * 32));
    new_var = i;
    mSlots[new_var].mX = mSlots[new_var].mX + b->mSlots[0].mVelX;
    mSlots[new_var].mY = mSlots[new_var].mY + b->mSlots[0].mVelY;
    if (b->mSlots[0].mVelX > 0) {
        unsigned char e = b->mSlots[0].mAccel;
        mSlots[new_var].mVelX = mSlots[new_var].mVelX - ((e * 16) + 0x10);
        if (((short)b->mSlots[0].mVelX) < 0) {
            b->mSlots[0].mVelX = 0;
        }
    } else if (b->mSlots[0].mVelX < 0) {
        unsigned char e = b->mSlots[0].mAccel;
        mSlots[new_var].mVelX = mSlots[new_var].mVelX + ((e * 16) + 0x10);
        if (((short)b->mSlots[0].mVelX) > 0) {
            b->mSlots[0].mVelX = 0;
        }
    } else {
        b->mSlots[0].mType = 0;
    }
}

#pragma push
#pragma opt_common_subs off
#pragma opt_strength_reduction off
// @symbol _ZN12dScMg3DEsp_c8SlotKickEi
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
    {
        int rnd = (int)((((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff);
        /* ROM uses lsr (logical), not asr: keep unsigned through the shifts */
        rnd = (int)((((unsigned int)rnd) << 4) >> 0xf);
        rnd = rnd + 0x10;
        mSlots[i].mDelay = (short)(rnd & 0xff);
    }
    mSlots[i].mType = 3;
}
#pragma pop

#pragma push
#pragma opt_common_subs off
// @symbol _ZN12dScMg3DEsp_c8SlotRiseEi
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

    {
        unsigned r = (unsigned)RandomIntInternal(&data_0209d4b8);
        unsigned t = (r >> 16) & 0x7fff;
        t = (t << 4) >> 15;
        mSlots[i].mDelay = (unsigned char)(t + 0x10);
        mSlots[i].mType = 3;
    }
}
#pragma pop

extern unsigned char data_ov006_0212e57c[];

#pragma push
#pragma opt_common_subs off
// @symbol _ZN12dScMg3DEsp_c9SlotSetupEi
void dScMg3DEsp_c::SlotSetup(int idx) {
    unsigned int r;
    mSlots[idx].mVelX = 0;
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    mSlots[idx].mAccel = (unsigned char)((r << 3) >> 15);
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    mSlots[idx].mGravity = (unsigned char)((r << 3) >> 15);
    mSlots[idx].mVisible = 1;
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    mSlots[idx].mDelay = (unsigned char)(((r << 4) >> 15) + 8);
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    mSlots[idx].mPushTime = (unsigned char)(((r << 5) >> 15) + 0x10);
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    mSlots[idx].mType = data_ov006_0212e57c[(r << 1) >> 15];
}
#pragma pop

#pragma push
#pragma opt_common_subs off
// @symbol _ZN12dScMg3DEsp_c8SlotFallEi
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
#pragma pop

#pragma push
#pragma opt_common_subs off
// @symbol _ZN12dScMg3DEsp_c8SlotWaitEi
void dScMg3DEsp_c::SlotWait(int idx)
{
    if (mSlots[idx].mWait != 0) {
        mSlots[idx].mWait -= 1;
        if ((s16)mSlots[idx].mWait < 0)
            mSlots[idx].mWait = 0;
    } else {
        int rnd = (int)((((u32)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff);
        rnd = (int)((((u32)rnd) << 5) >> 15);
        mSlots[idx].mLife = rnd + 0x40;
        mSlots[idx].mState = 1;
        mSlots[idx].mType = 0;
        mSlots[idx].mVisible = 0;
        rnd = (int)((((u32)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff);
        mSlots[idx].mX = (int)((((u32)rnd) << 5) >> 15) << 15;
        rnd = (int)((((u32)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff);
        mSlots[idx].mY = ((int)((((u32)rnd) << 4) >> 15) << 3) + 0x40 << 12;
        mSlots[idx].mVelY = -0x200;
        mSlots[idx].mFrame = 0;
    }
}
#pragma pop

// @symbol _ZN12dScMg3DEsp_c11UpdateSlotsEv
void dScMg3DEsp_c::UpdateSlots() {
    for (int i = 0; i < 0x14; i++) {
        if (mSlots[i].mActive) {
            u8 k = mSlots[i].mState;
            (this->*s3DEspSlotFallTable[k])(i);
        }
    }
}

#pragma push
#pragma opt_strength_reduction off
// @symbol _ZN12dScMg3DEsp_c10ResetSlotsEv
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
#pragma pop

/* RenderCards reads its element through a this+0x5000 base: the ROM adds
   0x5000 to the stepping pointer and loads mCard's fields off it. Spelling
   them as dScMg3DEsp_c::mCards folds that split base into a hoisted element
   pointer (RenderCards grows from 0x104 to 0x118 bytes), so this view keeps
   the split and names the fields. */
struct EspAt5000 {
    char pad[0x208];
    dScMg3DEsp_c::Card mCard; /* 0x5208, mCards[0] */
    char pad2[0x54e - 0x220];
    u8 mFade; /* 0x554e */
};
typedef char EspAt5000_size_must_be_0x550[sizeof(EspAt5000) == 0x550 ? 1 : -1];

#define ESP_AT_5000(base) ((EspAt5000 *)((base) + 0x5000))

extern int sEsp3DAnimSetRowsSprites;

// @symbol _ZN12dScMg3DEsp_c10RenderRowsEv
void dScMg3DEsp_c::RenderRows()
{
    int i;
    for (i = 0; i < 3; i++) {
        if (mRows[i].mVisible != 0) {
            int x = mRows[i].mX >> 12;
            int y = mRows[i].mY >> 12;
            int priority = mRows[i].mPriority;
            int scale = mRows[i].mScale;
            int *o = (int *)Mg_RenderSubSpriteScaled(sEsp3DAnimSetRowsSprites, x, y, -1, priority,
                                                     scale, 0);
            if (mRows[i].mState == 2) {

                int w0 = o[0];
                int w1 = o[1];
                unsigned bits = (((unsigned)w1) << 0x10) >> 0x1c;
                o[0] = (w0 & (~0xc00)) | 0x400;
                *((unsigned short *)(((char *)o) + 4)) =
                    ((*((unsigned short *)(((char *)o) + 4))) & (~0xf000)) | (bits << 12);
            }
        }
    }
}

// @symbol _ZN12dScMg3DEsp_c7RowFadeEi
void dScMg3DEsp_c::RowFade(int i)
{
    int x = mRows[i].mScale;
    x = x - (x >> 7);
    mRows[i].mScale = x;
    mFade += 1;
    {
        unsigned char ip = mFade;
        G2x::SetBlendAlpha((volatile unsigned short *)0x4001050, 0, 4, 0x10 - ip, ip);
    }
    if (mFade < 0x10)
        return;
    mFade = 0;
    mRows[i].mVisible = 0;
    mRows[i].mActive = 0;
}

// @symbol _ZN12dScMg3DEsp_c8RowDecayEi
void dScMg3DEsp_c::RowDecay(int i)
{
    int x = mRows[i].mScale;
    x = x - (x >> 6);
    mRows[i].mScale = x;
    if (mRows[i].mScale <= 0x900) {
        mRows[i].mState = 2;
    }
}

// @symbol _ZN12dScMg3DEsp_c7RowWaitEi
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

// @symbol _ZN12dScMg3DEsp_c10UpdateRowsEv
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

// @symbol _ZN12dScMg3DEsp_c9SpawnRowsEi
void dScMg3DEsp_c::SpawnRows(int idx) {
    char *c = (char *)this;
    int i;
    /* e steps `this` by one card, so this reads mCards[idx]; spelled that
       way, SpawnRows grows from 0x94 to 0x9c bytes. */
    char *e = c + idx * 0x18;
    s32 *src1 = &((dScMg3DEsp_c *)e)->mCards[0].mX;
    s32 *src2 = &((dScMg3DEsp_c *)e)->mCards[0].mY;
    for (i = 0; i < 3; i++) {
        mRows[i].mActive = 1;
        mRows[i].mX = *src1;
        mRows[i].mY = *src2;
        mRows[i].mVisible = 0;
        mRows[i].mState = 0;
        mRows[i].mTimer = data_ov006_0212e58c[i];
        mRows[i].mPriority = 0;
        mRows[i].mScale = 0x1400;
    }
}

struct Ent_8e10 { u16 f0, f2, f4, f6; };
extern Ent_8e10 *data_ov006_02133f24[];

// @symbol _ZN12dScMg3DEsp_c11RenderCardsEv
void dScMg3DEsp_c::RenderCards() {
    char *c0 = (char *)this;
    int i;
    int x;
    int y;
    int c;
    Ent_8e10 *r7;
    char *c2;
    u8 b;
    c2 = c0;
    for (i = 0; i < 5; i++) {
        if (ESP_AT_5000(c2)->mCard.mVisible != 0) {
            b = ESP_AT_5000(c2)->mCard.mFace;
            x = ESP_AT_5000(c2)->mCard.mX >> 0xc;
            y = ESP_AT_5000(c2)->mCard.mY >> 0xc;
            c = ESP_AT_5000(c2)->mCard.mPriority;
            r7 = data_ov006_02133f24[b];
            for (;;) {
                int *r0 = (int *)Mg_RenderSubSpriteScaled((int)r7, x, y, -1, c, 0x1000, 0);
                if (ESP_AT_5000(c0)->mFade && !ESP_AT_5000(c2)->mCard.mState) {
                    int v = r0[0];
                    int w = r0[1];
                    int bits = (int)((unsigned int)(w << 0x10) >> 0x1c);
                    r0[0] = (v & ~0xc00) | 0x400;
                    *(u16 *)((char *)r0 + 4) =
                        (u16)((*(u16 *)((char *)r0 + 4) & ~0xf000) | (bits << 12));
                }
                if (r7->f6 == 0xffff)
                    break;
                r7++;
            }
        }
        c2 += 0x18;
    }
}

#undef ESP_AT_5000

// @symbol _ZN12dScMg3DEsp_c8CardSeekEi
void dScMg3DEsp_c::CardSeek(int i) {
    u16 timer;
    s16 tv1;
    s16 tv2;
    int speed2;
    int speed1;
    int angle1;

    timer = mCards[i].mTimer;
    if (timer != 0) {
        mCards[i].mTimer = timer - 1;
        if ((s16)mCards[i].mTimer <= 0) {
            mCards[i].mTimer = 0;
        }
        return;
    }

    speed1 = mCards[i].mSpeed;
    angle1 = mCards[i].mAngle;
    angle1 >>= 4;
    tv1 = data_02082214[(angle1 << 1) + 1];
    mCards[i].mX += FX_MUL(tv1, speed1);

    tv2 = data_02082214[(mCards[i].mAngle >> 4) << 1];
    speed2 = mCards[i].mSpeed;
    mCards[i].mY += FX_MUL(tv2, speed2);
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
            int rnd = RandomIntInternal(&data_0209d4b8);
            u32 temp = ((u32)rnd >> 16) & 0x7fff;
            if (((temp << 1) >> 0xf) == 0) {
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

// @symbol _ZN12dScMg3DEsp_c9CardTouchEi
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

// @symbol _ZN12dScMg3DEsp_c8CardExitEi
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

// @symbol _ZN12dScMg3DEsp_c11UpdateCardsEv
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

// @symbol _ZN12dScMg3DEsp_c9DealCardsEv
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
    mTarget =
        (unsigned char)(((((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff) * 5 >>
                        15);
    for (i = 0; i < 5; i++) {
        a[i] = 0;
        b[i] = 0;
    }
    for (i = 0; i < 5; i++) {
        do {
            k = (int)(((((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff) * 5 >> 15);
        } while (b[k] != 0);
        b[k] = 1;
        a[i] = k & 0xff;
    }
    m = (int)((n * ((((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff)) >> 15);
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

// @symbol _ZN12dScMg3DEsp_c10ClearCardsEv
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

// @symbol _ZN12dScMg3DEsp_c13SetDifficultyEv
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
    unsigned int rnd = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    mFlashMode = (unsigned char)((mult * rnd) >> 0xf);
}

// @symbol _ZN12dScMg3DEsp_c7ResolveEv
void dScMg3DEsp_c::Resolve()
{
    UpdateRows();
    UpdateCards();
}

// @symbol _ZN12dScMg3DEsp_c5AwaitEv
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

// @symbol _ZN12dScMg3DEsp_c4FadeEv
void dScMg3DEsp_c::Fade()
{
    mFadeTick += 1;
    if (mFadeTick >= 4) {
        mFadeTick = 0;
        mFade += 1;
        G2x::SetBlendAlpha((volatile unsigned short *)0x4001050, 0, 4, mFade, 0x10 - mFade);
    }
    UpdateCards();
    if (mFade < 0x10)
        return;
    mFade = 0;
    mFadeTick = 0;
    mPhase = 2;
    mTimer = 0;
}

// @symbol _ZN12dScMg3DEsp_c4DealEv
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

// @symbol _ZN12dScMg3DEsp_c10ResetRoundEv
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

extern "C" unsigned int sEsp3DResultSfx[];

// @symbol _ZN12dScMg3DEsp_c7ResultsEv
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
                    /* The ROM loads the pointer again here; a plain read
                       reuses the earlier load. */
                    dScMgBase_c *g = *(dScMgBase_c * volatile *)&gMgScene;
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

// @symbol _ZN12dScMg3DEsp_c4PlayEv
void dScMg3DEsp_c::Play()
{
    (this->*s3DEspPhaseTable[mPhase])();
    UpdateFlash();
}

// @symbol _ZN12dScMg3DEsp_c4WaitEv
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
// @symbol _ZN12dScMg3DEsp_c11OnNewRecordEv
void dScMg3DEsp_c::OnNewRecord()
{
    FreeGfxSlotsById(8);
}

/* Vtable slot 18. The name is a placeholder borrowed from dActor_c by slot
   index; see dScMgBase_c.h. The signature repeats the base declaration, or
   mwcc appends a slot instead of overriding. */
// @symbol _ZN12dScMg3DEsp_c10StartRoundEi
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
// @symbol _ZN12dScMg3DEsp_c16CleanupResourcesEv
s32 dScMg3DEsp_c::CleanupResources()
{
    gEspIrieModelFile.Release();
    gIrieBackModelFile.Release();
    return 1;
}

/* Vtable slot 9. Calls four helpers, resets the camera, then draws the
   first model, the dMg3DEspModel_c block and the second model. */
// @symbol _ZN12dScMg3DEsp_c6RenderEv
s32 dScMg3DEsp_c::Render()
{
    DrawBanner();
    RenderCards();
    RenderRows();
    RenderSlots();
    mCameraEyeX = 0;
    mCameraEyeY = 0xd0000;
    mCameraEyeZ = 0x40000;
    mCameraTargetX = 0xffed3000;
    mCameraTargetY = 0xe0000;
    mCameraTargetZ = 0x40000;
    mCameraAngle = 0xc00;
    Camera_UpdateMatrices(pad_4660);
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
// @symbol _ZN12dScMg3DEsp_c8BehaviorEv
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
// @symbol _ZN12dScMg3DEsp_c13InitResourcesEv
s32 dScMg3DEsp_c::InitResources()
{
    Matrix4x3 tmp;
    int f;
    int r5v;
    int r4v;

    data_0209d45c = 0x11;
    data_0209d454 = 0x10;
    G3X::SetFog(false, 0, 2, 0x1000);
    InitialiseVramGlobals();

    /* main BG0CNT: priority 1 */
    *(volatile unsigned short*)0x4000008 = (*(volatile unsigned short*)0x4000008 & ~3) | 1;
    Mg_SetTopScreenYOffset(0x10);

    mCameraEyeX = 0;
    mCameraEyeY = 0xd0000;
    mCameraEyeZ = 0x40000;
    mCameraTargetX = 0xffed3000;
    mCameraTargetY = 0xe0000;
    mCameraTargetZ = 0x40000;
    mCameraAngle = 0xc00;
    Camera_UpdateMatrices(pad_4660); /* the camera block starts at 0x4660 */

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

    data_0209d454 |= 4;
    /* sub BG2CNT, then the sub BG2 scroll offsets */
    *(volatile unsigned short*)0x400100c &= ~3;
    *(volatile unsigned short*)0x400100c &= ~0x40;
    *(volatile unsigned int*)0x4001018 = 0;
    *(volatile unsigned short*)0x400100c = (*(volatile unsigned short*)0x400100c & 0x43) | 0x210;

    f = LoadFile(0x12);
    DecompressLZ16((void *)f, (void *)((unsigned)G2S::GetBG2CharPtr() + 0x4000));
    Deallocate((void*)f);

    f = LoadFile(0x13);
    GXS::LoadBGPltt((const void *)f, 0x1e0, 0x20);
    Deallocate((void*)f);

    f = LoadFile(0x14);
    func_02056374((const void*)f, 0, 0x800);
    Deallocate((void*)f);

    r5v = LoadFile(8);
    r4v = LoadFile(9);
    DecompressLZ16((void*)r5v, (void*)0x6600000);
    GXS::LoadOBJPltt((const void *)r4v, 0, 0x100);
    Deallocate((void*)r5v);
    Deallocate((void*)r4v);

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
// @symbol dScMg3DEsp_c_classInit
extern "C" dScMg3DEsp_c *dScMg3DEsp_c_classInit(void) {
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
