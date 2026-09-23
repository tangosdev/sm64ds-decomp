//cpp
/* Koopa the Quick (ROM RTTI: daRNk_c), overlay 62.
 * This production TU owns the 18 functions at 0x0211975c..0x0211af38.
 * Class identity, lifecycle and four actor overrides are reconstructed; the
 * remaining address-named helpers still retain some raw field and ABI views.
 * Their original source lineage is recorded in the TU manifest.
 *
 * The pinned compiler emits these functions in reverse source order. The
 * inline destructor and factory supply the retail D1/D0 order and class data;
 * the text-only manifest verifies and externalizes the emitted RTTI/vtable.
 * Measured source-form constraints: notes/experiments/pr2859-source-repair-0920.json.
 *
 * Leftover: the func_ov062_* helpers keep linker names; naming belongs
 *   at their definitions.
 */
#include "daRNk_c.h"
#include "Player.h"
#include "types.h"
#include "common.h"
#include "Timer.h"
#include "SharedFilePtr.h"

int ApproachLinear(short &value, short target, short step);
int ApproachLinear(int &value, int target, int step);

extern "C" {
void func_ov062_02119800(char *self);
void func_ov062_02119954(void *self);
int func_ov062_021199ac(char *self);
int func_ov062_02119af0(char *self);
void func_ov062_02119be0(char *self);
void func_ov062_0211a0f0(char *self);
void func_ov062_0211a168(char *self);
void func_ov062_0211a1f4(char *self);
void func_ov062_0211a740(char *self);
void func_ov062_0211a9c4(char *self);
void func_ov062_0211aac0(char *self);
extern int data_ov062_0211e0a4[];
void _ZN5Sound22LoadAndSetMusic_Layer2Ej(u32 id);
}

/* Scalar fixed-point ABI entry; the class-valued form still needs
   caller reconstruction under the pinned compiler. */
extern "C" s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);

extern "C" {
extern void func_0201267c(unsigned int id, void *p);
extern void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern short data_02082214[];
/* This scalar entry follows its C definition, including u16 startFrame.
   ModelAnim.h instead declares u32; that shared contract is still unresolved. */
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, u16);
extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern s32 Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern void _ZN5Sound22StopLoadedMusic_Layer2Ev(void);
extern unsigned int func_0201277c(unsigned int id);
extern signed char data_0209f2f8;
extern s8 data_ov002_02111184;
extern bool _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32 a, u32 b, u32 c, int d, int e);
extern void func_02012694(u32 a, void *b);
unsigned int func_02012790(unsigned int id);
extern u8 data_0209d684;
extern Timer data_0209d4c8;
extern void Vec3_Asr(void *destination, void *source, int shift);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, short angY);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* self, void* sm, void* mtx, int a, int b, unsigned char g);
extern struct Matrix4x3 data_020a0e68;
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor, int r, int h, unsigned int d, unsigned int e);
/* The existing C initializer erases pointer slots to integers. Keep the
   evidenced actor view here; repairing the shared initializer is separate. */
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int r, int h, void *p, int q);
extern char data_ov062_0211e00c[];
extern int data_ov062_0211e014[];
extern int data_ov062_0211e024[];
extern char data_ov062_0211e01c[];
extern char data_ov062_0211e034[];
extern char data_ov062_0211e03c[];
extern int data_ov062_0211e02c[];
extern int data_ov062_0211e004[];
}

// @symbol daRNk_c_classInit
extern "C" {  /* .c-derived member: C linkage for the whole block */
int *daRNk_c_classInit(void)
{
    return (int *)new daRNk_c;
}
}

