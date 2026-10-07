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
 * with dScMg3DEsp_c's own members, Virtual50 through InitResources.
 * dScMg3DEsp_c_classInit, the scene factory, follows at 0x020ea1f0..0x020ea280
 * and ends the unit. Functions run in ROM order under
 * `#pragma defer_codegen off`; do not reorder.
 *
 * Each pragma bracket below carries the file-global pragma of the file
 * that function came from; the brackets bind only under defer_codegen off.
 *
 * SetFile and SetAnim take Fix12<int> by value in their real signatures;
 * calling them through the headers adds bytes, so their calls stay mangled
 * with a scalar speed.
 *
 * deslop leftovers:
 * - The scene's slot, row and card elements (stride 0x20/0x18 records at
 *   this+0x52xx..0x55xx) are only partially recovered: Row_8a44, Ent_8e10,
 *   P_8e10 and E_968c stand in for their element types, and the handlers
 *   index them through a char* alias of this. Member-array indexing needs
 *   the header layout recovered first and re-verifies differently, so the
 *   raw forms stay.
 * - Obj_777c is a vtable shim so AnimSet::Render can virtual-call the
 *   ModelAnim method that takes a void* argument; naming the real slot
 *   needs the ModelAnim vtable recovered.
 * - mRoundState/mTimer are named; the remaining unk_ fields on the scene
 *   and both helper classes are written-but-unread or have no proven
 *   semantics yet.
 */

#pragma defer_codegen off

#include "common.h"
#include "types.h"
#include "SharedFilePtr.h"
#include "dScMg3DEsp_c.h"
#include "dMg3DEspModel_c.h"
#include "dMg3DEspAnimSet_c.h"
#include "TextureTransformer.h"
#include "TextureSequence.h"
#include "Sound.h"
#include "PlayerInput.h"

struct BMD_File;
struct BTP_File;
struct BTA_File;
struct ModelComponents;

extern "C" {
/* The file handles: word 1 of a SharedFilePtr is its loaded file. */
extern SharedFilePtr data_ov006_02141e54;
extern SharedFilePtr data_ov006_02141e5c;
extern SharedFilePtr data_ov006_02141e64;
extern SharedFilePtr data_ov006_02141e6c;
extern SharedFilePtr data_ov006_02141e74;
extern SharedFilePtr data_ov006_02141e7c;
extern SharedFilePtr data_ov006_02141e84;
extern SharedFilePtr data_ov006_02141e8c;
extern SharedFilePtr data_ov006_02141e94;
extern SharedFilePtr data_ov006_02141e9c;

extern Matrix4x3 data_020a0e68;
void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
int _Z15ApproachLinear2Rsss(short* p, short a, short b);
/* local extern: the call passes an untyped this, so it cannot use the header method. */
void _ZN15dExtFrameCtrl_c7AdvanceEv(void* a);
/* local extern: the call passes an untyped this, so it cannot use the header method. */
int _ZN15dExtFrameCtrl_c8FinishedEv(void* a);
/* local extern: the call passes an untyped this, so it cannot use the header method. */
int _ZNK15dExtFrameCtrl_c12WillHitFrameEi(void* anim, int frame);
int _ZN5Model8LoadFileER13SharedFilePtr(void *p);
/* local extern: the call passes an untyped this, so it cannot use the header method. */
int _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(void *p);
void _ZN15TextureSequence8LoadFileER13SharedFilePtr(void* r);
void _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File(int bmd, void *bma);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(char *t, int f, int a, int b);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *anim, BCA_File *bca, int frame, int speed, u16 flags);
void _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(char *t, void *f, int a, int b, unsigned u);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void* self, void* btp, int a, int b, u16 d);
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void* self, void* bta, int a, int b, u16 d);
void _ZN5Model12SetPolygonIDEi(char *t, int id);
void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int, unsigned int, int, int, int, const void*, void*);
void func_02046208(char* a, int b, int c);
int func_020179b4(void* r0, void* r1, int r2);
/* The last four parameters are unsigned short in the real signature;
   RowFade passes 0x10 - n, which must not be truncated. */
void _ZN3G2x13SetBlendAlphaEPVttttj(volatile unsigned short *p, int a, int b, int c, int d);
s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
int RandomIntInternal(int *seed);
extern int data_0209d4b8;
void FreeGfxSlotsById(int a);
void Camera_UpdateMatrices(void* self);
int func_ov004_020af770(int a0, int a1, int a2, int a3, int a4, int a5, u16 a6);
void func_ov004_020af948(void* a, int b, int c, void* m);
void func_ov004_020b1e34(void *c, int a, int b, int d);
unsigned int func_02012790(unsigned int);
void func_ov004_020adb1c(int);
void func_ov004_020b67e8(int);
void func_ov004_020b0a54(int);
void func_ov004_020b0cac(int, int, int, int, int, short);
void func_ov004_020b04d0(int);
void _ZN3G3X6SetFogEbiii(int, int, int, int);
void InitialiseVramGlobals(void);
void Deallocate(void*);
int LoadFile(int handle);
unsigned _ZN3G2S13GetBG2CharPtrEv(void);
void DecompressLZ16(void *src, void *dst);
void _ZN3GXS10LoadBGPlttEPKvjj(const void* p, unsigned int a, unsigned int b);
void _ZN3GXS11LoadOBJPlttEPKvjj(const void* p, unsigned int a, unsigned int b);
void func_02056374(const void*, u32, u32);

extern int data_ov006_0213c7f4;
extern int data_ov006_0213c744[];
extern dMg3DEspModel_c::State data_ov006_0213c704;
extern dMg3DEspModel_c::State data_ov006_0213c754;
extern dMg3DEspModel_c::State data_ov006_0213c764;
extern dMg3DEspModel_c::State data_ov006_0213c76c;
extern dMg3DEspModel_c::State data_ov006_0213c774;
extern void *data_ov006_0213c844;
extern s16 data_02082214[];
extern unsigned char data_0209d45c;
extern unsigned char data_0209d454;
extern int data_ov004_020bc880;
extern int data_ov004_020bc884;
}

extern Matrix4x3 data_ov006_0213c85c;

/* Both models' starting matrix. Declaring it non-volatile adds 32 bytes to
   dScMg3DEsp_c::InitResources, and copying it without the stack temporary
   removes 32. */
extern volatile Matrix4x3 data_ov006_0213c88c;

/* ---- dScMg3DEsp_c: the destructor (key function) ---------------------- */

/* The members are raw storage (see dScMg3DEsp_c.h), so each is destroyed by
   an explicit call, in reverse declaration order. The vtable stores,
   mSysTracker's destruction and the chain to ~dScMgBase_c are
   compiler-generated; the deleting variant reaches dScMgBase_c's operator
   delete. */
// @symbol _ZN12dScMg3DEsp_cD1Ev
// @symbol _ZN12dScMg3DEsp_cD0Ev
dScMg3DEsp_c::~dScMg3DEsp_c()
{
    ((TextureTransformer *)mTextureTransformer)->TextureTransformer::~TextureTransformer();
    ((dMg3DEspModel_c *)pad_4fd8)->~dMg3DEspModel_c();
    ((Model *)mModel2)->Model::~Model();
    ((Model *)mModel1)->Model::~Model();
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
    r4 = c + 0x12c;
    i = 0;
    zero = (void*)i;
    do {
        if (*(int*)(c + i*4 + 0x168) != 0) {
            *(Matrix4x3*)(r5 + 0x1c) = data_020a0e68;
            ((MaterialChanger*)r4)->Update(*(ModelComponents*)(r5 + 8));
            ((Obj_777c*)r5)->m(zero);
        }
        i++;
        r5 += 0x64;
        r4 += 0x14;
    } while (i < 3);
}

