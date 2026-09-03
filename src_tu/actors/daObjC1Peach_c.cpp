//cpp
/* ov010/daObjC1Peach_c -- the Peach painting in the castle foyer, profile C1_PEACH.
 *
 * One translation unit for the whole class: .text 0x02111e10..0x0211203c and the
 * .data run 0x02112c24..0x02112ce4 that the class itself closes -- its RTTI
 * record, its type-name string, its profile record, and its vtable.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the reverse of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder.
 *
 * The destructor is NOT defined here. It is inline in the header, which is what
 * makes mwccarm emit the retail D1/D0 pair in retail order and no D2 at all; see
 * the header's own comment. The two comment blocks at the bottom carry the
 * `// @symbol` markers for those two ordinals so tiers.py charges their bytes to
 * the right members.
 */

#include "daObjC1Peach_c.h"
#include "SharedFilePtr.h"

extern "C" SharedFilePtr data_ov010_02112d64;

/* ------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daObjC1Peach_c_classInit, 0x02112004, size 0x38           */
/* ------------------------------------------------------------------------- */
// @symbol daObjC1Peach_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjC1Peach_c through RTTI,
 * allocation size, vtable identity, and the C1_PEACH registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: PeachPainting_Spawn. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void *_ZN7fBase_cnwEj(u32 size);
extern void _ZN8dActor_cC2Ev(void *self);
extern void _ZN5ModelC1Ev(void *self);
extern int _ZTV14daObjC1Peach_c[];

int *daObjC1Peach_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(296);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        /* &[2], not the bare symbol: this TU EMITS the vtable, so mwcc's symbol
         * is the object start and +8 is what reaches the address point. The
         * addend-0 spelling is right only for a TU that imports its vtable. */
        p[0] = (int)&_ZTV14daObjC1Peach_c[2];
        _ZN5ModelC1Ev((char *)p + 0xd4);
    }
    return p;
}
}

/* The actor profile record at 0x02112c44, seven words, word 0 relocating to the
 * factory above -- the only reference to it anywhere in the image, which is what
 * puts the record in this TU. Field names follow the dActor_c members that
 * SetRanges copies each one into. */
struct C1PeachSpawnInfo {
    int *(*classInit)();
    s16 profileIDAndExecuteOrder;   /* 0x0026 */
    s16 drawOrder;                  /* 0x0145 */
    u32 actorFlags;
    s32 clipOffsetY;
    s32 clipRadius;                 /* 0x1000 == 1.0 */
    s32 clipDistance;
    s32 farDistance;
};
typedef char C1PeachSpawnInfo_size_must_be_0x1c[
    sizeof(C1PeachSpawnInfo) == 0x1c ? 1 : -1];

// @symbol g_profile_C1_PEACH
extern "C" C1PeachSpawnInfo g_profile_C1_PEACH = {
    daObjC1Peach_c_classInit, 0x0026, 0x0145, 0, 0, 0x1000, 0, 0
};

/* ------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN14daObjC1Peach_c13InitResourcesEv, 0x02111fc0, 0x44    */
/* ------------------------------------------------------------------------- */
// @symbol _ZN14daObjC1Peach_c13InitResourcesEv
int daObjC1Peach_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov010_02112d64), 1, -1);
    mOpacity = 0xff;
    UpdateModelTransform();
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN14daObjC1Peach_c8BehaviorEv, 0x02111f28, size 0x98     */
/* ------------------------------------------------------------------------- */
// @symbol _ZN14daObjC1Peach_c8BehaviorEv
namespace cstd { int fdiv(int a, int b); }

/* The ROM call carries a third register argument even though the imported
 * mangled name records only the opacity. Keep that measured ABI view until
 * the declaration itself is repaired; the ordinary member call is shorter. */
extern "C" void _ZN9ModelBase12ApplyOpacityEj(void *, u32 opacity, int enable);

int daObjC1Peach_c::Behavior()
{
    int distance = DistToCPlayer();
    if (distance >= 0xe10000) {
        mOpacity = 0xff;
    } else if (distance <= 0xbf4000) {
        mOpacity = 0;
    } else {
        int fraction = cstd::fdiv(distance - 0xbf4000, 0x21c000);
        int opacity = (int)(((long long)fraction * 0xff + 0x800) >> 12);
        mOpacity = (u8)(opacity >> 3);
    }

    _ZN9ModelBase12ApplyOpacityEj(&mModel, mOpacity, 1);
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN14daObjC1Peach_c6RenderEv, 0x02111ee8, size 0x40       */
/* ------------------------------------------------------------------------- */
// @symbol _ZN14daObjC1Peach_c6RenderEv
int daObjC1Peach_c::Render()
{
    if (mOpacity == 0)
        return 1;

    mModel.Render(0);
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN14daObjC1Peach_c16CleanupResourcesEv, 0x02111ec4, 0x24 */
/* ------------------------------------------------------------------------- */
// @symbol _ZN14daObjC1Peach_c16CleanupResourcesEv
int daObjC1Peach_c::CleanupResources()
{
    data_ov010_02112d64.Release();
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN14daObjC1Peach_c20UpdateModelTransformEv, 0x02111e84   */
/* ------------------------------------------------------------------------- */
// @symbol _ZN14daObjC1Peach_c20UpdateModelTransformEv
extern "C" void Matrix4x3_FromRotationY(Matrix4x3 *, s32);

void daObjC1Peach_c::UpdateModelTransform()
{
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN14daObjC1Peach_cD0Ev, 0x02111e40, size 0x44           */
/* ------------------------------------------------------------------------- */
// @symbol _ZN14daObjC1Peach_cD0Ev
/* No body here on purpose: the inline `virtual ~daObjC1Peach_c() {}` in the
 * header emits this deleting variant. */

/* ------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN14daObjC1Peach_cD1Ev, 0x02111e10, size 0x30           */
/* ------------------------------------------------------------------------- */
// @symbol _ZN14daObjC1Peach_cD1Ev
/* No body here on purpose: emitted from the header's inline destructor, which
 * is also what puts it BELOW D0 in the object, the way retail has it. */
