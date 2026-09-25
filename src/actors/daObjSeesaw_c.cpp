//cpp
/* Production translation unit for ov095/daObjSeesaw_c.
 * 10 function(s), .text 0x02135700..0x02135cdc. The tilting seesaw platforms
 * (SEESAW, BOMB_SEESAW, KM1/KM3_SEESAW, KM2/KM3_YOKOSEESAW, RC_SEESAW).
 *
 * NAME: _ZTS13daObjSeesaw_c is "13daObjSeesaw_c" at ov095 0x021373cc; _ZTI at
 * 0x021373c0 reads [__si_class_type_info, that string, _ZTI10dBgActor_c]. The
 * vtable address point is 0x021374fc, and the word at -4 is that _ZTI. The
 * tree previously called the class SeesawBob (coined; vtable address only).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x02135700), D0
 * (0x02135744), then a D2 the cartridge has no home for (manifest:
 * compiler_only_output); the same pragma lays .text down in source order, so
 * this file is ROM-ascending.
 *
 * func_ov095_02135cdc (a tilt helper that also calls func_ov095_0213579c) and
 * its tail-call veneer func_ov095_02135e90 (the dBgW callback InitResources
 * installs) follow this run and are not folded yet. The seven classInit
 * factories start at 0x02135ea4 and stay in their own files.
 *
 * decl_common.h is first so common.h's flat Matrix4x3 stands.
 */

#pragma defer_codegen off

#include "decl_common.h"
#include "daObjSeesaw_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct Base {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int);
};
struct Derived { char pad[0xd4]; Base base; };

extern "C" {
extern int data_ov095_02136f58[];
int Vec3_Dist(void *a, void *b);
s16 Vec3_HorzAngle(void *a, void *b);
extern s16 data_02082214[];
int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, void *v, unsigned int d);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, int, void *, int, short, int);
extern void func_020393d4(void *, void *);
extern void func_020393c4(void *, void *);
}

/* D1 is two vtable stores and three destructor calls, every one a consequence
 * of `struct daObjSeesaw_c : dBgActor_c`: its own vptr, then dBgActor_c's --
 * inlined, because dBgActor_c's destructor is defined in its class body --
 * then dBgActor_c's Model and dBgW_KcMbg, then dActor_c. This class adds no
 * member with a destructor of its own. D0 adds the inline operator delete. */
// @symbol _ZN13daObjSeesaw_cD1Ev
// @symbol _ZN13daObjSeesaw_cD0Ev
daObjSeesaw_c::~daObjSeesaw_c()
{
}

// @symbol func_ov095_0213579c
extern "C" int func_ov095_0213579c(void* self, void* p0){
  char* p = (char*)p0;
  int r0 = data_ov095_02136f58[*(int*)(p+8) & 3];
  unsigned eq = (unsigned)(*(unsigned short*)(p+0xc) == 0xbf);
  if(eq == 0) return r0;
  if(*(unsigned char*)(p+0x703) != 0) r0 = 0x2000;
  return r0;
}

#pragma push
#pragma opt_propagation off
// @symbol _ZN13daObjSeesaw_c15OnGroundPoundedER8dActor_c
void daObjSeesaw_c::OnGroundPounded(dActor_c &other)
{
    char *a = (char *)this;
    char *b = (char *)&other;
    char *b5c = b + 0x5c;
    int dist = Vec3_Dist(a + 0x5c, b5c);
    s16 angle = Vec3_HorzAngle(a + 0x5c, b5c);
    int n, d, idx, prod, cur, v;
    s16 *p;
    mPoundedThisFrame = 1;
    n = func_ov095_0213579c(a, b);
    d = AngleDiff(angle, *(s16 *)(a + 0x8e));
    idx = ((u16)(s16)d >> 4) << 1;

    prod = (int)(((s64)dist * n + 0x800) >> 12);
    idx = idx + 1;

    p = (s16 *)(a + 0x8c);
    cur = *p;
    v = (int)(((s64)prod * data_02082214[idx] + 0x800) >> 12);
    v = (v + ((unsigned)(v >> 11) >> 20)) << 4;
    *p = (s16)(cur + (v >> 16));
    if (*(s16 *)(a + 0x8c) > 0x2000)
        *(s16 *)(a + 0x8c) = 0x2000;
    if (*(s16 *)(a + 0x8c) < -0x2000)
        *(s16 *)(a + 0x8c) = -0x2000;
}
#pragma pop