// @symbol _ZN7daRNk_c13InitResourcesEv
int daRNk_c::InitResources()
{
    unsigned char b;
    int zero;

    Model::LoadFile(*(SharedFilePtr *)data_ov062_0211e00c);
    Animation::LoadFile(*(SharedFilePtr *)data_ov062_0211e014);
    Animation::LoadFile(*(SharedFilePtr *)data_ov062_0211e024);
    Animation::LoadFile(*(SharedFilePtr *)data_ov062_0211e01c);
    Animation::LoadFile(*(SharedFilePtr *)data_ov062_0211e034);
    Animation::LoadFile(*(SharedFilePtr *)data_ov062_0211e03c);
    Animation::LoadFile(*(SharedFilePtr *)data_ov062_0211e02c);
    Animation::LoadFile(*(SharedFilePtr *)data_ov062_0211e004);
    if (mModelAnim.SetFile(*(BMD_File **)(data_ov062_0211e00c + 4), 1, -1) == 0)
        return 0;
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(void **)(data_ov062_0211e034 + 4), 0, 0x1000, 0);
    mScaleX = 0x14cc;
    mScaleY = 0x14cc;
    mScaleZ = 0x14cc;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x78000, 0x12c000, 0x800004, 0);
    zero = 0;
    mState = zero;
    unk_3aa = (s16)zero;
    mHasFinished = (unsigned char)zero;
    unk_39c = mPosX;
    unk_3a0 = mPosY;
    unk_3a4 = mPosZ;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x78000, 0x78000, 0, 0);
    mPathPtr.FromID(param1 & 0xf);
    mNumPathPts = mPathPtr.NumNodes();
    mCurPathPt = zero;
    unk_3c0 = mPosX;
    unk_3c4 = mPosY;
    unk_3c8 = mPosZ;
    mPathPtr.GetNode(mPathTarget, mCurPathPt);
    mHasPlayerUsedCannon = (unsigned char)zero;
    b = (unsigned char)((((unsigned int)param1 >> 4) + 1) & 0x3f);
    mPathPtToJumpAt1 = b;
    b = (unsigned char)((((unsigned int)param1 >> 10) + 1) & 0x3f);
    mPathPtToJumpAt2 = b;
    if (mPathPtToJumpAt1 <= 1)
        mPathPtToJumpAt1 = 0xff;
    if (mPathPtToJumpAt2 <= 1)
        mPathPtToJumpAt2 = 0xff;
    mStarID = (unsigned char)(mAngleX & 0xf);
    b = mStarID;
    mTrackedStar = TrackStar(b, 2);
    mFlagID = zero;
    mPlayer = 0;
    mIsRacing = (unsigned char)zero;
    mIsTalkingToMario = (unsigned char)zero;
    return 1;
}

// @symbol _ZN7daRNk_c16CleanupResourcesEv
int daRNk_c::CleanupResources()
{
    ((SharedFilePtr *)&data_ov062_0211e00c)->Release();
    ((SharedFilePtr *)&data_ov062_0211e014)->Release();
    ((SharedFilePtr *)&data_ov062_0211e024)->Release();
    ((SharedFilePtr *)&data_ov062_0211e01c)->Release();
    ((SharedFilePtr *)&data_ov062_0211e034)->Release();
    ((SharedFilePtr *)&data_ov062_0211e03c)->Release();
    ((SharedFilePtr *)&data_ov062_0211e02c)->Release();
    ((SharedFilePtr *)&data_ov062_0211e004)->Release();
    if (mIsRacing) _ZN5Sound22StopLoadedMusic_Layer2Ev();
    return 1;
}

// @symbol _ZN7daRNk_c8BehaviorEv
int daRNk_c::Behavior()
{
  typedef void (daRNk_c::*StateFunc)();
  StateFunc *states = (StateFunc *)data_ov062_0211e0a4;
  (this->*states[mState])();
  mModelAnim.Advance();
  mAngleY = mPrevAngleY;
  UpdatePos(&mdCcAc_c);
  UpdateWMClsn(mWithMeshClsn, 0);
  mdCcAc_c.Clear();
  mdCcAc_c.Update();
  func_ov062_0211aac0(((char*)this));
  return 1;
}

// @symbol _ZN7daRNk_c6RenderEv
/* Model is the real class now, through daRNk_c.h: HideMaterial is its own
   non-virtual and the slot-5 virtual is Render, which ModelAnim overrides. */
int daRNk_c::Render()
{
  mModelAnim.HideMaterial(0, 1);
  mModelAnim.Render((const Vector3 *)&mScaleX);
  return 1;
}

// @symbol func_ov062_0211aac0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov062_0211aac0(char* r6){
  struct Vector3 v;
  Vec3_Asr(&v, (struct Vector3*)(r6 + 0x5c), 3);
  Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
  Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(r6 + 0x8e));
  { struct M43w { int w[12]; };  /* array-wrapper copy: keeps C's block copy under -lang c++ */
    *(M43w*)(r6 + 0x31c) = *(M43w*)&data_020a0e68; }
  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(r6, r6 + 0x364, r6 + 0x31c, 0xa0000, 0xa0000, 0xf);
}
}

