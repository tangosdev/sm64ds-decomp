//cpp
/* ov045 / daObjKm2_Agaru_c -- the rising fire-sea platform of the KM2 set
 * (registry profile KM2_AGARU, RTTI name at ov045:0x02112cb0).
 *
 * One translation unit for the whole ROM run 0x021111a0..0x0211150c: the six
 * class methods, the two file-local dBgW callbacks between them, and the
 * factory, plus the .data run 0x02112cb0..0x02112d74 that holds the class's
 * RTTI chain, its profile descriptor and its vtable.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S.
 * mwccarm 2004/b56 emits one `.text` section per function and lays those
 * sections out in the object in the REVERSE of their order in the source text,
 * so writing the LAST function of the ROM run FIRST is what makes the object's
 * section order equal the ROM's address order.
 *
 * The destructor is NOT written out of line below. It is defined in the class
 * body in include/daObjKm2_Agaru_c.h, which is what makes mwccarm emit the
 * cartridge's D1/D0 pair, in the cartridge's order, and no D2 at all.
 */
#include "decl_common.h"
#include "decl_ActorBase.h"
#include "decl_Platform.h"

#include "daObjKm2_Agaru_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

/* Reconciled from the nine per-function copies. Every one of these is a ROM
 * symbol spelled by its final mangled name, so it needs C linkage: seen as C++
 * the compiler would mangle the already-mangled spelling a second time and emit
 * a reference nothing defines. */
extern "C" {
extern int   _ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void  _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, int f, int a, int b);
extern void  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);
extern void  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
extern int   _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern int   _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp);
extern void  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        void *thiz, int kcl, void *mtx, int fix, short s, void *clps);
extern int   _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_[];

/* dBgW's two callback setters. Neither is in a decl_*.h; the InitResources
 * file declared both locally. */
extern void func_020393d4(void *thiz, int fn);
extern void func_020393c4(void *thiz, int fn);

/* Defined below, after the function that installs it. */
void func_ov045_021114a8(char *r0, char *r1);
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 8 -- daObjKm2_Agaru_c_classInit, 0x021114dc, size 0x30         */
/* ------------------------------------------------------------------------- */
/* Reconstructed source-style name: SM64DS proves daObjKm2_Agaru_c through its
 * ROM RTTI string, allocation size, vtable identity, and the KM2_AGARU
 * registry profile; later EAD lineage supplies classInit. Exact original
 * spelling is not preserved. Historical alias: daObjKm2_Agaru_c_Spawn. */
// @symbol daObjKm2_Agaru_c_classInit
extern "C" int *daObjKm2_Agaru_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(808);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        /* &[2], not the bare symbol: this TU EMITS the vtable, so mwcc's
         * symbol is the object start at 0x02112cec and +8 is what reaches the
         * 0x02112cf4 address point the ROM stores. The addend-0 spelling was
         * right only while the class merely IMPORTED its vtable. */
        p[0] = (int)&_ZTV16daObjKm2_Agaru_c[2];
    }
    return p;
}

/* ------------------------------------------------------------------------- */
/* g_profile_KM2_AGARU, 0x02112cd0, size 0x1c                                 */
/* ------------------------------------------------------------------------- */
/* .data objects emit in SOURCE order, so the descriptor is written directly
 * after the factory it names -- which is where the ROM's run puts it, between
 * _ZTS (0x02112cbc) and the vtable object (0x02112cec). */
struct Km2AgaruSpawnInfo {
    int *(*classInit)();
    s16 profileIDAndExecuteOrder;   /* 0x008c */
    s16 drawOrder;                  /* 0x00d3 */
    u32 actorFlags;
    s32 clipOffsetY;
    s32 clipRadius;
    s32 clipDistance;
    s32 farDistance;
};
typedef char Km2AgaruSpawnInfo_size_must_be_0x1c[
    sizeof(Km2AgaruSpawnInfo) == 0x1c ? 1 : -1];

// @symbol g_profile_KM2_AGARU
extern "C" Km2AgaruSpawnInfo g_profile_KM2_AGARU = {
    daObjKm2_Agaru_c_classInit, 0x008c, 0x00d3, 0x00000002,
    0x000fa000, 0x000fa000, 0x00fa0000, 0x00fa0000
};

/* ------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov045_021114c8, 0x021114c8, size 0x14               */
/* ------------------------------------------------------------------------- */
/* The dBgW "something touched me" callback InitResources installs through
 * func_020393c4. It forwards to the predicate below; the platform pointer it
 * is handed first is not used. */
