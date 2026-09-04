//cpp
/* Production translation unit for ov036/daObjRc_Hane_c, hand-curated.
 * 7 function(s), .text 0x02111580..0x02111904.
 *
 * Rainbow Ride's flapping wing (profile RC_HANE): a decorative model rocked
 * about the actor's X and Y angles by a canned 64-entry table, with an
 * optional wingbeat sound every 0x40 frames. The whole actor is that table
 * lookup plus a matrix rebuild; it has no collision and no interaction.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov036 0x02113c00  "14daObjRc_Hane_c"
 *   _ZTI  ov036 0x02113bf4  __si_class_type_info; +8 -> _ZTI8dActor_c
 *                           (arm9 0x0208e390), so the DIRECT base is
 *                           dActor_c -- not dBgActor_c, unlike most of its
 *                           neighbours in this overlay.
 *   _ZTV  ov036 0x02113c38  the ADDRESS POINT itself: V-8 is a zero
 *                           offset-to-top, V-4 is &_ZTI, V+0 is slot 0
 *                           (InitResources, 0x02111854). The vptr store is
 *                           addend-0 against this symbol.
 *   size  0x11c             daObjRc_Hane_c_classInit's own literal (284).
 * The coined ShipWing alias that used to sit on this vtable is gone; the
 * ROM's own type string is where the class name now comes from.
 *
 * The table is 31 words, not the 32 its dBgActor_c-derived neighbours carry:
 * it ends at 0x02113cb4, which is already daObjRc_Tikuwa_c's typeinfo record.
 * That is exactly the length of _ZTV8dActor_c (arm9 0x0208e3a4..0x0208e420),
 * and it is the second, independent proof of the base. Only slots 0, 3, 6, 9,
 * 16 and 17 point inside ov036; the rest are dActor_c's own arm9 words. See
 * include/daObjRc_Hane_c.h for the slot-by-slot table.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder. The
 * destructor is not written here at all; it is inline in the header and so
 * emits last, which is where the cartridge has it.
 *
 * daObjRc_Hane_c_classInit (0x02111904) is NOT part of this entry and keeps
 * its own C file, the same arrangement daObjRc_Kaitendai_c and
 * daObjRcBuranko_c already landed with in this overlay.
 *
 * Consolidated from these legacy one-function sources (ROM address order):
 *   [0] 0x02111580  src/_ZN14daObjRc_Hane_cD1Ev.cpp
 *   [1] 0x021115b0  src/_ZN14daObjRc_Hane_cD0Ev.cpp
 *   [2] 0x021115f4  src/_ZN14daObjRc_Hane_c16CleanupResourcesEv.cpp
 *   [3] 0x02111618  src/func_ov036_02111618.c
 *   [4] 0x0211169c  src/_ZN14daObjRc_Hane_c6RenderEv.cpp
 *   [5] 0x021116c0  src/_ZN14daObjRc_Hane_c8BehaviorEv.cpp
 *   [6] 0x02111854  src/_ZN14daObjRc_Hane_c13InitResourcesEv.cpp
 */

/* decl_common.h is deliberately NOT included: it declares
 * data_ov036_02114070 as a bare char[], which contradicts the SharedFilePtr
 * this TU (and the cartridge) actually uses. Everything this file needs from
 * it is declared explicitly below. */
#include "types.h"
#include "daObjRc_Hane_c.h"
#include "Model.h"
#include "SharedFilePtr.h"

/* Externs: the union of the legacy files', kept at their legacy spelling.
 *
 * data_ov036_02114070 is a real SharedFilePtr in ov036 carrying no coined
 * alias, so the address-true name stands.
 *
 * data_02082214 is the canned motion table Behavior indexes: pairs of s16,
 * one pair per 16 units of the cycle. It is arm9 data with no shared header,
 * so it keeps the house spelling rather than inventing one.
 *
 * data_020a0e68 is the global scratch Matrix4x3 the Matrix4x3_* helpers
 * compose into; func_ov036_02111618 below copies the finished matrix out of
 * it. It is scratch, not state -- every caller in the game uses it the same
 * way, one call chain at a time.
 *
 * Sound::PlayBank3 keeps its mangled free spelling: it takes a Vector3 by
 * const reference, and an ordinary member call would let mwccarm re-home the
 * argument and change the ROM ABI.
 *
 * func_ov036_02111618 is this class's own private helper -- see its
 * definition below. It is a global symbol the cartridge calls with `this`
 * from Behavior, so it keeps C linkage and its existing name; renaming it
 * would move a symbol other config still names. */
/* MEASURED: the flat spelling of Matrix4x3 on purpose. include/common.h
 * spells this type `s32 m[12]`, while math/Matrix.h -- which Model.h drags in
 * here -- spells the same 0x30 bytes as `Matrix3x3 r; Vector3 t;`. The legacy
 * C body saw the flat one, and mwccarm scalarises the trailing struct copy
 * differently for the two, so reaching the nested spelling costs the match.
 * This TU therefore names the scratch matrix in the flat spelling directly
 * rather than depending on which header happened to be seen first. */
struct Mtx4x3Flat { s32 m[12]; };

extern SharedFilePtr data_ov036_02114070;   /* the wing's BMD */