// @symbol func_ov062_0211a9c4
extern "C" {  /* .c-derived member: C linkage for the whole block */

void func_ov062_0211a9c4(char *c)
{
    unsigned short val;
    struct Vector3 v;
    char *a;
    struct Vector3 *sp;

    val = *(unsigned short *)(c + 0x100);
    if (val != 0) {
        *(unsigned short *)(c + 0x100) = val - 1;
        return;
    }

    ((daRNk_c *)c)->mPlayer = ((dActor_c *)c)->ClosestPlayer();
    if (((daRNk_c *)c)->mPlayer == 0)
        return;

    sp = (struct Vector3 *)((int)((daRNk_c *)c)->mPlayer + 0x5c);
    v.x = sp->x;
    v.y = sp->y;
    v.z = sp->z;

    if (Vec3_Dist((struct Vector3 *)(c + 0x5c), &v) >= 0xc8000)
        return;

    if (((daRNk_c *)c)->mPlayer->StartTalk(*(fBase_c *)c, true) == 0)
        return;

    a = (char *)dActor_c::FindWithActorID(0xcd, 0);
    if (a == 0)
        return;

    *(int *)(c + 0x394) = *(int *)(a + 4);
    *(unsigned char *)(a + 0x16e) = 0;
    *(int *)(c + 0x38c) = 1;
    {
        char *base = c + 0x300;
        *(short *)(base + 0xa8) = Vec3_HorzAngle((struct Vector3 *)(c + 0x5c), &v);
    }
    *(unsigned char *)(c + 0x390) = 0;
}
}

// @symbol func_ov062_0211a740
/* Offer the race after the player finishes talking. */
extern "C" void func_ov062_0211a740(char* c)
{
    switch (*(u8*)(c + 0x390)) {
    case 0:
        if (ApproachLinear(*(s16*)(c + 0x94), *(s16*)(c + 0x3a8), 0x800) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void*)(c + 0x300), *(void**)(data_ov062_0211e03c + 4), 0, 0x1000, 0);
            *(u8*)(c + 0x390) += 1;
            func_0201267c(0xec, c + 0x74);
            if (*(int*)((char *)((daRNk_c *)c)->mPlayer + 8) == 0) {
                func_02012790(0xa);
                *(u8*)(c + 0x3b6) = 1;
            }
        }
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        return;
    case 1:
        if (((daRNk_c *)c)->mPlayer->GetTalkState() != 0)
            return;
        {
            unsigned int msg;
            Vector3 v;
            if (*(u8*)(c + 0x3b6) == 0) {
                msg = 0x9e;
            } else if (data_0209f2f8 == 0x18) {
                msg = 0xc4;
            } else {
                msg = 0x91;
            }

            {
                int z = *(int*)(c + 0x64);
                int y = *(int*)(c + 0x60) + 0xc8000;
                int x = *(int*)(c + 0x5c);
                v.x = x;
                v.y = y;
                v.z = z;
            }
            if (((daRNk_c *)c)->mPlayer->ShowMessage(*(fBase_c *)c, msg, &v, 1, 0) != 0)
                *(u8*)(c + 0x390) += 1;
        }
        return;
    case 2:
        if (((daRNk_c *)c)->mPlayer->GetTalkState() != 2)
            return;
        if (*(u8*)(c + 0x3b6) != 0) {
            if (data_0209d684 == 1) {
                *(int*)(c + 0x38c) = 2;
                *(u16*)(c + 0x100) = 0x32;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void*)(c + 0x300), *(void**)(data_ov062_0211e034 + 4), 0, 0x1000, 0);
                *(u16*)(c + 0x3aa) = 0;
                *(u8*)(c + 0x3b6) = 0;
            } else if (data_0209d684 == 2) {
                *(int*)(c + 0x38c) = 0;
                *(u16*)(c + 0x100) = 0x3c;
                ((daRNk_c *)c)->mPlayer->HasFinishedTalking();
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void*)(c + 0x300), *(void**)(data_ov062_0211e034 + 4), 0, 0x1000, 0);
                *(u8*)(c + 0x3b6) = 0;
            }
        } else {
            *(int*)(c + 0x38c) = 0;
            *(u16*)(c + 0x100) = 0x3c;
            ((daRNk_c *)c)->mPlayer->HasFinishedTalking();
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void*)(c + 0x300), *(void**)(data_ov062_0211e034 + 4), 0, 0x1000, 0);
        }
        *(u8*)(c + 0x390) = 0;
        *(int*)(c + 0x98) = 0;
        data_0209d4c8.ResetTimer();
        return;
    default:
        return;
    }
}