extern "C" void func_ov045_021114c8(void *self, void *a, void *b)
{
    func_ov045_021114a8((char *)a, (char *)b);
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov045_021114a8, 0x021114a8, size 0x20               */
/* ------------------------------------------------------------------------- */
/* Sets the ridden flag at 0x326 when the thing that touched the collider is of
 * type 0xbf. Behavior clears the flag every frame, so it means "this frame". */
extern "C" void func_ov045_021114a8(char *r0, char *r1)
{
    int b = (*(unsigned short *)(r1 + 0xc) == 0xbf);
    if (b) *(bool *)(r0 + 0x326) = true;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 5 -- daObjKm2_Agaru_c::InitResources, 0x021113ec, size 0xbc    */
/* ------------------------------------------------------------------------- */
// @symbol _ZN16daObjKm2_Agaru_c13InitResourcesEv
s32 daObjKm2_Agaru_c::InitResources()
{
    char *c = (char *)this;
    int f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov045_02113188);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, f, 1, -1);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
    f = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov045_02113180);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124, f, c + 0x2ec, 0x199, mAngleY, data_ov045_021125d0);
    func_020393d4(c + 0x124,
        (int)_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020393c4(c + 0x124, (int)func_ov045_021114c8);
    *(short *)(c + 0x300 + 0x24) = 0;
    mState = 0;
    mRestY = mPosY;
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 4 -- daObjKm2_Agaru_c::Behavior, 0x0211129c, size 0x150        */
/* ------------------------------------------------------------------------- */
/* State 0 waits to be ridden, state 1 rises 0xa000 a frame to mRestY+0x5dc000,
 * state 2 sinks back to mRestY; mDelayTimer holds each end for 0x14 frames. */
// @symbol _ZN16daObjKm2_Agaru_c8BehaviorEv
s32 daObjKm2_Agaru_c::Behavior()
{
    char *c = (char *)this;
    switch (mState) {
    case 0:
        if (mRidden != 0)
            *(unsigned char *)(((int)c + 0x327)) =
                *(unsigned char *)(((int)c + 0x327)) + 1;
        break;
    case 1:
        if (mDelayTimer >= 0x14) {
            int lim;
            *(int *)(((int)c + 0x60)) =
                *(int *)(((int)c + 0x60)) + 0xa000;
            lim = mRestY + 0x5dc000;
            if (mPosY >= lim) {
                mPosY = lim;
                *(unsigned char *)(((int)c + 0x327)) =
                    *(unsigned char *)(((int)c + 0x327)) + 1;
                mDelayTimer = 0;
            }
        } else {
            *(unsigned short *)(((int)c + 0x324)) =
                *(unsigned short *)(((int)c + 0x324)) + 1;
        }
        break;
    case 2:
        if (mDelayTimer >= 0x14) {
            int lim;
            *(int *)(((int)c + 0x60)) =
                *(int *)(((int)c + 0x60)) - 0xa000;
            lim = mRestY;
            if (mPosY <= lim) {
                mPosY = lim;
                mState = 0;
                mDelayTimer = 0;
            }
        } else {
            *(unsigned short *)(((int)c + 0x324)) =
                *(unsigned short *)(((int)c + 0x324)) + 1;
        }
        break;
    }
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0x1f4000, 0))
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
    mRidden = 0;
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 3 -- daObjKm2_Agaru_c::Render, 0x02111274, size 0x28           */
/* ------------------------------------------------------------------------- */
/* The per-function file spelled this with two local shadow types -- a `struct
 * Base` of six virtuals and a `struct Derived { char pad[0xd4]; Base base; }`
 * -- which are dBgActor_c's Model at +0xd4 and its slot-5
 * Render(const Vector3 *). Reconciled onto the real member. */
// @symbol _ZN16daObjKm2_Agaru_c6RenderEv
s32 daObjKm2_Agaru_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 2 -- daObjKm2_Agaru_c::CleanupResources, 0x0211123c, size 0x38 */
/* ------------------------------------------------------------------------- */
// @symbol _ZN16daObjKm2_Agaru_c16CleanupResourcesEv
s32 daObjKm2_Agaru_c::CleanupResources()
{
    mMeshCollider.Disable();
    ((SharedFilePtr *)(data_ov045_02113188))->Release();
    ((SharedFilePtr *)(data_ov045_02113180))->Release();
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinals 1 and 0 -- one inline definition, two ROM-visible variants:   */
/*   _ZN16daObjKm2_Agaru_cD1Ev  0x021111a0  size 0x44  (complete-object dtor) */
/*   _ZN16daObjKm2_Agaru_cD0Ev  0x021111e4  size 0x58  (deleting destructor)  */
/* ------------------------------------------------------------------------- */
/* Defined in include/daObjKm2_Agaru_c.h's class body, and deliberately NOT
 * repeated out of line here: out of line mwccarm emits D2, D0, D1, and the
 * cartridge holds D1, D0 with no D2 at all. The body is two vtable stores and
 * the base subobject teardown -- this class adds no member with a destructor
 * of its own. D0 additionally returns the object to its heap through the
 * inline operator delete it inherits, which is why nothing mentions a heap. */

// @symbol _ZN16daObjKm2_Agaru_cD0Ev
// @symbol _ZN16daObjKm2_Agaru_cD1Ev