extern "C" {
extern s16 data_02082214[];                 /* the canned rock table */
extern Mtx4x3Flat data_020a0e68;            /* global scratch matrix */

void func_ov036_02111618(char *self);
void Vec3_Asr(void *dst, void *src, int n);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(void *m, short ang);
void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
void Matrix4x3_ApplyInPlaceToRotationZ(void *m, short ang);

void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void *v);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN14daObjRc_Hane_c13InitResourcesEv, 0x02111854, size 0xb0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjRc_Hane_c13InitResourcesEv
s32 daObjRc_Hane_c::InitResources()
{
    BMD_File *file = (BMD_File *)Model::LoadFile(data_ov036_02114070);
    mModel.SetFile(file, 1, -1);

    /* Two independent bits of the spawn parameter: bit 0 mirrors the wing,
       bit 8 lets it make noise. A mirrored wing starts a quarter-turn tipped
       and half a turn around, so a mirrored pair reads as one pair of wings
       rather than two copies of the same one. */
    mReverseMotion = param1 & 1;
    mPlaySound = (param1 >> 8) & 1;
    if (mReverseMotion != 0) {
        mAngleX += 0x2400;
        mAngleY += 0x8000;
        mAngleZ += 0x8000;
    }

    /* Behavior rocks AROUND these, so they have to be the placed angles. */
    mBaseAngleX = mAngleX;
    mBaseAngleY = mAngleY;
    mBaseAngleZ = mAngleZ;
    mFrame = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN14daObjRc_Hane_c8BehaviorEv, 0x021116c0, size 0x194 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjRc_Hane_c8BehaviorEv
s32 daObjRc_Hane_c::Behavior()
{
    /* mFrame << 10 wraps the frame counter into a full 16-bit turn, and >> 4
       brings it back to the table's entries -- the round trip through s16 is
       what makes the cycle wrap for free. Each entry is a PAIR: [0] drives
       yaw (<< 13), [1] drives pitch (<< 11), so the wing sweeps twice as far
       sideways as it does up and down.

       The two branches differ only in the SIGN of the yaw term: a mirrored
       wing sweeps the other way, but still pitches up on the same beat. */
    s16 a = mFrame << 10;
    if (mReverseMotion == 0) {
        int idx = ((u16)a >> 4) * 2;
        mAngleY = mBaseAngleY + (int)((((s64)data_02082214[idx] << 13) + 0x800) >> 12);
        mAngleX = mBaseAngleX + (int)((((s64)data_02082214[idx + 1] << 11) + 0x800) >> 12);
    } else {
        int idx = ((u16)a >> 4) * 2;
        mAngleY = mBaseAngleY - (int)((((s64)data_02082214[idx] << 13) + 0x800) >> 12);
        mAngleX = mBaseAngleX + (int)((((s64)data_02082214[idx + 1] << 11) + 0x800) >> 12);
    }

    /* Through a pointer on purpose: read-modify-write straight on the member
       lets mwccarm CSE the field address, which costs an instruction the
       cartridge does not spend here. */
    {
        u16 *p = &mFrame;
        *p = *p + 1;
    }

    /* Only the sounding wing resets the counter, so a silent wing's frame
       runs the full 16 bits and wraps on its own -- the table only ever sees
       the low bits either way. */
    if (mPlaySound != 0) {
        if (mFrame == 0x40) {
            _ZN5Sound9PlayBank3EjRK7Vector3(0x75, &mCamSpacePosX);
            mFrame = 0;
        }
    }

    func_ov036_02111618(((char*)this));
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN14daObjRc_Hane_c6RenderEv, 0x0211169c, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjRc_Hane_c6RenderEv
s32 daObjRc_Hane_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov036_02111618, 0x02111618, size 0x84 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov036_02111618
/* This class's private "put the model where the actor is" helper, and the
 * reason the rocking is visible at all: it composes the actor's position and
 * three angles into the global scratch matrix, then copies the result into
 * mModel's own Matrix4x3 (mModel + 0xc, object offset 0xe0).
 *
 * The rotation order is Y, X, Z -- yaw first, then the pitch the table
 * drives, then roll.
 *
 * KEPT AT RAW OFFSETS ON PURPOSE. The cartridge gives it C linkage and a
 * global symbol -- it is called with `this` from Behavior above, and other
 * config still names it -- so it is not yet a member, and the offsets are what
 * the delinked body actually spells. The comments say what each one is;
 * promoting it to a real method is a separate change with its own codegen
 * risk. */
extern "C" {
void func_ov036_02111618(char *c)
{
    int v[3];

    /* actor position (0x5c) >> 3, into the scratch matrix's translation */
    Vec3_Asr(v, c + 0x5c, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);

    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short *)(c + 0x8e));
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short *)(c + 0x8c));
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(short *)(c + 0x90));

    /* ... and out to mModel.mat4x3 */
    *(Mtx4x3Flat *)(c + 0xe0) = data_020a0e68;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN14daObjRc_Hane_c16CleanupResourcesEv, 0x021115f4, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjRc_Hane_c16CleanupResourcesEv
s32 daObjRc_Hane_c::CleanupResources()
{
    data_ov036_02114070.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN14daObjRc_Hane_cD0Ev, 0x021115b0, size 0x44        */
/* ROM ordinal 0 -- _ZN14daObjRc_Hane_cD1Ev, 0x02111580, size 0x30        */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjRc_Hane_cD1Ev
// @symbol _ZN14daObjRc_Hane_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The destructor body is INLINE in
 * include/daObjRc_Hane_c.h and declared FIRST. Two measurements force that:
 *   - out of line, mwcc emits D0 ahead of D1 and the cartridge has D1 first,
 *     which rombuild refuses outright;
 *   - out of line it also emits the D2 base-object variant, which the ROM
 *     never carried.
 * Declaring it first is what makes this TU the vtable's home, so _ZTV and the
 * RTTI pair land here rather than in whichever other TU happens to name them.
 *
 * Both bodies are short because the chain is short: this class's vptr store,
 * then CommonModel's destructor, then dActor_c's. This class's own fields are
 * two s16 triples and three small integers, none of which has a destructor.
 * D0's trailing deallocation is the inherited inline operator delete, which is
 * why nothing here names a heap.
 */