// @symbol func_ov062_0211a1f4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov062_0211a1f4(char *a)
{
        int r6;
    int r5;
    int r4;
    int r7;
    volatile int tmp[3];

    switch (*(u8 *)(a + 0x390)) {
    case 0:
        if (((daRNk_c *)a)->mPlayer->Unk_020c4f40(0x5a) != 0)
            (*(u8 *)(((int)a + 0x390)))++;
        return;
    case 1:
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1f, 0x14, 0x7f, 0x6b000, 0);
        if (((daRNk_c *)a)->mPlayer->GetTalkState() != -1)
            return;
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1f, 0x7f, 0, 0x7f000, 0);
        func_02012694(0x4d, a + 0x74);
        _ZN5Sound22LoadAndSetMusic_Layer2Ej(0x41);
        *(u8 *)(a + 0x3b5) = 1;
        (*(u8 *)(((int)a + 0x390)))++;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(a + 0x300, (void *)data_ov062_0211e014[1], 0x40000000, 0x1000, 0);
        data_ov002_02111184 = 1;
        data_0209d4c8.StartTimer();
        func_0201267c(0x4d, a + 0x74);
        return;
    case 2:
        r6 = func_ov062_02119af0(a);
        if (*(int *)(a + 0x360) == data_ov062_0211e024[1])
            func_ov062_02119800(a);
        if (r6 == -1) {
            char *o;
            if (*(int *)(a + 0x394) == 0)
                return;
            o = (char *)dActor_c::FindWithID(*(int *)(a + 0x394));
            if (o == 0)
                return;
            *(int *)(a + 0x38c) = 3;
            *(u8 *)(a + 0x390) = 0;
            data_0209d4c8.StopTimer();
            *(u8 *)(a + 0x3af) = (*(u8 *)(o + 0x16e) != 0) ? 1 : 0;
            *(u8 *)(o + 0x16e) = 1;
            return;
        }
        r4 = func_ov062_021199ac(a);
        if (((dActor_c *)a)->GetSubtraction(*(s16 *)(a + 0x94),
                _ZN4cstd5atan2E5Fix12IiES1_(*(int *)(a + 0xd4), *(int *)(a + 0xdc))) >= 0x6000)
            r7 = *(int *)(a + 0xd8) * 7 - 0x6000;
        else
            r7 = 0x1000;
        if (*(u8 *)(a + 0x3ac) == 0)
            *(u8 *)(a + 0x3ac) = ((daRNk_c *)a)->mPlayer->IsBeingShotOutOfCannon();
        r5 = 4;
        if (*(int *)(a + 0x394) != 0) {
            char *o = (char *)dActor_c::FindWithID(*(int *)(a + 0x394));
            if (o != 0) {
                if (*(u8 *)(o + 0x16e) != 0 &&
                    Vec3_Dist((const Vector3 *)(a + 0x5c),
                              (const Vector3 *)&((daRNk_c *)a)->mPlayer->mPosX) > 0x7d0000)
                    r5 = 8;
                else if (data_0209f2f8 == 0x18)
                    r5 = 6;
            }
        }
        {
            int acc = r5 * 0x6000;
            ApproachLinear(*(int *)(a + 0x98),
                (int)(((long long)acc * r7 + 0x800) >> 12), r5 * 0x19a);
        }
        ApproachLinear(*(s16 *)(a + 0x94), *(s16 *)(a + 0x3a8), 0x800);
        if (*(int *)(a + 0x360) == data_ov062_0211e024[1]) {
            *(int *)(a + 0x35c) = *(int *)(a + 0x98) >> 3;
        } else {
            *(int *)(a + 0x35c) = 0x1000;
            if (((Animation *)(a + 0x350))->Finished() != 0 &&
                (*(int *)(a + 0x360) == data_ov062_0211e014[1] || *(int *)(a + 0x360) == data_ov062_0211e004[1]))
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(a + 0x300, (void *)data_ov062_0211e024[1], 0, 0x1000, 0);
        }
        if (r6 == 1 && (*(int *)(a + 0x3bc) == *(u8 *)(a + 0x3ad) || *(int *)(a + 0x3bc) == *(u8 *)(a + 0x3ae))) {
            func_ov062_02119954(a);
            return;
        }
        if (r4 != 0) {
            if (r4 < 0)
                *(int *)(a + 0x98) = 0;
            if (r4 == 0)
                return;
            func_ov062_02119954(a);
            return;
        }
        if (((dBgCh_Actr *)(a + 0x144))->IsOnGround() != 0)
            return;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(a + 0x300, (void *)data_ov062_0211e02c[1], 0x40000000, 0x1000, 0);
        *(int *)(a + 0xa8) = 0xa000;
        *(int *)(a + 0x98) = (int)(((long long)*(int *)(a + 0x98) * 0xd00 + 0x800) >> 12);
        *(u8 *)(a + 0x390) = 3;
        return;
    case 3:
        if (func_ov062_02119af0(a) == -1) {
            char *o;
            *(int *)(a + 0x38c) = 3;
            *(u8 *)(a + 0x390) = 0;
            data_0209d4c8.StopTimer();
            o = (char *)dActor_c::FindWithID(*(int *)(a + 0x394));
            *(u8 *)(a + 0x3af) = (*(u8 *)(o + 0x16e) != 0) ? 1 : 0;
            *(u8 *)(o + 0x16e) = 1;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(a + 0x300, (void *)data_ov062_0211e004[1], 0x40000000, 0x1000, 0);
            return;
        }
        ApproachLinear(*(s16 *)(a + 0x94), *(s16 *)(a + 0x3a8), 0x800);
        if (((dBgCh_Actr *)(a + 0x144))->IsOnGround() == 0)
            return;
        *(u8 *)(a + 0x390) = 2;
        *(int *)(a + 0x9c) = -0x2000;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(a + 0x300, (void *)data_ov062_0211e004[1], 0x40000000, 0x1000, 0);
        *(int *)(a + 0x358) = 0;
        tmp[0] = *(int *)(a + 0x5c);
        tmp[1] = *(int *)(a + 0x60);
        tmp[2] = *(int *)(a + 0x64);
        tmp[1] = *(int *)(a + 0x60) + 0x28000;
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb2, *(int *)(a + 0x5c),
            *(int *)(a + 0x60) + 0x28000, *(int *)(a + 0x64));
        return;
    default:
        return;
    }
}
}

