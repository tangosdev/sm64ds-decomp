//cpp
/* An ambient sound-effect emitter -- ov002/daSetSE_c.
 *
 * A GENUINE TRANSLATION UNIT, ENROLLED AND CANONICAL. It is the whole of the
 * cartridge's contiguous linker run .text 0x020f198c..0x020f1bc4, eight
 * functions and nothing else, and the production build links this object rather
 * than eight per-function ones. The filename is tools/tu_names.py's:
 * candidate_stem('daSetSE_c') is d_a_set_se.
 *
 * THE CLASS IS NAMED FROM THE CARTRIDGE, not coined. ov002 0x0210b470 is a
 * __si_class_type_info whose _ZTS at 0x0210b464 reads exactly `9daSetSE_c`, and
 * whose one base, at subobject offset 0, is arm9 0x0208e390 -- _ZTI8dActor_c.
 * That is why the header says `struct daSetSE_c : dActor_c` and why the RTTI
 * below has ROM homes to be compared against at all: a coined name is a
 * length-prefixed mangled string that matches nothing at any address, so it can
 * never be word-compared, and a class whose records cannot be compared cannot
 * have a key-function TU.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder.
 *
 * NAMING THE CLASS PUTS ITS VAGUE-LINKAGE DATA IN THIS OBJECT, because this TU
 * defines the key function. Nine records come out, and every one of them has a
 * cartridge home to be compared against; romdata_check compares each with
 * relocations applied before production isolation discards it:
 *
 *   _ZTV9daSetSE_c        ov002 0x0210b4c8   (the ADDRESS POINT -- the record
 *                                          itself starts 8 bytes earlier at
 *                                          0x0210b4c0 and is 0x84 bytes, the
 *                                          2-word header plus 31 slots, ending
 *                                          at _ZTI12daMugenBGM_c 0x0210b544;
 *                                          romdata_check subtracts the preamble
 *                                          for _ZTV*, so 0x0210b4c8 is the
 *                                          right thing to record here)
 *   _ZTI9daSetSE_c        ov002 0x0210b470
 *   _ZTS9daSetSE_c        ov002 0x0210b464
 *   _ZTI8dActor_c / _ZTI7dBase_c / _ZTI7fBase_c   arm9
 *   _ZTS8dActor_c / _ZTS7dBase_c / _ZTS7fBase_c   arm9
 *
 * A VTABLE CLAIM IS SCOPED TO THE SLOTS. A symbols.txt _ZTV address is the
 * ADDRESS POINT, eight bytes past the table's real start; the
 * {offset-to-top, _ZTI pointer} header pair is emitted here and word-compared by
 * nothing. The 31 slots were diffed against _ZTV8dActor_c by hand and differ in
 * exactly seven places -- 0, 3, 6, 9, 12, 16, 17 -- each holding the address of
 * the member the header names beside that slot.
 */

#include "daSetSE_c.h"
#include "Camera.h"

extern "C" {
extern int _ZTV9daSetSE_c[];
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN8dActor_cC2Ev(void *);
/* The mangled name decodes to (unsigned, unsigned, unsigned, const Vector3 &,
   short); the 4th and 5th parameters are deliberately spelled `void *` and
   `u32` here instead. Both spellings are ABI-identical on ARM -- a const
   reference is passed as the pointer this hands it, and a short argument is
   promoted to a full word -- so the codegen is the same, and the call site
   passes &mCamSpacePosX, the first component of the inherited Vector3, without
   needing that type declared. Worth tightening to the real spelling when
   Vector3 is reachable from this TU; it is a declaration divergence, not a
   disagreement about what the ROM calls. */
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 a, u32 b, u32 c, void *v, u32 e);
extern int data_0209b4ac;
extern int data_ov002_0210b498[];
extern int data_ov002_02110aec;
extern Camera *data_0209f318;
extern int IsStarCollectedInLevel(s8 levelID, int starID);
extern s8 data_0209f2f8;
extern u8 data_0209f220;
extern u8 data_0209f2d8;
extern int data_0209caa0[];
extern int data_0209fc48;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daSetSE_c_Spawn, 0x020f1b94, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol daSetSE_c_Spawn
extern "C" daSetSE_c *daSetSE_c_Spawn(void)
{
    daSetSE_c *p =
        (daSetSE_c *)_ZN7fBase_cnwEj(sizeof(daSetSE_c));
    if (p) {
        _ZN8dActor_cC2Ev(p);
        *(int *)p = (int)&_ZTV9daSetSE_c[2];
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN9daSetSE_c13InitResourcesEv, 0x020f1ac4, size 0xd0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daSetSE_c13InitResourcesEv
int daSetSE_c::InitResources()
{
    int flag;

    if (data_0209f2f8 == 8 && (data_0209f220 == 1 || IsStarCollectedInLevel(8, 1) == 0))
        return 0;

    /* Volatile keeps CW from materializing an extra base pointer for this
       inherited-field RMW; the ROM loads and stores [this + 0x08] directly. */
    u32 soundID = *(volatile u32 *)((char *)this + 0x8);
    *(volatile u32 *)((char *)this + 0x8) = soundID & 0xf;

    if ((int)(data_0209f2d8 == 0) != 0
        && (data_0209caa0[2] & 0x80) == 0
        && (int)(data_0209fc48 != 0) == 0)
        flag = 1;
    else
        flag = 0;

    data_ov002_02110aec = flag;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN9daSetSE_c8BehaviorEv, 0x020f19fc, size 0xc8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daSetSE_c8BehaviorEv
int daSetSE_c::Behavior()
{
    u32 param;
    int a;
    Camera *cam;

    if (data_ov002_02110aec != 0)
        return 1;

    param = param1;
    if (param >= 1 && param <= 4) {
        cam = data_0209f318;
        a = data_0209b4ac;
        if (a == 0x32 || a == 0x33 || a == 0x34 ||
            cam->IsUnderwater())
            return 1;
    }

    mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        mSoundHandle, 3, (u32)data_ov002_0210b498[param1],
        &mCamSpacePosX, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN9daSetSE_c6RenderEv, 0x020f19f4, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daSetSE_c6RenderEv
int daSetSE_c::Render()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN9daSetSE_c16OnPendingDestroyEv, 0x020f19f0, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daSetSE_c16OnPendingDestroyEv
void daSetSE_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN9daSetSE_c16CleanupResourcesEv, 0x020f19e8, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daSetSE_c16CleanupResourcesEv
int daSetSE_c::CleanupResources()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN9daSetSE_cD0Ev, 0x020f19b0, size 0x38 */
/* -------------------------------------------------------------------------- */
/* _ZN9daSetSE_cD0Ev (vtable slot 17, the deleting destructor) is NOT
 * hand-written here. A hand-written mangled D0 next to a real out-of-line D1
 * ICEs mwccarm 2004/b56 (ELFgen.c:483); the compiler synthesizes D0 itself
 * from D1. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN9daSetSE_cD1Ev, 0x020f198c, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daSetSE_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 * Vtable slot 16: one vptr store, then the tail into ~dActor_c.
 *
 * (no definition here: `virtual ~daSetSE_c() {}` is in include/daSetSE_c.h,
 * and that placement is load-bearing rather than stylistic -- out of line,
 * mwccarm emits D0 before D1 and adds a homeless D2, and objisolate then
 * refuses this whole TU. The header carries the reasoning and the leaf
 * measurement that makes it safe.) */