// @symbol _ZN17dMg3DEspAnimSet_c8BehaviorEv
void dMg3DEspAnimSet_c::Behavior()
{
    char *self = (char *)this;
    int sb = 0;
    if (*(short*)(self + 0x178) != 0) {
        if (_Z15ApproachLinear2Rsss((short*)(self + 0x17a), 0, 1)) {
            *(short*)(self + 0x17a) = *(short*)(self + 0x17c);
            sb = 1;
            if (*(short*)(self + 0x178) > 0) {
                *(short*)(int)(self + 0x178) -= 1;
            }
        }
    }
    {
        int zero = 0;
        int i;
        char* a = self;
        char* b = self + 0x12c;
        char* cc = self;
        for (i = 0; i < 3; i++) {
            if (((int*)(self + 0x168))[i] != 0) {
                _ZN15dExtFrameCtrl_c7AdvanceEv(a + 0x50);
                _ZN15dExtFrameCtrl_c7AdvanceEv(b);
                if (_ZN15dExtFrameCtrl_c8FinishedEv(a + 0x50)) {
                    ((int*)(self + 0x168))[i] = 0;
                }
            } else {
                if (sb == 1) {
                    ((int*)(self + 0x168))[i] = 1;
                    *(int*)(a + 0x58) = zero;
                    sb = zero;
                    *(int*)(cc + 0x134) = zero;
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
    *(int *)(o + 0x174) = 0x800;
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov006_02141e94);
    bca = _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(&data_ov006_02141e6c);
    _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File(bmd, &data_ov006_0213c7f4);
    w = o;
    w2 = o + 0x12c;
    for (i = 0; i < 3; i++) {
        _ZN9ModelBase7SetFileEP8BMD_Fileii(w, bmd, 1, m1);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((ModelAnim *)w, (BCA_File *)bca, 0x40000000, *(int *)(o + 0x174), z1);
        _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(w2, &data_ov006_0213c7f4, 0x40000000, *(int *)(o + 0x174), z2);
        _ZN5Model12SetPolygonIDEi(w, (i + 1) & 0xff);
        *(int *)(o + i * 4 + 0x168) = 0;
        w += 0x64;
        w2 += 0x14;
    }
    *(u16 *)(o + 0x178) = 0;
    *(u16 *)(o + 0x17a) = 0;
}

// @symbol _ZN17dMg3DEspAnimSet_cD1Ev
dMg3DEspAnimSet_c::~dMg3DEspAnimSet_c()
{
    data_ov006_02141e94.Release();
    data_ov006_02141e6c.Release();
}

// @symbol _ZN17dMg3DEspAnimSet_cC1Ev
dMg3DEspAnimSet_c::dMg3DEspAnimSet_c() {}

#pragma pop

/* ---- dMg3DEspModel_c --------------------------------------------------- */

// @symbol _ZN15dMg3DEspModel_c14ResetTransformEv
void dMg3DEspModel_c::ResetTransform()
{
    *(Matrix4x3*)((char*)this + 0x28) = data_ov006_0213c85c;
}

// @symbol _ZN15dMg3DEspModel_c6RenderEv
void dMg3DEspModel_c::Render()
{
    char* c = (char*)this;
    int* p = (int*)(((int)c + 0x210));
    int* d = data_ov006_0213c744;
    if (p[0] == d[0]) {
        if (p[1] == d[1]) return;
        if (*(int*)(c + 0x210) == 0) return;
    }

    func_02046208(c + 0x14, *(unsigned char*)(c + 0x21a), 0);

    *(int*)(c + 0x78) = (unsigned)((*(short*)(c + 0x218)) << 0x11) >> 4;
    mTextureSequence.Update(mModelAnim.data);

    ResetTransform();

    mModelAnim.Render(0);

    mAnimSet.Render();
}

// @symbol _ZN15dMg3DEspModel_c8BehaviorEv
void dMg3DEspModel_c::Behavior()
{
    /* mwcc's member-pointer null representation makes the second word
     * irrelevant when the function word is null. Spell that comparison out
     * to preserve the original early-return shape. */
    char* c = (char*)this;
    s32* state = (s32*)((int)c + 0x210);
    s32* idleState = (s32*)&data_ov006_0213c704;
    if (state[0] == idleState[0]) {
        if (state[1] == idleState[1]) return;
        if (*(s32*)(c + 0x210) == 0) return;
    }

    (this->*mState)();

    mModelAnim.Advance();
    mAnimSet.Behavior();

    if (mShowSparks == 0) return;

    mParticleSys = (u32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        mParticleSys, 0xec, 0x48c000, 0x140000, 0x200000, 0, 0);
}

// @symbol _ZN15dMg3DEspModel_c3HitEv
void dMg3DEspModel_c::Hit()
{
    char *thiz = (char*)this;
    if (((dExtFrameCtrl_c*)(thiz + 0x5c))->Finished() != 0 &&
        *(void**)(thiz + 0x6c) == ((void**)&data_ov006_02141e8c)[1]) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, ((BCA_File**)&data_ov006_02141e84)[1], 0, 0x800, 0);
        return;
    }
    if (((dExtFrameCtrl_c*)(thiz + 0x5c))->WillHitFrame(0x10) == 0 &&
        ((dExtFrameCtrl_c*)(thiz + 0x5c))->WillHitFrame(0x50) == 0)
        return;
    if (mMuted != 0)
        return;
    Sound::PlayBank2_2D(0x18f);
}

// @symbol _ZN15dMg3DEspModel_c8StartHitEv
void dMg3DEspModel_c::StartHit()
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(BCA_File**)((char*)&data_ov006_02141e8c + 4), 0x40000000, 0x800, 0);
    if (mMuted == 0)
        Sound::PlayBank2_2D(0x191);
    mState = data_ov006_0213c754;
}

// @symbol _ZN15dMg3DEspModel_c5IntroEv
void dMg3DEspModel_c::Intro()
{
    char *c = (char*)this;
    if (_ZN15dExtFrameCtrl_c8FinishedEv(c + 0x5c)) {
        if (mMuted == 0)
            Sound::PlayBank2_2D(0x18f);
        StartWait();
        return;
    }
    if (_ZNK15dExtFrameCtrl_c12WillHitFrameEi(c + 0x5c, 0x1e) == 0)
        return;
    mAnimSet.SetPeriod(0x1e);
    mAnimSet.SetSpeed(0x800);
    mAnimSet.SetRepeats(-1);
    mShowSparks = 1;
}

// @symbol _ZN15dMg3DEspModel_c10StartIntroEv
void dMg3DEspModel_c::StartIntro()
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(BCA_File**)((char*)&data_ov006_02141e5c + 4), 0x40000000, 0x800, 0);
    mAnimSet.SetPeriod(0xf);
    mAnimSet.SetSpeed(0x1000);
    mAnimSet.SetRepeats(3);
    if (mMuted == 0)
        Sound::PlayBank2_2D(0x18e);
    mState = data_ov006_0213c764;
}

// @symbol _ZN15dMg3DEspModel_c4WaitEv
void dMg3DEspModel_c::Wait()
{
    if (mMuted)
        return;
    if (!mModelAnim.WillHitFrame(0)) {
        if (!mModelAnim.WillHitFrame(0x39))
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
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim,
        ((BCA_File**)&data_ov006_02141e7c)[1], 0, 0x800, 0);
    mState = data_ov006_0213c76c;
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
    mState = data_ov006_0213c774;
}