// @symbol func_ov062_0211a168
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov062_0211a168(char* r4){
  ApproachLinear(*(int*)(r4 + 0x98), 0x3000, 0x1000);
  if (((Animation *)(r4 + 0x350))->WillHitFrame(
        (unsigned short)(((Animation *)(r4 + 0x350))->GetFrameCount() - 1)) == 0) return;
  *(int*)(r4 + 0x38c) = 4;
  *(int*)(r4 + 0x98) = 0x3000;
  *(unsigned char*)(r4 + 0x390) = 0;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)(r4 + 0x300), *(void**)(data_ov062_0211e01c + 4), 0x40000000, 0x1000, 0);
}
}

// @symbol func_ov062_0211a0f0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov062_0211a0f0(char* c)
{
    ApproachLinear(*(int*)(c + 0x98), 0, 0x4000);
    if (!((Animation *)(c + 0x350))->Finished()) return;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, *(void**)(data_ov062_0211e034 + 4), 0, 0x1000, 0);
    *(int*)(c + 0x38c) = 5;
    *(unsigned char*)(c + 0x390) = 0;
    *(short*)(c + 0x100) = 0;
}
}

// @symbol func_ov062_02119be0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov062_02119be0(char* self)
{
    Vector3 playerPos;
    Vector3 msgPos;
    Vector3 starPos;
    unsigned int msg;
    unsigned short t;
    unsigned short* tp;
    int x;
    int y;
    int z;

    tp = (unsigned short*)(self + 0x100);
    t = *tp;
    if (t != 0) {
        t--;
        *tp = t;
    }

    switch (*(unsigned char*)(self + 0x390)) {
    case 0:
        if (*(unsigned short*)(self + 0x100) != 0)
            return;
        {
            Vector3* pp = (Vector3 *)&((daRNk_c *)self)->mPlayer->mPosX;
            playerPos.x = pp->x;
            playerPos.y = pp->y;
            playerPos.z = pp->z;
        }
        if (Vec3_Dist((Vector3*)(self + 0x5c), &playerPos) >= 0x190000)
            return;
        if (((daRNk_c *)self)->mPlayer->StartTalk(*(fBase_c *)self, 1) == 0)
            return;
        *(short*)(self + 0x3a8) = Vec3_HorzAngle((Vector3*)(self + 0x5c), &playerPos);
        (*(unsigned char*)(((int)self + 0x390)))++;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x300, *(void**)(data_ov062_0211e03c + 4), 0, 0x1000, 0);
        if (*(unsigned char*)(self + 0x3b4) == 0) {
            _ZN5Sound22StopLoadedMusic_Layer2Ev();
            func_0201277c(0x4d);
            *(unsigned char*)(self + 0x3b5) = 0;
            *(unsigned char*)(self + 0x3b4) = 1;
        }
        *(unsigned char*)(self + 0x3b3) = 0;
        return;
    case 1:
        if (ApproachLinear(*(short*)(self + 0x94), *(short*)(self + 0x3a8), 0x800) != 0)
            (*(unsigned char*)(((int)self + 0x390)))++;
        *(short*)(self + 0x8e) = *(short*)(self + 0x94);
        return;
    case 2:
        if (*(int*)((char *)((daRNk_c *)self)->mPlayer + 8) == 0) {
            if (*(unsigned char*)(self + 0x3af) == 0) {
                msg = 0x14d;
            } else if (*(unsigned char*)(self + 0x3ac) != 0) {
                *(unsigned char*)(self + 0x3af) = 0;
                msg = 0x14c;
            } else if (data_0209f2f8 == 0x18) {
                msg = 0xc5;
            } else {
                msg = 0x92;
            }
        } else {
            *(unsigned char*)(self + 0x3af) = 0;
            *(unsigned char*)(self + 0x3b3) = 1;
            msg = 0x9f;
        }
        x = ((daRNk_c *)self)->mPlayer->GetTalkState();
        if (x != 0)
            return;
        x = *(int*)(self + 0x5c);
        z = *(int*)(self + 0x64);
        y = *(int*)(self + 0x60) + 0xc8000;
        msgPos.x = x;
        msgPos.y = y;
        msgPos.z = z;
        if (((daRNk_c *)self)->mPlayer->ShowMessage(*(fBase_c *)self, msg, &msgPos, 0, 0) != 0)
            (*(unsigned char*)(((int)self + 0x390)))++;
        return;
    case 3:
        if (((daRNk_c *)self)->mPlayer->GetTalkState() != 0xFFFFFFFF)
            return;
        (*(unsigned char*)(((int)self + 0x390)))++;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x300, *(void**)(data_ov062_0211e034 + 4), 0, 0x1000, 0);
        *(unsigned short*)(self + 0x100) = 0x3c;
        if (*(unsigned char*)(self + 0x3af) != 0) {
            starPos.x = *(int*)(self + 0x5c);
            starPos.y = *(int*)(self + 0x60);
            starPos.z = *(int*)(self + 0x64);
            starPos.y += 0x64000;
            ((dActor_c *)self)->UntrackAndSpawnStar(*(signed char *)(self + 0x3b0), *(unsigned char *)(self + 0x3b1), starPos, 4);
            return;
        }
        if (*(unsigned char*)(self + 0x3b3) == 1) {
            *(unsigned char*)(self + 0x3b3) = 0;
            *(unsigned char*)(self + 0x390) = 0;
        }
        return;
    case 4:
        {
            Vector3* pp = (Vector3 *)&((daRNk_c *)self)->mPlayer->mPosX;
            playerPos.x = pp->x;
            playerPos.y = pp->y;
            playerPos.z = pp->z;
        }
        if (Vec3_Dist((Vector3*)(self + 0x5c), &playerPos) >= 0x190000)
            return;
        if (((daRNk_c *)self)->mPlayer->StartTalk(*(fBase_c *)self, 0) != 0)
            (*(unsigned char*)(((int)self + 0x390)))++;
        return;
    case 5:
        if (((daRNk_c *)self)->mPlayer->GetTalkState() != 0)
            return;
        *(short*)(self + 0x3a8) = Vec3_HorzAngle((Vector3*)(self + 0x5c), (Vector3*)((char *)((daRNk_c *)self)->mPlayer + 0x5c));
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x300, *(void**)(data_ov062_0211e03c + 4), 0, 0x1000, 0);
        (*(unsigned char*)(((int)self + 0x390)))++;
        return;
    case 6:
        if (ApproachLinear(*(short*)(self + 0x94), *(short*)(self + 0x3a8), 0x800) != 0)
            (*(unsigned char*)(((int)self + 0x390)))++;
        *(short*)(self + 0x8e) = *(short*)(self + 0x94);
        return;
    case 7:
        {
            char* p = (char *)((daRNk_c *)self)->mPlayer;
            if (*(int*)(p + 8) == 0)
                msg = 0xa0;
            else
                msg = 0xa1;
            x = ((Player *)p)->GetTalkState();
            if (x != 0)
                return;
        }
        x = *(int*)(self + 0x5c);
        z = *(int*)(self + 0x64);
        y = *(int*)(self + 0x60) + 0xc8000;
        msgPos.x = x;
        msgPos.y = y;
        msgPos.z = z;
        if (((daRNk_c *)self)->mPlayer->ShowMessage(*(fBase_c *)self, msg, &msgPos, 0, 0) != 0)
            (*(unsigned char*)(((int)self + 0x390)))++;
        return;
    case 8:
        if (((daRNk_c *)self)->mPlayer->GetTalkState() != 0xFFFFFFFF)
            return;
        *(unsigned char*)(self + 0x390) = 4;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x300, *(void**)(data_ov062_0211e034 + 4), 0, 0x1000, 0);
        return;
    }
}
}

