//cpp
/* daPukupuku_c -- Cheep Cheep (PUKUPUKU).
 *
 * Swims out from its spawn point, pauses, then swims again. InitResources
 * records the home position and installs the swim state. Behavior handles
 * Yoshi, steps the state timer, runs the current state, and hurts Mario on
 * contact. The swim state picks a random heading and speed; if it gets too
 * far from home it turns back. When the timer expires it pauses (speed 0),
 * then starts swimming again.
 *
 * common.h is first on purpose. Matrix4x3 is either common.h's flat s32 m[12]
 * or math/Matrix.h's nested {r, t}, and the first include wins. The nested
 * form splits the model-matrix copy in func_ov090_02133338. Nothing here
 * names .r or .t.
 *
 * Source order is the reverse of the ROM. mwccarm emits one .text section per
 * function in reverse source order. Do not reorder. The inline destructor in
 * daPukupuku_c.h emits D1 then D0 from `return new`; g_profile_PUKUPUKU stays
 * outside this TU.
 *
 * The state record is 16 bytes, filled by __sinit_ov090_02133f4c from the
 * four 8-byte pointer-to-member words at 0x021342b8. Enter is at +0 and is
 * called by func_ov090_021332e8; main is at +8 and is called by Behavior.
 * data_ov090_02134594 is swim (enter 02133290, main 02133200).
 * data_ov090_02134584 is pause (enter 021331c4, main 02133190).
 * PukuStatePMF is declared while PukuStateC is still incomplete: that is the
 * 8-byte member pointer the ROM table stores (function, this-delta 0).
 *
 * deslop leftovers:
 * - Render: `if ((mFlags & 0x40000) != 0) return 1` shrinks the function
 *   0x50 -> 0x44. The compare has to land in an int (`int b = ((mFlags &
 *   0x40000) != 0); if (b)`). mModelAnim.Render(0) itself matches.
 * - func_ov090_02133190 and func_ov090_02133200: `mStateTimer == 0` is
 *   ldrsh. The ROM is ldrh, so the test is
 *   `*(unsigned short *)&mStateTimer`. One word in 02133190.
 * - InitResources dCcAcPos_c::Init: a Fix12<int> temporary grows the
 *   function 0x104 -> 0x114. A bare int does not compile (no conversion
 *   to Fix12<int>). The scalar extern keeps the ROM symbol.
 * - InitResources ModelAnim::SetAnim: a Fix12<int> speed temporary grows
 *   it 0x104 -> 0x110. Same symbol, bigger body.
 * - InitResources dBgCh_Actr::Init: the header method (Fix12i) relocates
 *   to _ZN10dBgCh_Actr4InitEP8dActor_ciiP10Vector3_16S3_. The ROM symbol
 *   is _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_. The
 *   instruction bytes stay 0x104; the destination does not.
 */


#include "common.h"
#include "daPukupuku_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "Animation.h"
#include "decl_common.h"

/* shadow typedef 'Fix12i' */
typedef int Fix12i;

/* Behavior calls the main PMF, which sits 8 bytes into the state record. */
typedef void (dEnemyBase_c::*PukuMainPMF)();
struct PukuMainView {
    char enter[8];
    PukuMainPMF main;
};

/* BCA handle. The sinit builds it as a SharedFilePtr; SetAnim reads +4. */
struct AnimFilePtr { int a; struct BCA_File *file; };

extern "C" {
extern "C" void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj( Player*, const Vector3&, unsigned int, Fix12i, unsigned int, unsigned int, unsigned int);
extern Vector3 data_ov090_021342d8;
extern int RandomIntInternal(void*);
extern int data_0209e650[];
extern s32 Vec3_Dist(void* a, void* b);
extern s16 Vec3_HorzAngle(void* a, void* b);
extern int ApproachAngle(s16* angle, int target, int invFactor, int maxDelta, int minDelta);
extern void Vec3_Asr(void *dst, void *src, int n);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, short rx, short ry, short rz);
extern struct Matrix4x3 data_020a0e68;
extern SharedFilePtr data_ov090_02134564;
extern AnimFilePtr data_ov090_0213455c;
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
void *thisp, struct dActor_c *, struct Vector3 const &, int, int, unsigned int, unsigned int);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
void *thisp, struct dActor_c *, int, int, struct Vector3_16 *, int);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
void *thisp, struct BCA_File *, int, int, unsigned int);
}

// @symbol daPukupuku_c_classInit
extern "C" {
int *daPukupuku_c_classInit(void)
{
    return (int *)new daPukupuku_c;
}
}

// @symbol _ZN12daPukupuku_c13InitResourcesEv
int daPukupuku_c::InitResources()
{
    struct BMD_File *bmd;
    struct Vector3 v;

    bmd = (struct BMD_File *)Model::LoadFile(data_ov090_02134564);
    mModelAnim.SetFile(bmd, 1, -1);

    Animation::LoadFile(*(struct SharedFilePtr *)&data_ov090_0213455c);

    v = data_ov090_021342d8;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mdCcAcPos_c, this, v, 0x32000, 0x3c000, 0x200004, 0x8000);

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mAngleY = mPrevAngleY;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x1e000, 0x1e000, 0, 0);

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, data_ov090_0213455c.file, 0, 0x1000, 0);

    func_ov090_021332e8((PukuStatePMF *)&data_ov090_02134594);
    return 1;
}

