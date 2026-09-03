//cpp
/* SHADOW translation unit -- ov022/daObjFl_London_c (8 function(s)).
 *
 * NOT ENROLLED, NOT CANONICAL until tools/tu_promote.py moves it into src/.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder; the
 * destructor pair at the end is the one documented exception, because the
 * compiler picks the order inside a D0/D1 group itself.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02111980  src/_ZN16daObjFl_London_cD1Ev.cpp
 *   [1] 0x021119c4  src/_ZN16daObjFl_London_cD0Ev.cpp
 *   [2] 0x02111a1c  src/func_ov022_02111a1c.c
 *   [3] 0x02111a64  src/_ZN16daObjFl_London_c16CleanupResourcesEv.cpp
 *   [4] 0x02111aa8  src/_ZN16daObjFl_London_c6RenderEv.cpp
 *   [5] 0x02111ad0  src/_ZN16daObjFl_London_c8BehaviorEv.cpp
 *   [6] 0x02111bdc  src/_ZN16daObjFl_London_c13InitResourcesEv.cpp
 *   [7] 0x02111c7c  src/daObjFl_London_c_classInit.c
 *
 * plus the 0x1c actor descriptor this TU also owns, .data
 * 0x02113f2c..0x02113ff0: _ZTI/_ZTS for the class, g_profile_FL_LONDON, and
 * the 32-slot vtable whose 0x02113f70 address point the factory installs. The
 * run ends exactly where _ZTI16daObjFl_Seesaw_c begins, so no boundary symbol
 * is needed to bound the vtable's extent. tu_map splits the unlabelled factory
 * into a TU of its own; the manifest is extended over it by hand, which
 * config/tu_manifest.d/ov022/daObjFl_London_c.json records under
 * boundary_evidence.
 */

#include "daObjFl_London_c.h"
#include "decl_common.h"

/* Local shadow declarations carried from the legacy files verbatim. */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };

extern "C" {
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int a, void *v);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *t, int a, int b);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *t);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int fix, short s, void *clps);
extern void func_020393d4(void *p, void *v);
extern void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
extern void *data_ov022_02114580;
extern void *data_ov022_02114578;
extern void *data_ov064_0211bb2c;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daObjFl_London_c_classInit, 0x02111c7c, size 0x30         */
/* plus the .data 0x02113f4c descriptor that names it                         */
/* -------------------------------------------------------------------------- */
extern "C" {
extern void *_ZN7fBase_cnwEj(u32 size);
extern void _ZN10dBgActor_cC2Ev(void *self);
extern int _ZTV16daObjFl_London_c[];

/* @symbol daObjFl_London_c_classInit -- the factory the FL_LONDON descriptor
 * names. It keeps its coined spelling: `classInit` alone collides across every
 * actor and the real name is not in the cartridge. Historical alias:
 * daObjFl_London_c_Spawn. */
// @symbol daObjFl_London_c_classInit
int *daObjFl_London_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        /* &[2], not the bare symbol: this TU EMITS the vtable, so the compiler's
         * symbol is the object start at 0x02113f68 and +8 is what reaches the
         * 0x02113f70 address point the ROM actually stores. While the class only
         * IMPORTED its vtable, addend 0 was the right spelling for the same
         * word. */
        p[0] = (int)&_ZTV16daObjFl_London_c[2];
    }
    return p;
}
}

/* The 0x1c actor descriptor the profile table points at. Word 0 relocates to
 * the factory above; the rest is the ROM's, read back at 0x02113f4c. */
struct LondonSpawnInfo {
    int *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    s32 clipOffsetY;
    s32 clipRadius;
    s32 clipDistance;
    s32 farDistance;
};
typedef char LondonSpawnInfo_size_must_be_0x1c[sizeof(LondonSpawnInfo) == 0x1c ? 1 : -1];

