//cpp
/* Production translation unit for ov017/daObjKsWater_c, hand-curated.
 * 7 function(s), .text 0x021111a0..0x021114b8.
 *
 * The rising water inside the sunken ship (profile KS_MIZU). It holds still
 * while any treasure chest in the room is shut, then climbs 0x5000 a frame,
 * with a looping sound, until it has risen 0x92e000 from where it started.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov017 0x02111bc0  "14daObjKsWater_c"
 *   _ZTI  ov017 0x02111bb4  __si_class_type_info; +8 -> _ZTI10dBgActor_c
 *                           (ov002 0x021089ec), so the DIRECT base is
 *                           dBgActor_c and nothing else.
 *   _ZTV  ov017 0x02111bf8  the ADDRESS POINT itself; the preamble below it at
 *                           0x02111bf0 is a zero offset-to-top and &_ZTI.
 *   size  0x340             daObjKsWater_c_classInit's own literal (832).
 * The coined ShipWater alias that used to sit on this vtable is gone; the ROM's
 * own type string is where the class name now comes from.
 *
 * Only slots 0, 3, 6, 9, 16 and 17 of the cartridge's 34-word table point
 * inside ov017; every other slot still holds dBgActor_c's arm9 word, so
 * nothing else is overridden.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder. The
 * destructor is not written here at all; it is inline in the header and so
 * emits last, which is where the cartridge has it.
 *
 * Consolidated from these legacy one-function sources (ROM address order):
 *   [0] 0x021111a0  src/_ZN14daObjKsWater_cD1Ev.cpp
 *   [1] 0x021111ec  src/_ZN14daObjKsWater_cD0Ev.cpp
 *   [2] 0x0211124c  src/_ZN14daObjKsWater_c16CleanupResourcesEv.cpp
 *   [3] 0x02111290  src/_ZN14daObjKsWater_c6RenderEv.cpp
 *   [4] 0x021112c4  src/_ZN14daObjKsWater_c8BehaviorEv.cpp
 *   [5] 0x021113c0  src/_ZN14daObjKsWater_c13InitResourcesEv.cpp
 *   [6] 0x02111480  src/daObjKsWater_c_classInit.c
 */

#include "daObjKsWater_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "Model.h"

/* Externs: the union of the legacy files', kept at their legacy spelling.
 *
 * decl_common.h is NOT included anywhere in this file, and cannot be: it
 * declares data_ov017_02111c88 as an int array, and this TU needs it at its
 * real SharedFilePtr type to call Release() on it. The one declaration the
 * factory block below would otherwise have taken from it -- the vtable array --
 * is restated here instead.
 *
 * Sound::PlayLong has no header declaration yet (Sound.h keeps only PlayBank3
 * and Player::SetPlayableSeqCount so far), and the position it takes by
 * const-reference is dActor_c's mCamSpacePosX/Y/Z -- three separate s32
 * fields, not a Vector3 -- so a real call still needs a reinterpret at the
 * call site either way.
 *
 * The two SetFile calls keep their mangled free-function spelling: both take
 * Fix12<int> by value, and a by-value class parameter changes mwccarm's
 * argument homing, so an ordinary member call would move the ROM ABI. */
extern "C" {
extern int _ZTV14daObjKsWater_c[];

extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b,
                                             unsigned int c, void *pos,
                                             unsigned int e);

extern SharedFilePtr data_ov017_02111c80;   /* the collision KCL handle */
extern SharedFilePtr data_ov017_02111c88;   /* the water model's BMD handle */
extern BTA_File      data_ov017_02111a60;   /* its scrolling texture animation */
extern CLPS_Block    data_ov017_02111a94;

void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
    TextureTransformer *self, BTA_File *file, int flags, int speed, u32 startFrame);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat, int scale, s16 angY,
    CLPS_Block *clps);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- daObjKsWater_c_classInit, 0x02111480, size 0x38 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol daObjKsWater_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_TextureTransformer.h"
/* resolved: VT0 = _ZTV14daObjKsWater_c */
/* Reconstructed source-style name. Historical alias: ShipWater_Spawn. */
int *daObjKsWater_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(832);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        /* This TU defines every virtual of the class, so mwccarm emits the
           vtable HERE and its _ZTV names the emitted OBJECT -- the two-word
           Itanium preamble at 0x02111bf0 -- not the address point. The [2]
           steps over the preamble onto the address point the cartridge
           actually stores, 0x02111bf8. */
        p[0] = (int)&_ZTV14daObjKsWater_c[2];
        /* The TextureTransformer at 0x320 is the one member with a constructor
           of its own; the cartridge runs it here, after the base, rather than
           through a member-init list. */
        _ZN18TextureTransformerC1Ev((char *)p + 0x320);
    }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN14daObjKsWater_c13InitResourcesEv, 0x021113c0, size 0xc0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjKsWater_c13InitResourcesEv