// @symbol func_ov062_02119af0
extern "C" int func_ov062_02119af0(char *p) {
    int dxc;
    int dzc;
    int dx;
    int dz;
    int ncc;
    int n5c;
    int nc0;
    int nd4;
    int n64;
    int nc8;

    ncc = *(int *)(p + 0x3cc);
    n5c = *(int *)(p + 0x5c);
    nc0 = *(int *)(p + 0x3c0);
    dx = ncc - n5c;
    dxc = ncc - nc0;  /* computed here, not after dz: first-use order drives the allocator under propagation (the standalone .c used #pragma opt_propagation off instead, which is file-final in a merged TU) */
    nd4 = *(int *)(p + 0x3d4);
    n64 = *(int *)(p + 0x64);
    nc8 = *(int *)(p + 0x3c8);
    dz = nd4 - n64;
    dzc = nd4 - nc8;
    *(short *)(p + 0x3a8) = (short)_ZN4cstd5atan2E5Fix12IiES1_(dx, dz);
    int dot = (dxc >> 0xc) * (dx >> 0xc) + (dzc >> 0xc) * (dz >> 0xc);
    if (dot <= 0 || Vec3_Dist((Vector3 *)(p + 0x5c), (Vector3 *)(p + 0x3cc)) < (*(int *)(p + 0x98) >> 1)) {
        int v = *(int *)(p + 0x3cc);
        int *cnt = (int *)(p + 0x3bc);
        *(int *)(p + 0x3c0) = v;
        *(int *)(p + 0x3c4) = *(int *)(p + 0x3d0);
        *(int *)(p + 0x3c8) = *(int *)(p + 0x3d4);
        *cnt = *cnt + 1;
        if (*(int *)(p + 0x3bc) >= *(int *)(p + 0x3b8)) {
            *(int *)(p + 0x3bc) = 0;
        }
        int idx = *(int *)(p + 0x3bc);
        if (idx == 0) return -1;
        ((PathPtr *)(p + 0x3d8))->GetNode(*(Vector3 *)(p + 0x3cc), (unsigned)idx);
        return 1;
    }
    return 0;
}