// @symbol g_profile_FL_LONDON
extern "C" LondonSpawnInfo g_profile_FL_LONDON = {
    daObjFl_London_c_classInit, 0x0049, 0x0119, 0x00000002,
    0x00100000, 0x00300000, 0x02000000, 0x00000000
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN16daObjFl_London_c13InitResourcesEv, 0x02111bdc, 0xa0  */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_London_c13InitResourcesEv
int daObjFl_London_c::InitResources()
{
    void *f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov022_02114580);
    mModel.SetFile((BMD_File *)f, 1, -1);
    func_ov022_02111a1c((char *)this);
    UpdateClsnPosAndRot();
    void *k = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&data_ov022_02114578);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, k, &mClsnMat, 0x1000, mAngleY, &data_ov064_0211bb2c);
    func_020393d4(&mMeshCollider, (void *)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    mFlag = 0;
    mCooldown = 0xf;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN16daObjFl_London_c8BehaviorEv, 0x02111ad0, size 0x10c  */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_London_c8BehaviorEv
s32 daObjFl_London_c::Behavior()
{
    char *c = (char *)this;
    if (DecIfAbove0_Byte((unsigned char *)c + 0x31e) == 0) {
        if (mFlag == 0) {
            short *p = (short *)(c + 0x96);
            *p = *p - 0x100;
            if (mPrevAngleZ <= -0x2000) { mPrevAngleZ = -0x2000; mCooldown = 0xf; mFlag = 1; }
        } else {
            short *p = (short *)(c + 0x96);
            *p = *p + 0x100;
            if (mPrevAngleZ >= 0) { mPrevAngleZ = 0; mCooldown = 0xf; mFlag = 0; }
        }
    } else {
        if (mCooldown == 1) {
            if (mFlag == 0) _ZN5Sound9PlayBank3EjRK7Vector3(0x34, c + 0x74);
            else            _ZN5Sound9PlayBank3EjRK7Vector3(0x35, c + 0x74);
        }
    }
    mAngleZ = mPrevAngleZ;
    func_ov022_02111a1c(c);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN16daObjFl_London_c6RenderEv, 0x02111aa8, size 0x28     */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_London_c6RenderEv
s32 daObjFl_London_c::Render()
{
    Derived *d = (Derived *)this; Base *b = &d->base; b->m(0); return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN16daObjFl_London_c16CleanupResourcesEv, 0x02111a64     */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_London_c16CleanupResourcesEv
s32 daObjFl_London_c::CleanupResources()
{
    void *t = (void *)this;
    if (_ZN4dBgW9IsEnabledEv((char *)t + 0x124)) {
        _ZN4dBgW7DisableEv((char *)t + 0x124);
    }
    _ZN13SharedFilePtr7ReleaseEv(&data_ov022_02114580);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov022_02114578);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov022_02111a1c, 0x02111a1c, size 0x48                */
/* -------------------------------------------------------------------------- */
// @symbol func_ov022_02111a1c
/* This class's own still-unnamed helper: rebuild the model matrix from the
 * actor's three angles, then publish the position at 1/8 scale. Both callers
 * (InitResources and Behavior) are in this TU. */
extern "C" void func_ov022_02111a1c(char *t)
{
    Matrix4x3_FromRotationZXYExt(t + 0xf0, *(short *)(t + 0x8c), *(short *)(t + 0x8e), *(short *)(t + 0x90));
    *(int *)(t + 0x114) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x118) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x11c) = *(int *)(t + 0x64) >> 3;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN16daObjFl_London_cD0Ev, 0x021119c4, size 0x58          */
/* ROM ordinal 0 -- _ZN16daObjFl_London_cD1Ev, 0x02111980, size 0x44          */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjFl_London_cD1Ev
// @symbol _ZN16daObjFl_London_cD0Ev
/* Both destructors are emitted from the INLINE `~daObjFl_London_c() {}` in
 * include/daObjFl_London_c.h -- there is deliberately no body here.
 *
 * A complete-object destructor stores this class's vtable over the one the base
 * constructor left, then dBgActor_c's -- inlined, because dBgActor_c declares
 * its destructor in its own class body -- then destroys the base's members in
 * reverse declaration order: dBgW_KcMbg at 0x124 and Model at 0xd4, then the
 * dActor_c base. This class adds only two u8 fields, which have nothing to
 * destroy. The deleting destructor does all of that and calls operator delete,
 * inlined, which is why neither body mentions a heap.
 *
 * Defining ~daObjFl_London_c() out of line HERE instead would break the TU two
 * ways: mwccarm would emit D0 at 0x021119c4 before D1 at 0x02111980, reversing
 * ROM order so objisolate refuses the entire TU, and it would emit a third
 * symbol, D2, which has no address anywhere in the cartridge.
 */