/* Load the water model and its BTA texture animation, load and place the mesh
 * collider, and remember the starting surface height so Behavior has something
 * to measure the rise against.
 *
 * THE COLLISION FILE'S HANDLE WAS NAMED FROM THE WRONG OVERLAY. The legacy C
 * form passed _ZN18TiltingPlatformBfsD0Ev, an ov045 FUNCTION that happens to
 * sit at 0x02111c80 -- the same address as ov017's own data_ov017_02111c80,
 * which is what this actually is. Overlays share address space, so both names
 * relocate to the same word and the byte gate could not tell them apart (see
 * "Reloc slots are wildcards" in notes/mwccarm-codegen.md). Named correctly here.
 *
 * The + 4 on the model handle is SharedFilePtr's loaded-file pointer.
 * include/SharedFilePtr.h declares no fields on purpose -- the layout is not
 * recovered and the files that spell it locally disagree -- so the offset stays
 * raw and says what it is rather than inventing a member. */
int daObjKsWater_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov017_02111c88), 1, -1);
    TextureTransformer::Prepare(**(BMD_File **)((char *)&data_ov017_02111c88 + 4),
                                data_ov017_02111a60);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
        &mTextureTransformer, &data_ov017_02111a60, 0, 0x1000, 0);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)dBgW_Kc::LoadFile(data_ov017_02111c80),
        &mClsnMat, 0x1000, mAngleY, &data_ov017_02111a94);
    mMeshCollider.Enable(this);
    mOriginalPosY = mPosY;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN14daObjKsWater_c8BehaviorEv, 0x021112c4, size 0xfc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjKsWater_c8BehaviorEv
/* Two states, latched one way only.
 *
 * While mChestsOpen is clear the actor walks every actor-ID-0xd chest in the
 * room each frame and demands that all of them be in state 1 or 2; one shut
 * chest anywhere clears the flag for that frame. Once they all agree, the flag
 * latches and is never cleared again.
 *
 * After that the water climbs 0x5000 a frame -- and re-issues the looping
 * sound, handing the previous handle back so it is a continuation rather than
 * a new voice -- until it has moved 0x92e000 from mOriginalPosY, then stops. */
int daObjKsWater_c::Behavior()
{
    if (mChestsOpen == 0) {
        int ok = 1;
        dActor_c *p = dActor_c::FindWithActorID(0xd, 0);
        while (p != 0) {
            /* offset into the chest actor's own (unrecovered) type -- ID 0xd is
             * not a class any of this file's headers know about */
            int state = *(int *)((char *)p + 0x16c);
            int cond = 1;
            if (state != 1) {
                if (state != 2) cond = 0;
            }
            if (cond == 0) ok = 0;
            p = dActor_c::FindWithActorID(0xd, p);
        }
        if (ok != 0) mChestsOpen = 1;
    } else {
        int d = mOriginalPosY - mPosY;
        if (d < 0) d = -d;
        if (d < 0x92e000) {
            int *q;
            mSoundID = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                mSoundID, 3, 0x96, ((char *)this) + 0x74, 0);
            q = (int *)((char *)&mPosY);
            *q -= 0x5000;
        }
    }
    mTextureTransformer.speed = 0x1000;
    mTextureTransformer.Advance();
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN14daObjKsWater_c6RenderEv, 0x02111290, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjKsWater_c6RenderEv
int daObjKsWater_c::Render()
{
    mTextureTransformer.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN14daObjKsWater_c16CleanupResourcesEv, 0x0211124c, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjKsWater_c16CleanupResourcesEv
int daObjKsWater_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov017_02111c88.Release();
    data_ov017_02111c80.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN14daObjKsWater_cD0Ev, 0x021111ec, size 0x60          */
/* ROM ordinal 0 -- _ZN14daObjKsWater_cD1Ev, 0x021111a0, size 0x4c          */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjKsWater_cD1Ev
// @symbol _ZN14daObjKsWater_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The destructor body is INLINE in
 * include/daObjKsWater_c.h and declared FIRST. Two measurements force that:
 *   - out of line, mwcc emits D0 ahead of D1 and the cartridge has D1 first,
 *     which rombuild refuses outright;
 *   - out of line it also emits the D2 base-object variant, which the ROM
 *     never carried.
 * Declaring it first is what makes this TU the vtable's home, so _ZTV and the
 * RTTI pair land here rather than in whichever other TU happens to name them.
 *
 * Two vtable stores and three destructor calls, every one a consequence of
 * `struct daObjKsWater_c : dBgActor_c`: its own vptr, then dBgActor_c's --
 * inlined, its destructor is defined in its class body -- then dBgActor_c's
 * Model at 0xd4 and dBgW_KcMbg at 0x124, then dActor_c. The four fields this
 * class adds have no destructors of their own. D0's trailing deallocation is
 * the inherited inline operator delete, which is why nothing here names a heap.
 */