// @symbol func_ov062_021199ac
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov062_021199ac(char *self)
{
    char *other;
    s16 angle;
    s32 dist;
    s16 selfAngle;
    s16 otherAngle;
    u16 idx;
    s16 tableVal;
    s32 otherK;
    s32 fixed1;
    s32 r;

    other = (char *)((dActor_c *)self)->ClosestWithActorID(0xdc);
    if (other == 0)
        goto ret0;
    if (*(u8 *)(other + 0x3d0) != 2)
        goto ret0;

    angle = Vec3_HorzAngle((const struct Vector3 *)(self + 0x5c), (const struct Vector3 *)(other + 0x5c));
    dist = Vec3_Dist((const struct Vector3 *)(self + 0x5c), (const struct Vector3 *)(other + 0x5c));
    selfAngle = *(s16 *)(self + 0x94);
    otherAngle = *(s16 *)(other + 0x94);
    idx = (u16)(s16)(otherAngle - selfAngle);
    tableVal = data_02082214[(idx >> 4) * 2 + 1];
    otherK = *(s32 *)(other + 0x98);
    fixed1 = (s32)(((long long)otherK * tableVal + 0x800) >> 12);

    r = ((dActor_c *)self)->GetSubtraction(selfAngle, angle);
    if (r < 0x4000) {
        s32 selfK;
        s32 fixed2;
        if (dist >= 0x190000)
            goto ret0;
        selfK = *(s32 *)(self + 0x98);
        fixed2 = (s32)(((long long)selfK * 0xb33 + 0x800) >> 12);
        if (fixed1 < fixed2)
            return 1;
        *(s32 *)(self + 0x98) -= 0x2000;
        goto ret0;
    } else {
        if (dist >= 0x12c000)
            goto ret0;
        if (fixed1 > *(s32 *)(self + 0x98))
            return -1;
        goto ret0;
    }

ret0:
    return 0;
}
}