// @symbol _ZN15dMg3DEspModel_c13InitResourcesEv
int dMg3DEspModel_c::InitResources()
{
    char* c = (char*)this;
    if (func_020179b4(&data_ov006_02141e54, c + 0xc, 1) == 0)
        return 0;
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(&data_ov006_02141e64);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(&data_ov006_02141e5c);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(&data_ov006_02141e7c);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(&data_ov006_02141e8c);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(&data_ov006_02141e84);
    TextureSequence::Prepare(**(BMD_File**)((void**)&data_ov006_02141e54 + 1),
                             **(BTP_File**)((void**)&data_ov006_02141e64 + 1));
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x70, *((void**)&data_ov006_02141e64 + 1), 0, 0x1000, 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((ModelAnim*)(c + 0xc), *((BCA_File**)&data_ov006_02141e5c + 1), 0x40000000, 0x800, 0);
    mAnimSet.InitResources();
    ResetTransform();
    Reset();
    mShowSparks = 0;
    return 1;
}

// @symbol _ZN15dMg3DEspModel_cD1Ev
dMg3DEspModel_c::~dMg3DEspModel_c()
{
    data_ov006_02141e54.Release();
    data_ov006_02141e5c.Release();
    data_ov006_02141e7c.Release();
    data_ov006_02141e8c.Release();
    data_ov006_02141e84.Release();
    data_ov006_02141e64.Release();
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
extern "C" dScMg3DEsp_cState data_ov006_02141f2c[];
extern "C" dScMg3DEsp_cState data_ov006_02141fac[];
extern "C" dScMg3DEsp_cState data_ov006_02141f44[];
extern "C" dScMg3DEsp_cObjState data_ov006_02141f1c[];
extern "C" dScMg3DEsp_cObjState data_ov006_02141f5c[];
extern "C" dScMg3DEsp_cObjState data_ov006_02141f74[];
extern "C" dScMg3DEsp_cObjState data_ov006_02141f8c[];

// @symbol _ZN12dScMg3DEsp_c10DrawBannerEv
void dScMg3DEsp_c::DrawBanner()
{
    char *c = (char*)this;
    if (*(unsigned char *)(c + 0x5555) != 0)
        func_ov004_020b1e34(c, 0xe0, 0x14, 1);
}

/* Early-out if the byte flag at mFlashStep is set; otherwise set
   the embedded model's mPolygonID to 0x1e and step mFlashStep. The check and
   store share the self+0x5000 base (add r2); the laundered increment
   pool-loads the 0x5553 offset. */
// @symbol _ZN12dScMg3DEsp_c10FlashQuickEv
void dScMg3DEsp_c::FlashQuick()
{
    char *self = (char*)this;
    if (*(unsigned char *)(self + 0x5553)) return;
    *(unsigned char *)(self + 0x51f2) = 0x1e;
    *(unsigned char *)(self + 0x5553) += 1;
}

// @symbol _ZN12dScMg3DEsp_c9FlashSyncEv
void dScMg3DEsp_c::FlashSync()
{
    char* c = (char*)this;
    u8 state = *(u8*)(c + 0x5553);
    if (state == 0) {
        u8* p = (u8*)(c + 0x5553);
        *(u8*)(c + 0x51f2) = 0x1e;
        (*p)++;
        return;
    }
    if (state != 1)
        return;
    if (_ZNK15dExtFrameCtrl_c12WillHitFrameEi(c + 0x5034, 0)) {
        u8* p = (u8*)(c + 0x5554);
        (*p)++;
    }
    if (*(u8*)(c + 0x5554) < 1)
        return;
    {
        u8* p = (u8*)(c + 0x5553);
        (*p)++;
    }
    *(u8*)(c + 0x51f2) = 0x1f;
}

// @symbol _ZN12dScMg3DEsp_c9FlashOnceEv
void dScMg3DEsp_c::FlashOnce()
{
    unsigned char *c = (unsigned char*)this;
    unsigned char *r2;
    unsigned char *p;
    unsigned char k;

    r2 = c;
    r2 += 0x5000;
    if (r2[0x553] != 0) {
        return;
    }
    p = (unsigned char *)(c + 0x5553);
    *p = (unsigned char)(*p + 1);
    k = 0x1f;
    r2[0x1f2] = k;
}

// @symbol _ZN12dScMg3DEsp_c11UpdateFlashEv
void dScMg3DEsp_c::UpdateFlash()
{
    char* c = (char*)this;
    unsigned char idx = *(unsigned char*)(c + 0x5000 + 0x552);
    if (idx >= 3) return;
    (this->*data_ov006_02141f44[idx])();
}

extern void* data_ov006_02133a3c[];

typedef struct {
    int x;                  /* 0x00 */
    int y;                  /* 0x04 */
    char _0[0x13];          /* 0x08..0x1a */
    unsigned char type;     /* 0x1b */
    unsigned char enable;   /* 0x1c */
    char _1[3];             /* -> size 0x20 */
} Slot_8354;

typedef struct {
    char _0[0x52bc];
    Slot_8354 slots[20];
} Outer_8354;

// @symbol _ZN12dScMg3DEsp_c11RenderSlotsEv
void dScMg3DEsp_c::RenderSlots()
{
    Outer_8354* a = (Outer_8354*)this;
    int i;
    for (i = 0; i < 20; i++) {
        if (a->slots[i].enable) {
            func_ov004_020af948(data_ov006_02133a3c[a->slots[i].type],
                                a->slots[i].x >> 12, a->slots[i].y >> 12, 0);
        }
    }
}

// @symbol _ZN12dScMg3DEsp_c10SlotDampenEi
void dScMg3DEsp_c::SlotDampen(int i)
{
  char *c = (char*)this;
  unsigned long new_var;
  char *b = c + (i * 32);
  char *q = c + 0x52bc;
  new_var = i;
  *((int *) (q + (new_var * 32))) = (*((int *) (q + (new_var * 32)))) + (*((int *) (b + 0x52c4)));
  *((int *) ((c + 0x52c0) + (new_var * 32))) = (*((int *) ((c + 0x52c0) + (new_var * 32)))) + (*((int *) (b + 0x52c8)));
  if ((*((int *) (b + 0x52c4))) > 0)
  {
    unsigned char e = *((unsigned char *) (b + 0x52d5));
    *((int *) ((c + 0x52c4) + (new_var * 32))) = (*((int *) ((c + 0x52c4) + (new_var * 32)))) - ((e * 16) + 0x10);
    if (((short) (*((int *) (b + 0x52c4)))) < 0)
    {
      *((int *) (b + 0x52c4)) = 0;
    }
  }
  else
    if ((*((int *) (b + 0x52c4))) < 0)
  {
    unsigned char e = *((unsigned char *) (b + 0x52d5));
    *((int *) ((c + 0x52c4) + (new_var * 32))) = (*((int *) ((c + 0x52c4) + (new_var * 32)))) + ((e * 16) + 0x10);
    if (((short) (*((int *) (b + 0x52c4)))) > 0)
    {
      *((int *) (b + 0x52c4)) = 0;
    }
  }
  else
  {
    *((unsigned char *) (b + 0x52da)) = 0;
  }
}

#pragma push
#pragma opt_common_subs off
#pragma opt_strength_reduction off
// @symbol _ZN12dScMg3DEsp_c8SlotKickEi
void dScMg3DEsp_c::SlotKick(int i)
{
    char *c = (char*)this;
    *(int *)(c + 0x52bc + i * 32) += *(int *)(c + i * 32 + 0x52c4);
    *(int *)(c + 0x52c0 + i * 32) += *(int *)(c + i * 32 + 0x52c8);
    if (*(unsigned short *)(c + i * 32 + 0x52ce) != 0) {
        *(unsigned short *)(c + 0x52ce + i * 32) -= 1;
        if (*(short *)(c + i * 32 + 0x52ce) < 0)
            *(short *)(c + i * 32 + 0x52ce) = 0;
        return;
    }
    *(int *)(c + 0x52c4 + i * 32) -= (*(unsigned char *)(c + i * 32 + 0x52d5) << 4) + 0x10;
    if (*(unsigned short *)(c + i * 32 + 0x52d0) != 0) {
        *(unsigned short *)(c + 0x52d0 + i * 32) -= 1;
        if (*(short *)(c + i * 32 + 0x52d0) < 0)
            *(short *)(c + i * 32 + 0x52d0) = 0;
        return;
    }
    {
        int rnd = (int)((((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff);
        /* ROM uses lsr (logical), not asr: keep unsigned through the shifts */
        rnd = (int)((((unsigned int)rnd) << 4) >> 0xf);
        rnd = rnd + 0x10;
        *(short *)(c + i * 32 + 0x52ce) = (short)(rnd & 0xff);
    }
    *(unsigned char *)(c + i * 32 + 0x52da) = 3;
}
#pragma pop

#pragma push
#pragma opt_common_subs off
// @symbol _ZN12dScMg3DEsp_c8SlotRiseEi
void dScMg3DEsp_c::SlotRise(int i)
{
    char *c = (char*)this;
    *(int*)(c + 0x52bc + i * 0x20) += *(int*)(c + i * 0x20 + 0x5000 + 0x2c4);
    *(int*)(c + 0x52c0 + i * 0x20) += *(int*)(c + i * 0x20 + 0x5000 + 0x2c8);

    if (*(unsigned short*)(c + i * 0x20 + 0x5200 + 0xce) != 0)
    {
        *(unsigned short*)(c + 0x52ce + i * 0x20) -= 1;
        {
            short s = *(short*)(c + i * 0x20 + 0x5200 + 0xce);
            if (s < 0)
            {
                s = 0;
                *(short*)(c + i * 0x20 + 0x5200 + 0xce) = s;
            }
        }
        return;
    }

    {
        unsigned char b = *(unsigned char*)(c + i * 0x20 + 0x5000 + 0x2d5);
        *(int*)(c + 0x52c4 + i * 0x20) += (b << 4) + 0x10;
    }

    if (*(unsigned short*)(c + i * 0x20 + 0x5200 + 0xd0) != 0)
    {
        *(unsigned short*)(c + 0x52d0 + i * 0x20) -= 1;
        {
            short s = *(short*)(c + i * 0x20 + 0x5200 + 0xd0);
            if (s < 0)
            {
                s = 0;
                *(short*)(c + i * 0x20 + 0x5200 + 0xd0) = s;
            }
        }
        return;
    }

    {
        unsigned r = (unsigned)RandomIntInternal(&data_0209d4b8);
        unsigned t = (r >> 16) & 0x7fff;
        t = (t << 4) >> 15;
        *(unsigned short*)(c + i * 0x20 + 0x5200 + 0xce) = (unsigned char)(t + 0x10);
        *(unsigned char*)(c + i * 0x20 + 0x5000 + 0x2da) = 3;
    }
}
#pragma pop

extern unsigned char data_ov006_0212e57c[];

#pragma push
#pragma opt_common_subs off
// @symbol _ZN12dScMg3DEsp_c9SlotSetupEi
void dScMg3DEsp_c::SlotSetup(int idx)
{
    char *c = (char*)this;
    unsigned int r;
    *(int *)(c + (idx << 5) + 0x52c4) = 0;
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(unsigned char *)(c + (idx << 5) + 0x52d5) = (unsigned char)((r << 3) >> 15);
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(unsigned char *)(c + (idx << 5) + 0x52d6) = (unsigned char)((r << 3) >> 15);
    *(unsigned char *)(c + (idx << 5) + 0x52d8) = 1;
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(unsigned short *)(c + (idx << 5) + 0x52ce) = (unsigned char)(((r << 4) >> 15) + 8);
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(unsigned short *)(c + (idx << 5) + 0x52d0) = (unsigned char)(((r << 5) >> 15) + 0x10);
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(unsigned char *)(c + (idx << 5) + 0x52da) = data_ov006_0212e57c[(r << 1) >> 15];
}
#pragma pop

#pragma push
#pragma opt_common_subs off
// @symbol _ZN12dScMg3DEsp_c8SlotFallEi
void dScMg3DEsp_c::SlotFall(int idx)
{
  char* base = (char*)this;
  unsigned char b = *(unsigned char*)(base + idx*0x20 + 0x52da);
  (this->*data_ov006_02141f8c[b])(idx);

  unsigned char c = *(unsigned char*)(base + idx*0x20 + 0x52d6);
  *(int*)(base + 0x52c8 + idx*0x20) -= (c<<3) + 0x10;
  int sh = *(unsigned short*)(base + idx*0x20 + 0x52cc) >> 3;
  if(sh >= 7) sh = 7;
  *(unsigned char*)(base + idx*0x20 + 0x52d7) = 7 - sh;
  if(*(unsigned short*)(base + idx*0x20 + 0x52cc) != 0){
    *(unsigned short*)(base + 0x52cc + idx*0x20) -= 1;
    if(*(short*)(base + idx*0x20 + 0x52cc) < 0) *(short*)(base + idx*0x20 + 0x52cc) = 0;
  }else{
    *(unsigned char*)(base + idx*0x20 + 0x52d9) = 0;
  }
}
#pragma pop

#pragma push
#pragma opt_common_subs off
// @symbol _ZN12dScMg3DEsp_c8SlotWaitEi
void dScMg3DEsp_c::SlotWait(int idx)
{
    char *o = (char*)this;
    if (*(u16 *)(o + idx * 0x20 + 0x52d2) != 0) {
        *(u16 *)((char *)(((int)o + 0x52d2)) + idx * 0x20) -= 1;
        if (*(short *)(o + idx * 0x20 + 0x52d2) < 0)
            *(u16 *)(o + idx * 0x20 + 0x52d2) = 0;
    } else {
        int rnd = (int)((((u32)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff);
        rnd = (int)((((u32)rnd) << 5) >> 15);
        *(u16 *)(o + idx * 0x20 + 0x52cc) = rnd + 0x40;
        *(u8 *)(o + idx * 0x20 + 0x52d9) = 1;
        *(u8 *)(o + idx * 0x20 + 0x52da) = 0;
        *(u8 *)(o + idx * 0x20 + 0x52d8) = 0;
        rnd = (int)((((u32)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff);
        *(int *)(o + idx * 0x20 + 0x52bc) = (int)((((u32)rnd) << 5) >> 15) << 15;
        rnd = (int)((((u32)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff);
        *(int *)(o + idx * 0x20 + 0x52c0) = ((int)((((u32)rnd) << 4) >> 15) << 3) + 0x40 << 12;
        *(int *)(o + idx * 0x20 + 0x52c8) = -0x200;
        *(u8 *)(o + idx * 0x20 + 0x52d7) = 0;
    }
}
#pragma pop

struct Row_8a44 { u8 d[0x20]; };

// @symbol _ZN12dScMg3DEsp_c11UpdateSlotsEv
void dScMg3DEsp_c::UpdateSlots()
{
    Row_8a44 *rows = (Row_8a44 *)this;
    for (int i = 0; i < 0x14; i++) {
        if (rows[i].d[0x52d4]) {
            u8 k = rows[i].d[0x52d9];
            (this->*data_ov006_02141f1c[k])(i);
        }
    }
}

#pragma push
#pragma opt_strength_reduction off
// @symbol _ZN12dScMg3DEsp_c10ResetSlotsEv
void dScMg3DEsp_c::ResetSlots()
{
  char *c = (char*)this;
  int i;
  char *new_var;
  for (i = 0; i < 0x14; i++)
  {
    *((unsigned char *) ((c + (i * 0x20)) + 0x52d4)) = 0;
    *((unsigned char *) ((c + (i * 0x20)) + 0x52d8)) = 0;
  }

  if (1)
  {
  }
  for (i = 0; i < 0x14; i++)
  {
    *((unsigned char *) ((c + ((i * 0x20) & 0xFFFFFFFFFFFFFFFFu)) + 0x52d4)) = 1;
    *((unsigned char *) ((c + (i * 0x20)) + 0x52d9)) = 0;
    *((unsigned char *) ((c + (i * 0x20)) + 0x52da)) = 0;
    *((short *) ((c + (i * 0x20)) + 0x52cc)) = 0;
    new_var = (c + (i * 0x20)) + 0x52d2;
    *((short *) new_var) = ((i & 0xf) << 3) << 2;
  }

}
#pragma pop

extern int data_ov006_02136e38;

inline char *inline_fn_8b18(int *arg0)
{
  return (char *) arg0;
}

// @symbol _ZN12dScMg3DEsp_c10RenderRowsEv
void dScMg3DEsp_c::RenderRows()
{
  char *thiz = (char*)this;
  int *g;
  int i;
  for (i = 0; i < 3; i++)
  {
    if ((*((unsigned char *) ((thiz + 0x5000) + 0x292))) != 0)
    {
      int *o = (int *) func_ov004_020af770(*(&data_ov006_02136e38), (*((int *) ((thiz + 0x5000) + 0x280))) >> 12, (*((int *) ((thiz + 0x5000) + 0x284))) >> 12, -1, *((unsigned char *) ((thiz + 0x5000) + 0x293)), *((int *) ((thiz + 0x5000) + 0x288)), 0);
      if ((*((unsigned char *) ((thiz + 0x5000) + 0x291))) == 2)
      {
        int w0 = o[0];
        int w1 = o[1];
        o[0] = (w0 & (~0xc00)) | 0x400;
        *((unsigned short *) (((char *) o) + 4)) = ((*((unsigned short *) (inline_fn_8b18(o) + 4))) & (~0xf000)) | (((((unsigned) w1) << 0x10) >> 0x1c) << 12);
      }
    }
    thiz += 0x14;
  }

}

// @symbol _ZN12dScMg3DEsp_c7RowFadeEi
void dScMg3DEsp_c::RowFade(int i)
{
    char *c = (char*)this;
    int off = i * 0x14;
    char *b = c + 0x5288;
    int x = *(int *)(b + off);
    x = x - (x >> 7);
    *(int *)(b + off) = x;
    *(unsigned char *)(((int)c + 0x554e)) += 1;
    {
        unsigned char ip = *(unsigned char *)(c + 0x554e);
        _ZN3G2x13SetBlendAlphaEPVttttj(
            (volatile unsigned short *)0x4001050, 0, 4,
            0x10 - ip, ip);
    }
    if (*(unsigned char *)(c + 0x554e) < 0x10)
        return;
    *(unsigned char *)(c + 0x554e) = 0;
    *(unsigned char *)(c + off + 0x5292) = 0;
    *(unsigned char *)(c + off + 0x5290) = 0;
}

// @symbol _ZN12dScMg3DEsp_c8RowDecayEi
void dScMg3DEsp_c::RowDecay(int i)
{
  char* c = (char*)this;
  int off = i * 0x14;
  char* b = c + 0x5288;
  int x = *(int*)(b + off);
  x = x - (x >> 6);
  *(int*)(b + off) = x;
  if(*(int*)(b + off) <= 0x900){
    *(unsigned char*)(c + off + 0x5000 + 0x291) = 2;
  }
}

// @symbol _ZN12dScMg3DEsp_c7RowWaitEi
void dScMg3DEsp_c::RowWait(int i)
{
  char* c = (char*)this;
  int idx = i*0x14;
  char* r2 = c + 0x528c;
  unsigned short* p = (unsigned short*)(r2 + idx);
  if(*p != 0){
    *p = *p - 1;
    if(*(short*)p < 0) *(short*)p = 0;
    return;
  }
  char* r0 = c + idx;
  *(char*)(r0+0x5000+0x292)=1;
  *(char*)(r0+0x5000+0x291)=1;
}

// @symbol _ZN12dScMg3DEsp_c10UpdateRowsEv
void dScMg3DEsp_c::UpdateRows()
{
    int i;
    char* s = (char*)this;
    for (i = 0; i < 3; i++) {
        if (*(unsigned char*)(s + 0x5290)) {
            (this->*data_ov006_02141f74[*(unsigned char*)(s + 0x5291)])(i);
        }
        s += 0x14;
    }
}

extern unsigned short data_ov006_0212e58c[];

// @symbol _ZN12dScMg3DEsp_c9SpawnRowsEi
void dScMg3DEsp_c::SpawnRows(int idx)
{
    char *c = (char*)this;
    int i;
    char *d = c;
    char *e = c + idx * 0x18;
    char *src1 = e + 0x5208;
    char *src2 = e + 0x520c;
    for (i = 0; i < 3; i++) {
        *(unsigned char *)(d + 0x5290) = 1;
        *(int *)(d + 0x5280) = *(int *)src1;
        *(int *)(d + 0x5284) = *(int *)src2;
        *(unsigned char *)(d + 0x5292) = 0;
        *(unsigned char *)(d + 0x5291) = 0;
        *(short *)(d + 0x528c) = data_ov006_0212e58c[i];
        *(unsigned char *)(d + 0x5293) = 0;
        *(int *)(d + 0x5288) = 0x1400;
        d += 0x14;
    }
}

struct Ent_8e10 { u16 f0, f2, f4, f6; };
extern Ent_8e10 *data_ov006_02133f24[];

struct P_8e10 {
  char pad[0x208];
  int i208;
  int i20c;
  char pad2[0x219 - 0x210];
  u8 b219;
  u8 b21a;
  u8 b21b;
  u8 b21c;
  char pad3[0x54e - 0x21d];
  u8 b54e;
};

#define PP(base) ((P_8e10 *)((base) + 0x5000))

// @symbol _ZN12dScMg3DEsp_c11RenderCardsEv
void dScMg3DEsp_c::RenderCards()
{
  char *c0 = (char*)this;
  int i;
  int x;
  int y;
  int c;
  Ent_8e10 *r7;
  char *c2;
  u8 b;
  c2 = c0;
  for (i = 0; i < 5; i++) {
    if (PP(c2)->b21a != 0) {
      b = PP(c2)->b21b;
      x = PP(c2)->i208 >> 0xc;
      y = PP(c2)->i20c >> 0xc;
      c = PP(c2)->b21c;
      r7 = data_ov006_02133f24[b];
      for (;;) {
        int *r0 = (int *)func_ov004_020af770(
            (int)r7, x, y, -1, c, 0x1000, 0);
        if (PP(c0)->b54e && !PP(c2)->b219) {
          int v = r0[0];
          int w = r0[1];
          int bits = (int)((unsigned int)(w << 0x10) >> 0x1c);
          r0[0] = (v & ~0xc00) | 0x400;
          *(u16 *)((char *)r0 + 4) = (u16)((*(u16 *)((char *)r0 + 4) & ~0xf000) | (bits << 12));
        }
        if (r7->f6 == 0xffff) break;
        r7++;
      }
    }
    c2 += 0x18;
  }
}

#undef PP

// @symbol _ZN12dScMg3DEsp_c8CardSeekEi
void dScMg3DEsp_c::CardSeek(int i)
{
    char *self = (char*)this;
    int n;
    u16 timer;
    s16 tv1;
    s16 tv2;
    int speed2;
    int speed1;
    int angle1;
    char *angleBase;
    char *speedBase;

    n = i * 0x18;
    timer = *(u16 *) (self + 0x5214 + n);
    if (timer != 0)
    {
        *(u16 *) (self + 0x5214 + n) = timer - 1;
        if ((*(s16 *) (self + 0x5214 + n)) <= 0)
        {
            *(u16 *) (self + 0x5214 + n) = 0;
        }
        return;
    }

    speedBase = self + 0x5210;
    angleBase = self + 0x5216;
    speed1 = *(int *) (speedBase + n);
    angle1 = *(u16 *) (angleBase + n);
    angle1 >>= 4;
    tv1 = data_02082214[(angle1 << 1) + 1];
    *(int *) (self + 0x5208 + n) = (*(int *) (self + 0x5208 + n)) + ((int) ((((s64) tv1) * speed1 + 0x800) >> 0xc));

    tv2 = data_02082214[(*(u16 *) (angleBase + n) >> 4) << 1];
    speed2 = *(int *) (speedBase + n);
    *(int *) (self + 0x520c + n) = (*(int *) (self + 0x520c + n)) + ((int) ((((s64) tv2) * speed2 + 0x800) >> 0xc));
    *(int *) (speedBase + n) = (*(int *) (speedBase + n)) + 0x300;

    {
        int dx = 0x80 - ((*(int *) (self + 0x5208 + n)) >> 0xc);
        int dz = 0x30 - ((*(int *) (self + 0x520c + n)) >> 0xc);
        if (dx < -6)
            return;
        if (dx > 6)
            return;
        if (dz < -6)
            return;
        if (dz > 6)
            return;
    }

    *(int *) (self + 0x5208 + n) = 0x80000;
    *(int *) (self + 0x520c + n) = 0x30000;
    *(u8 *) (self + 0x5218 + n) = 0;

    if (*(u8 *) (self + 0x5552) == 0)
    {
        if ((*(u8 *) (self + 0x5551)) == (*(u8 *) (self + 0x521b + n)))
        {
            *(u8 *) (self + 0x5550) = 1;
        }
        else
        {
            int rnd = RandomIntInternal(&data_0209d4b8);
            u32 temp = ((u32) rnd >> 16) & 0x7fff;
            if (((temp << 1) >> 0xf) == 0)
            {
                *(u8 *) (self + 0x5550) = 0;
            }
            else
            {
                *(u8 *) (self + 0x5551) = *(u8 *) (self + 0x521b + n);
                *(u16 *) (self + 0x51f0) = *(u8 *) (self + 0x521b + n);
                *(u8 *) (self + 0x5550) = 1;
            }
        }
    }
    else
    {
        if ((*(u8 *) (self + 0x5551)) == (*(u8 *) (self + 0x521b + n)))
        {
            *(u8 *) (self + 0x5550) = 1;
        }
        else
        {
            *(u8 *) (self + 0x5550) = 0;
        }
    }

    mRoundState = 2;
    mPhase = 0;
    mTimer = 0x90;
    if (*(u8 *) (self + 0x5550) == 0)
    {
        u16 *p548 = (u16 *) (self + 0x5548);
        *p548 = *p548 + 0x40;
    }

    ((dMg3DEspModel_c*)pad_4fd8)->StartHit();
}

// @symbol _ZN12dScMg3DEsp_c9CardTouchEi
void dScMg3DEsp_c::CardTouch(int idx)
{
    char* self = (char*)this;
    unsigned int i = gActivePlayerSlot;
    int ok = 0;
    int n;
    int v, w;

    if (gTouchHeld[i * 4] != 0) {
        if (gTouchEdge[i * 4] != 0) {
            ok = 1;
        }
    }
    if (ok == 0) return;

    n = idx * 0x18;
    v = gTouchX[gActivePlayerSlot * 4] - (*(int*)(self + 0x5208 + n) >> 12);
    w = gTouchY[gActivePlayerSlot * 4] - (*(int*)(self + 0x520c + n) >> 12);

    if (v < -0xa) return;
    if (v > 0xa) return;
    if (w < -0x18) return;
    if (w > 0x18) return;

    {
        u8* pc = (u8*)(self + 0x554f);
        *pc = *pc + 1;
    }
    *(s16*)(self + 0x5214 + n) = 0x20;
    *(int*)(self + 0x5210 + n) = 0;
    *(u8*)(self + 0x5219 + n) = 2;

    *(s16*)(self + 0x5216 + n) = _ZN4cstd5atan2E5Fix12IiES1_(
        0x30 - (*(int*)(self + 0x520c + n) >> 12),
        0x80 - (*(int*)(self + 0x5208 + n) >> 12));

    SpawnRows(idx);
    Sound::PlayBank2_2D(0x190);
}

// @symbol _ZN12dScMg3DEsp_c8CardExitEi
void dScMg3DEsp_c::CardExit(int i)
{
    char* c = (char*)this;
    unsigned char st = *(unsigned char*)(c + 0x554e);
    if (st == 1) {
        unsigned char* p = (unsigned char*)(c + 0x521c + i * 0x18);
        if (*p == 1) *p = 0;
    }
    if (*(unsigned char*)(c + 0x554e) >= 0x10) {
        *(unsigned char*)(c + i * 0x18 + 0x5219) = 1;
    }
}

// @symbol _ZN12dScMg3DEsp_c11UpdateCardsEv
void dScMg3DEsp_c::UpdateCards()
{
    int i;
    char* s = (char*)this;
    for (i = 0; i < 5; i++) {
        if (*(unsigned char*)(s + 0x5218)) {
            (this->*data_ov006_02141f5c[*(unsigned char*)(s + 0x5219)])(i);
        }
        s += 0x18;
    }
}

extern unsigned char data_ov006_0212e588[];
extern int volatile data_ov006_0213c818[];

// @symbol _ZN12dScMg3DEsp_c9DealCardsEv
void dScMg3DEsp_c::DealCards()
{
    char *c = (char*)this;
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
    char *p;

    n = data_ov006_0212e588[*(int *)(c + 0x5544)];
    *(unsigned char *)(c + 0x5551) =
        (unsigned char)(((((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff) * 5 >> 15);
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
    v = *(unsigned char *)(c + 0x5551);
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
        p = c;
        do {
            *(int *)(p + 0x5208) = ((unsigned short *)data_ov006_0213c818[n - 2])[o] << 12;
            *(int *)(p + 0x520c) = ((unsigned short *)data_ov006_0213c818[n - 2])[l * 2 + 1] << 12;
            *(unsigned char *)(p + 0x5218) = 1;
            *(unsigned char *)(p + 0x521a) = 1;
            *(unsigned char *)(p + 0x521c) = 1;
            *(unsigned char *)(p + 0x521b) = (unsigned char)a[l];
            *(unsigned char *)(p + 0x5219) = 0;
            o += 2;
            p += 0x18;
            l++;
        } while (l < n);
    }
    *(short *)(c + 0x51f0) = *(unsigned char *)(c + 0x5551);
}

// @symbol _ZN12dScMg3DEsp_c10ClearCardsEv
void dScMg3DEsp_c::ClearCards()
{
  char* c = (char*)this;
  int i;
  char* p = c;
  for (i = 0; i < 5; i++) {
    *(int*)(p+0x5208) = 0;
    *(int*)(p+0x520c) = 0;
    *(unsigned short*)(p+0x5214) = 0;
    *(unsigned char*)(p+0x5218) = 0;
    *(unsigned char*)(p+0x521a) = 0;
    *(unsigned char*)(p+0x521b) = 0;
    *(unsigned char*)(p+0x521c) = 0;
    p += 0x18;
  }
}

// @symbol _ZN12dScMg3DEsp_c13SetDifficultyEv
void dScMg3DEsp_c::SetDifficulty()
{
    char *c = (char*)this;
    int v = *(int *)(c + 0xbc);
    int mult = 2;
    if (v >= 0xf) {
        *(int *)(c + 0x5544) = 3;
    } else if (v >= 0xa) {
        *(int *)(c + 0x5544) = 2;
    } else if (v >= 5) {
        *(int *)(c + 0x5544) = 1;
    } else {
        *(int *)(c + 0x5544) = 0;
        mult = 3;
    }
    *(unsigned char *)(c + 0x5552) =
        (unsigned char)((mult * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf);
}

// @symbol _ZN12dScMg3DEsp_c7ResolveEv
void dScMg3DEsp_c::Resolve()
{
    UpdateRows();
    UpdateCards();
}

struct E_968c { char pad[0x18]; };

// @symbol _ZN12dScMg3DEsp_c5AwaitEv
void dScMg3DEsp_c::Await()
{
    char *c = (char*)this;
    int i;
    struct E_968c *arr;
    UpdateCards();
    if (*(unsigned char *)(c + 0x554f) == 0) return;
    *(int *)(c + 0x5540) = 3;
    arr = (struct E_968c *)c;
    for (i = 0; i < 5; i++) {
        char *base = (char *)&arr[i];
        if (*(unsigned char *)(base + 0x5218) != 0) {
            if (*(unsigned char *)(base + 0x5219) != 2) {
                *(unsigned char *)(base + 0x5218) = 0;
                *(unsigned char *)(base + 0x521a) = 0;
            }
        }
    }
}

// @symbol _ZN12dScMg3DEsp_c4FadeEv
void dScMg3DEsp_c::Fade()
{
    char *thiz = (char*)this;
    *(unsigned short*)(((int)thiz + 0x554a)) += 1;
    if (*(unsigned short*)(thiz + 0x554a) >= 4) {
        *(unsigned short*)(thiz + 0x554a) = 0;
        *(unsigned char*)(((int)thiz + 0x554e)) += 1;
        _ZN3G2x13SetBlendAlphaEPVttttj(
            (volatile unsigned short*)0x4001050, 0, 4,
            *(unsigned char*)(thiz + 0x554e), 0x10 - *(unsigned char*)(thiz + 0x554e));
    }
    UpdateCards();
    if (*(unsigned char*)(thiz + 0x554e) < 0x10)
        return;
    *(unsigned char*)(thiz + 0x554e) = 0;
    *(unsigned short*)(thiz + 0x554a) = 0;
    *(int*)(thiz + 0x5540) = 2;
    *(unsigned short*)(thiz + 0x5548) = 0;
}

// @symbol _ZN12dScMg3DEsp_c4DealEv
void dScMg3DEsp_c::Deal()
{
    char *c = (char*)this;
    if (*(unsigned short*)(c + 0x5548) != 0) {
        unsigned short *d = (unsigned short*)((int)(c + 0x5548));
        *d = *d - 1;
        if (*(short*)(c + 0x5548) < 0)
            *(short*)(c + 0x5548) = 0;
        return;
    }
    SetDifficulty();
    ((dMg3DEspModel_c*)pad_4fd8)->StartIntro();
    DealCards();
    *(int*)(c + 0x5540) = 1;
    FreeGfxSlotsById(0x1d);
    if (*(unsigned char*)(c + 0xc4) == 0) {
        *(unsigned char*)(c + 0xc3) = 1;
        *(unsigned char*)(c + 0xc4) = 1;
        *(short*)(c + 0xc0) = 0;
    }
}

// @symbol _ZN12dScMg3DEsp_c10ResetRoundEv
void dScMg3DEsp_c::ResetRound()
{
    char *c = (char*)this;
    ClearCards();
    *(short*)(c + 0x5548) = 0;
    *(short*)(c + 0x554a) = 0;
    *(unsigned char*)(c + 0x554e) = 0;
    *(unsigned char*)(c + 0x554f) = 0;
    *(unsigned char*)(c + 0x5550) = 0;
    *(unsigned char*)(c + 0x5552) = 0xff;
    *(unsigned char*)(c + 0x5553) = 0;
    *(short*)(c + 0x554c) = 0;
    *(unsigned char*)(c + 0x5554) = 0;
    *(unsigned char*)(c + 0x5555) = 1;
}

extern "C" unsigned int data_ov006_0212e594[];

// @symbol _ZN12dScMg3DEsp_c7ResultsEv
void dScMg3DEsp_c::Results()
{
    char *c = (char *)this;

    UpdateRows();
    if (*(u16 *)(c + 0x5548) != 0) {
        (*(u16 *)(int)(((long long)(int)(c + 0x5548))))--;
        if (*(u16 *)(c + 0x5548) == 0x40) {
            if (*(u8 *)(c + 0x5550) != 0) {
                int idx = data_ov004_020beb68 != 0 ? *(int *)((char *)data_ov004_020beb68 + 0xb4) : 0;
                while (idx >= 5)
                    idx -= 5;
                if (data_ov004_020beb68 != 0) {
                    /* The ROM loads the pointer again here. The retired
                       file declared it char * and got that load for free;
                       dScMgBase_c.h declares it void *, and every non-
                       volatile spelling of that reuses the earlier load. */
                    char *g = *(char * volatile *)&data_ov004_020beb68;
                    if (*(int *)(g + 0xb4) < 9999)
                        (*(int *)(int)(((long long)(int)(g + 0xb4))))++;
                    if (*(int *)(g + 0xb4) > *(int *)(g + 0xb8))
                        *(int *)(g + 0xb8) = *(int *)(g + 0xb4);
                }
                Sound::PlayBank2_2D(data_ov006_0212e594[idx]);
            } else {
                func_02012790(0xe);
            }
        }
        if (*(s16 *)(c + 0x5548) > 0)
            return;
        if (*(u8 *)(c + 0x5550) != 0) {
            int rem, quot;
            func_ov004_020adb1c(data_ov004_020beb68 != 0 ? *(int *)((char *)data_ov004_020beb68 + 0xb4) : 0);
            rem = data_ov004_020beb68 != 0 ? *(int *)((char *)data_ov004_020beb68 + 0xb4) : 0;
            quot = 0;
            while (rem >= 5) {
                rem -= 5;
                quot++;
            }
            if (rem == 0 && quot != 0) {
                if (quot <= 2)
                    func_ov004_020b67e8(0x14);
                else
                    func_ov004_020b67e8(0x15);
                func_ov004_020b0a54(0);
            } else {
                (*(int *)(int)(((long long)(int)(c + 0xbc))))++;
                if (*(u32 *)(c + 0xbc) > 0x270e)
                    *(u32 *)(c + 0xbc) = 0x270e;
                OnYoshiTryEat(-1);
            }
        } else {
            *(u16 *)(c + 0x554c) = 0;
            *(u8 *)(c + 0x5555) = 0;
            func_ov004_020b0cac(8, 0x80, 0xa0, 1, -1, 0xd);
            func_ov004_020b0a54(0x12);
            *(int *)(c + 0x51e4) = 1;
        }
        *(u8 *)(c + 0xc3) = 0;
        *(u16 *)(c + 0x5548) = 0;
    } else {
        int b;
        u8 i;
        if (*(u8 *)(c + 0x5550) != 0)
            return;
        b = 0;
        i = gActivePlayerSlot;
        if (gTouchHeld[i * 4] != 0) {
            if (gTouchEdge[i * 4] != 0)
                b = 1;
        }
        if (b != 0)
            ClearCards();
        (*(u16 *)(int)(((long long)(int)(c + 0x554c))))++;
        if (*(u16 *)(c + 0x554c) == 0xb4)
            ClearCards();
        if (*(u16 *)(c + 0x554c) >= 0xb5)
            *(u16 *)(c + 0x554c) = 0xb5;
    }
}

// @symbol _ZN12dScMg3DEsp_c4PlayEv
void dScMg3DEsp_c::Play()
{
    (this->*data_ov006_02141fac[mPhase])();
    UpdateFlash();
}

// @symbol _ZN12dScMg3DEsp_c4WaitEv
void dScMg3DEsp_c::Wait()
{
    char* c = (char*)this;
    char* r2 = c + 0x5500;
    if (*(unsigned short*)(r2 + 0x48) != 0) {
        unsigned short* p = (unsigned short*)(c + 0x5548);
        *p = *p - 1;
        if (*(short*)(r2 + 0x48) < 0)
            *(unsigned short*)(r2 + 0x48) = 0;
        return;
    }
    *(int*)(c + 0x5000 + 0x53c) = 1;
    *(unsigned short*)(r2 + 0x48) = 0x40;
}

/* ---- dScMg3DEsp_c ------------------------------------------------------ */

/* Vtable slot 20. Virtual50 is a placeholder named for the slot's byte
   offset; dScMgBase_c.h documents the void contract. */
// @symbol _ZN12dScMg3DEsp_c9Virtual50Ev
void dScMg3DEsp_c::Virtual50()
{
    FreeGfxSlotsById(8);
}

/* Vtable slot 18. The name is a placeholder borrowed from dActor_c by slot
   index; see dScMgBase_c.h. The signature repeats the base declaration, or
   mwcc appends a slot instead of overriding. */
// @symbol _ZN12dScMg3DEsp_c13OnYoshiTryEatEi
void dScMg3DEsp_c::OnYoshiTryEat(int a)
{
    ResetRound();
    mRoundState = 0;
    if (a == 0) {
        unk_0bc++;
        if (unk_0bc > 9998) unk_0bc = 9998;
    } else if (a == 0x12) {
        if (data_ov004_020beb68 != 0) ((dScMgBase_c*)data_ov004_020beb68)->mHudScore = 0;
        unk_0bc = 0;
        if (unk_0bc > 9998) unk_0bc = 9998; /* the ROM keeps this clamp; 16 bytes */
    }
    unk_51e4 = 0;
    ((dMg3DEspModel_c*)pad_4fd8)->Reset();
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
}

/* Vtable slot 3. Releases the files of the two models; InitResources loads
   mModel1 from the first handle and mModel2 from the second. */
// @symbol _ZN12dScMg3DEsp_c16CleanupResourcesEv
s32 dScMg3DEsp_c::CleanupResources()
{
    data_ov006_02141e9c.Release();
    data_ov006_02141e74.Release();
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
    /* Model::data, at +8. Naming the member makes mwcc add the 8 in a separate instruction. */
    ((TextureTransformer *)mTextureTransformer)->Update(*(ModelComponents *)(mModel1 + 8));
    ((Model *)mModel1)->Render(0);
    ((dMg3DEspModel_c *)pad_4fd8)->Render();
    ((Model *)mModel2)->Render(0);
    return 1;
}

/* Vtable slot 6. Runs the current state from the member-function table,
   indexed by mRoundState, then steps the slot table, the texture animation
   and the dMg3DEspModel_c block. */
// @symbol _ZN12dScMg3DEsp_c8BehaviorEv
s32 dScMg3DEsp_c::Behavior()
{
    (this->*data_ov006_02141f2c[mRoundState])();
    UpdateSlots();
    ((TextureTransformer *)mTextureTransformer)->Advance();
    ((dMg3DEspModel_c*)pad_4fd8)->Behavior();
    return 1;
}

/* Vtable slot 0. Sets up the camera and both display engines, loads the two
   models, the texture animation and the dMg3DEspModel_c block, then calls
   OnYoshiTryEat(-1). */
// @symbol _ZN12dScMg3DEsp_c13InitResourcesEv
s32 dScMg3DEsp_c::InitResources()
{
    Matrix4x3 tmp;
    int f;
    int r5v;
    int r4v;

    data_0209d45c = 0x11;
    data_0209d454 = 0x10;
    _ZN3G3X6SetFogEbiii(0, 0, 2, 0x1000);
    InitialiseVramGlobals();

    /* main BG0CNT: priority 1 */
    *(volatile unsigned short*)0x4000008 = (*(volatile unsigned short*)0x4000008 & ~3) | 1;
    func_ov004_020b04d0(0x10);

    mCameraEyeX = 0;
    mCameraEyeY = 0xd0000;
    mCameraEyeZ = 0x40000;
    mCameraTargetX = 0xffed3000;
    mCameraTargetY = 0xe0000;
    mCameraTargetZ = 0x40000;
    mCameraAngle = 0xc00;
    Camera_UpdateMatrices(pad_4660); /* the camera block starts at 0x4660 */

    if (func_020179b4(&data_ov006_02141e9c, mModel1, 1) == 0) return 0;

    TextureTransformer::Prepare(**(BMD_File**)((void**)&data_ov006_02141e9c + 1), *(BTA_File*)&data_ov006_0213c844);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(mTextureTransformer, &data_ov006_0213c844, 0, 0x1000, 0);

    if (((dMg3DEspModel_c*)pad_4fd8)->InitResources() == 0) return 0;

    if (func_020179b4(&data_ov006_02141e74, mModel2, 1) == 0) return 0;

    tmp = (Matrix4x3&)data_ov006_0213c88c;
    ((Model*)mModel1)->mat4x3 = (Matrix4x3&)data_ov006_0213c88c;
    ((Model*)mModel2)->mat4x3 = tmp;

    data_0209d454 |= 4;
    /* sub BG2CNT, then the sub BG2 scroll offsets */
    *(volatile unsigned short*)0x400100c &= ~3;
    *(volatile unsigned short*)0x400100c &= ~0x40;
    *(volatile unsigned int*)0x4001018 = 0;
    *(volatile unsigned short*)0x400100c = (*(volatile unsigned short*)0x400100c & 0x43) | 0x210;

    f = LoadFile(0x12);
    DecompressLZ16((void*)f, (void*)(_ZN3G2S13GetBG2CharPtrEv() + 0x4000));
    Deallocate((void*)f);

    f = LoadFile(0x13);
    _ZN3GXS10LoadBGPlttEPKvjj((const void*)f, 0x1e0, 0x20);
    Deallocate((void*)f);

    f = LoadFile(0x14);
    func_02056374((const void*)f, 0, 0x800);
    Deallocate((void*)f);

    r5v = LoadFile(8);
    r4v = LoadFile(9);
    DecompressLZ16((void*)r5v, (void*)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void*)r4v, 0, 0x100);
    Deallocate((void*)r5v);
    Deallocate((void*)r4v);

    func_ov004_020b04d0(0x20);

    OnYoshiTryEat(-1);

    mTimer = 0x40;

    unk_0a8 = 3;
    unk_0ac = unk_0a8;
    mRoundState = 1;

    ResetSlots();

    unk_0a4 = 1;

    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);

    data_ov004_020bc880 = 0x80;
    data_ov004_020bc884 = ~0x3f;

    mHudScore = 0;

    return 1;
}

extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned int sz);
extern void *_ZN11dScMgBase_cC2Ev(void *p);
extern void _ZN8Particle10SysTrackerC1Ev(void *);
extern void *_ZN5ModelC1Ev(void *);
extern void *_ZN15dMg3DEspModel_cC1Ev(void *);
extern void *_ZN18TextureTransformerC1Ev(void *);
extern int _ZTV19dScMgSingle3DBase_c[];
extern int _ZTV12dScMg3DEsp_c[];
void *dScMg3DEsp_c_classInit(void);
/* Reconstructed source-style name: SM64DS proves dScMg3DEsp_c through RTTI,
 * allocation size, vtable identity, and the MG_3DESP registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgPsycheOut_Spawn. */
// @symbol dScMg3DEsp_c_classInit
void *dScMg3DEsp_c_classInit(void){
    char *o = (char *)_ZN7fBase_cnwEj(0x5558);
    if(o != 0){
        _ZN11dScMgBase_cC2Ev(o);
        *(int *)o = (int)_ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(o + 0x471c);
        *(int *)o = (int)&_ZTV12dScMg3DEsp_c[2];
        _ZN5ModelC1Ev(o + 0x4f38);
        _ZN5ModelC1Ev(o + 0x4f88);
        _ZN15dMg3DEspModel_cC1Ev(o + 0x4fd8);
        _ZN18TextureTransformerC1Ev(o + 0x51f4);
    }
    return o;
}
}