// @symbol _ZN12daPukupuku_c8BehaviorEv
int daPukupuku_c::Behavior()
{
    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        mdCcAcPos_c.Clear();
        if (mEatenByYoshi != 0) {
            if (unk_104 == 0) {
                mdCcAcPos_c.Update();
            }
        }
        func_ov090_02133338((char *)this);
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    UpdatePos(&mdCcAcPos_c);
    {
        PukuMainView *q = *(PukuMainView **)pad_370;
        if (q->main != 0)
            (((dEnemyBase_c *)this)->*(q->main))();
    }
    mAngleY = mPrevAngleY;
    mModelAnim.speed = 0x1000;
    static_cast<Animation &>(mModelAnim).Advance();
    func_ov090_02133338((char *)this);
    func_ov090_021330c8((char *)this);
    mdCcAcPos_c.Clear();
    {
        Player *p = ClosestPlayer();
        if (p != 0 && p->mIsVanish == 0) {
            mdCcAcPos_c.Update();
        }
    }
    return 1;
}

// @symbol _ZN12daPukupuku_c6RenderEv
int daPukupuku_c::Render()
{
    int b = ((mFlags & 0x40000) != 0);
    if (b) return 1;
    mModelAnim.Render(0);
    return 1;
}

// @symbol _ZN12daPukupuku_c16OnPendingDestroyEv
void daPukupuku_c::OnPendingDestroy()
{
}

// @symbol _ZN12daPukupuku_c16CleanupResourcesEv
int daPukupuku_c::CleanupResources()
{
    data_ov090_02134564.Release();
    ((SharedFilePtr *)&data_ov090_0213455c)->Release();
    return 1;
}

// @symbol func_ov090_02133338
extern "C" {
void func_ov090_02133338(char *c) {
    daPukupuku_c *self = (daPukupuku_c *)c;
    int v[3];
    Vec3_Asr(v, &self->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        self->mAngleX, self->mAngleY, self->mAngleZ);
    self->mModelAnim.mat4x3 = data_020a0e68;
}
}

// @symbol _ZN12daPukupuku_c19func_ov090_021332e8EPM10PukuStateCFivE
struct PukuStateC { char pad[0x370]; PukuStatePMF *pp; };
int daPukupuku_c::func_ov090_021332e8(PukuStatePMF *p)
{
    *(PukuStatePMF **)((char *)this + 0x370) = p;
    PukuStatePMF *q = *(PukuStatePMF **)((char *)this + 0x370);
    if (*q == 0)
        return 1;
    return (((PukuStateC *)this)->**q)();
}

// @symbol func_ov090_02133290
extern "C" {
int func_ov090_02133290(char* c){
    daPukupuku_c *self = (daPukupuku_c *)c;
    unsigned int r = RandomIntInternal(data_0209e650);
    *(short *)(self->pad_380 + 4) = ((r >> 8) & 0xf) << 0xc;
    r = RandomIntInternal(data_0209e650);
    self->mStateTimer = (s16)(((r >> 8) & 0x3f) + 0x32);
    self->mHorzSpeed = 0x5000;
    return 1;
}
}

// @symbol _ZN12daPukupuku_c19func_ov090_02133200Ev
int daPukupuku_c::func_ov090_02133200()
{
    if (Vec3_Dist(&mPosX, &mHomePosX) > 0x3e8000) {
        *(unsigned short *)&mStateTimer = 0x32;
        *(s16 *)(pad_380 + 4) = Vec3_HorzAngle(&mPosX, &mHomePosX);
    }
    ApproachAngle(&mPrevAngleY, *(s16 *)(pad_380 + 4), 1, 0x100, 0x200);
    if (*(unsigned short *)&mStateTimer == 0)
        func_ov090_021332e8((PukuStatePMF *)data_ov090_02134584);
    return 1;
}

// @symbol func_ov090_021331c4
extern "C" {
int func_ov090_021331c4(char* c){
    daPukupuku_c *self = (daPukupuku_c *)c;
    unsigned int r = RandomIntInternal(data_0209e650);
    self->mStateTimer = (s16)(((r >> 8) & 0x3f) + 0x32);
    self->mHorzSpeed = 0;
    return 1;
}
}

// @symbol _ZN12daPukupuku_c19func_ov090_02133190Ev
int daPukupuku_c::func_ov090_02133190() {
    if (*(unsigned short *)&mStateTimer == 0) {
        func_ov090_021332e8((PukuStatePMF *)&data_ov090_02134594);
    }
    return 1;
}

// @symbol func_ov090_021330c8
extern "C" void func_ov090_021330c8(char* thiz)
{
    daPukupuku_c *c = (daPukupuku_c *)thiz;
    Vector3 v;
    v.x = data_ov090_021342d8.x;
    v.y = data_ov090_021342d8.y;
    v.z = data_ov090_021342d8.z;
    c->mdCcAcPos_c.SetPosRelativeToActor(v);
    {
        unsigned int id = c->mdCcAcPos_c.otherOwner;
        if (id == 0) return;
        {
            Player* a = (Player*)dActor_c::FindWithID(id);
            int b = (int)(a->actorID == 0xbf);
            if (b == 0) return;
            if (a->mIsVanish != 0) return;
            {
                Vector3 hv;
                hv.x = c->mPosX;
                hv.y = c->mPosY;
                hv.z = c->mPosZ;
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, hv, 1, 0xc000, 1, 0, 1);
            }
        }
    }
}

// @symbol _ZN12daPukupuku_cD0Ev
/* D0 is emitted from ~daPukupuku_c() in the header. */

// @symbol _ZN12daPukupuku_cD1Ev
/* D1 is emitted from the same inline destructor. */