#pragma push
#pragma opt_common_subs off
// @symbol func_ov095_021358cc
extern "C" int func_ov095_021358cc(int a, short *pos, short *vel, int target, short thresh, int accel, short mult)
{
    short old = pos[0];
    pos[0] = old + vel[0];
    short now = pos[0];
    if (now == target
        || ((now - target) * (old - target) < 0
            && vel[0] > -thresh && vel[0] < thresh)) {
        pos[0] = target;
        vel[0] = 0;
        return 1;
    }
    if (now >= target)
        accel = (short)-accel;
    if ((short)vel[0] * (short)accel < 0)
        accel = (short)accel * (short)mult;
    vel[0] = vel[0] + accel;
    return 0;
}
#pragma pop

// @symbol func_ov095_0213597c
extern "C" void func_ov095_0213597c(char *t)
{
    Matrix4x3_FromRotationXYZExt(t + 0xf0, *(short *)(t + 0x8c), *(short *)(t + 0x8e), *(short *)(t + 0x90));
    *(int *)(t + 0x114) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x118) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x11c) = *(int *)(t + 0x64) >> 3;
}

// @symbol _ZN13daObjSeesaw_c16CleanupResourcesEv
int daObjSeesaw_c::CleanupResources()
{
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled())
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    ((SharedFilePtr *)(*(void **)(data_ov095_021374a0 + mVariant * 0xc)))->Release();
    ((SharedFilePtr *)(*(void **)(data_ov095_021374a4 + mVariant * 0xc)))->Release();
    return 1;
}

// @symbol _ZN13daObjSeesaw_c6RenderEv
int daObjSeesaw_c::Render()
{
    Base *b = &((Derived *)this)->base; b->m(0); return 1;
}

// @symbol _ZN13daObjSeesaw_c8BehaviorEv
int daObjSeesaw_c::Behavior()
{
    int b = (int)(((*(s32 *)&mFlags) & 8) != 0);
    if (b != 0) {
        if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled()) {
            ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
        }
        return 1;
    }
    if (mPoundedThisFrame == 0) {
        func_ov095_021358cc((int)((char*)this), (short*)(((char*)this) + 0x8c), (short*)(((char*)this) + 0x324), 0, 6, 3, 3);
    }
    {
        int s = mAngleXSpeed;
        if (s < 0) s = (short)-s;
        if (s > 0xa) {
            mTiltSound = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                mTiltSound, 3, 0x8b, ((char *)this) + 0x74, 0);
        }
    }
    if (mAngleX > 0x2000) mAngleX = 0x2000;
    if (mAngleX < -0x2000) mAngleX = -0x2000;
    func_ov095_0213597c(((char *)this));
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char *)this), 0, 0)) {
        UpdateClsnPosAndRot();
    }
    mPoundedThisFrame = 0;
    return 1;
}

// @symbol _ZN13daObjSeesaw_c13InitResourcesEv
int daObjSeesaw_c::InitResources()
{
    unsigned char idx;
    int f;
    switch (actorID) {
        case 0x1c: mVariant = 0; break;
        case 0x27: mVariant = 1; break;
        case 0x85: mVariant = 2; break;
        case 0x8f: mVariant = 3; break;
        case 0x95: mVariant = 4; break;
        case 0x96: mVariant = 5; break;
        case 0x80: mVariant = 6; break;
    }
    idx = mVariant;
    f = (int)Model::LoadFile(**(SharedFilePtr **)(data_ov095_021374a0 + idx * 0xc));
    ((ModelBase *)&mModel)->SetFile((BMD_File *)f, 1, -1);
    func_ov095_0213597c(((char *)this));
    UpdateClsnPosAndRot();
    {
        unsigned char i = mVariant;
        f = (int)dBgW_Kc::LoadFile(**(SharedFilePtr **)(data_ov095_021374a4 + i * 0xc));
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, f, &mClsnMat, 0x1000, mAngleY, *(int *)(data_ov095_021374a8 + i * 0xc));
    }
    func_020393d4(&mMeshCollider, (int *)&dBgW::UpdatePosWithTransform);
    func_020393c4(&mMeshCollider, func_ov095_02135e90);
    return 1;
}