// @symbol func_ov062_02119954
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov062_02119954(void *c)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x300, (void*)data_ov062_0211e02c[1], 0x40000000, 0x1000, 0);
    *(int*)((char*)c + 0xa8) = 0x2d000;
    *(int*)((char*)c + 0x9c) = -0x4000;
    *(unsigned char*)((char*)c + 0x390) = 3;
}
}

// @symbol func_ov062_02119800
extern "C" void func_ov062_02119800(char *c)
{
    volatile int stack[3];
    char *self = c;
    unsigned int kind = ((unsigned int)(*(int *)(self + 0x358) << 4)) >> 16;
    short ang;
    int x, y, z;

    if (kind < 2)
        goto check_hi;
    if (kind <= 8)
        goto body;
check_hi:
    if (kind < 0x13)
        goto reset;
    if (kind > 0x19)
        goto reset;

body:
    if (*(unsigned char *)(self + 0x3b2) != 0)
        return;
    func_0201267c(0xe4, self + 0x74);
    *(unsigned char *)(self + 0x3b2) = 1;

    ang = *(short *)(self + 0x8e);
    x = *(int *)(self + 0x5c);
    /* Keep the comparison at this load boundary: the direct assignment
       moves six instructions under 2004/b56 (see the pinned experiment). */
    stack[0] = (kind > 8) ? x : x;
    y = *(int *)(self + 0x60);
    ang = (short)(ang + 0x4000);
    stack[1] = y;
    z = *(int *)(self + 0x64);
    stack[2] = z;
    stack[1] = y + 0x1e000;

    if (kind > 8)
        goto add_path;

    {
        unsigned short u = (unsigned short)ang;
        int n = (int)(u >> 4);
        short cs = data_02082214[n * 2];
        short sn = data_02082214[n * 2 + 1];
        short k = 0x1e;
        stack[0] = x - (int)(cs * k);
        stack[2] = z - (int)(sn * k);
    }
    goto do_new;

add_path:
    {
        unsigned short u = (unsigned short)ang;
        int n = (int)(u >> 4);
        short cs = data_02082214[n * 2];
        short sn = data_02082214[n * 2 + 1];
        short k = 0x1e;
        stack[0] = (int)(cs * k) + x;
        stack[2] = (int)(sn * k) + z;
    }
do_new:
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xf9, (int)stack[0], (int)stack[1], (int)stack[2]);
    return;

reset:
    *(unsigned char *)(self + 0x3b2) = 0;
}

/* D0 is the DELETING destructor: destroy through this class (dEnemyBase_c
 * chain) then return the object to its heap via an inline operator delete.
 * Both variants are emitted from the single inline destructor in
 * daRNk_c.h (class-form skill): D1 then D0 in ROM order, no leaf D2. */

/* D1 is emitted from the inline destructor in daRNk_c.h alongside D0
 * alongside D0. Members are destroyed in reverse declaration
 * order, then dEnemyBase_c::~dEnemyBase_c. */
