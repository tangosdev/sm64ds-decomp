//cpp
/* Production translation unit for ov036/daObjRc_Hane_c, hand-curated.
 * 8 function(s), .text 0x02111580..0x0211193c.
 *
 * Rainbow Ride's flapping wing (profile RC_HANE / SHIP_WING 125): a decorative
 * model rocked about the actor's X and Y angles by a canned 64-entry table,
 * with an optional wingbeat sound every 0x40 frames. The whole actor is that
 * table lookup plus a matrix rebuild; it has no collision and no interaction.
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
 * THE EIGHTH IS THE FACTORY. daObjRc_Hane_c_classInit (0x02111904, historical
 * alias daObjRc_Hane_c_Spawn) is the RC_HANE registry profile's spawn function
 * and sits immediately after InitResources in the ROM's own .text order, so it
 * is part of this TU. Written first here, being the highest-address member.
 *
 *
 * common.h FIRST: UpdateModelTransform assigns a whole Matrix4x3, and the ROM
 * copies it as three 4-word ldm/stm pairs. common.h's flat s32 m[12] is that
 * copy; math/Matrix.h's nested {Matrix3x3 r; Vector3 t;} splits it.
 *
 * Notes:
 * - common.h first (UpdateModelTransform's twelve-word Matrix4x3 copy)
 * - data_ov036_02114070 keeps its linker name (the BMD handle is
 *   constructed by this TU's static initializer, file 1702)
 * - g_profile_RC_HANE stays outside the licensed .text
 */

/* decl_common.h is deliberately NOT included: it declares
 * data_ov036_02114070 as a bare char[], which contradicts the SharedFilePtr
 * this TU (and the cartridge) actually uses. Everything this file needs from
 * it is declared explicitly below. */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_rc_hane_AssetId { ASSET_RC_HANE_BMD = 0x06a6 };

#include "common.h"
#include "daObjRc_Hane_c.h"
#include "dExtSimpleModel_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "math/MtxStack.h"
#include "nitro/fx/fx_trig.h"

/* 8-byte model handle. The constructor and destructor are the retail
 * func_02017acc / func_02017ab4 pair; the manifest aliases the generated
 * names onto those ROM symbols. */
struct RcHaneModelFilePtr : SharedFilePtr {
    RcHaneModelFilePtr(unsigned int fileID);
    ~RcHaneModelFilePtr();
};

typedef char RcHaneModelFilePtr_size_must_be_8[sizeof(RcHaneModelFilePtr) == 8 ? 1 : -1];

/* Externs: the union of the legacy files', kept at their legacy spelling.
 *
 * data_ov036_02114070 is a real SharedFilePtr in ov036 carrying no coined
 * alias, so the address-true name stands.
 *
 * data_020a0e68 (math/MtxStack.h) is the global scratch Matrix4x3 the
 * Matrix4x3_* helpers compose into; UpdateModelTransform below copies the
 * finished matrix out of it. */

extern RcHaneModelFilePtr data_ov036_02114070; /* the wing's BMD */

extern "C" {

void Vec3_Asr(void *dst, void *src, int n);
}


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* Reconstructed source-style name: SM64DS proves daObjRc_Hane_c through RTTI,
   allocation size, vtable identity, and the RC_HANE registry profile; later
   EAD lineage supplies classInit. Exact original spelling is not preserved.
   Historical alias: daObjRc_Hane_c_Spawn.

   Every instruction the cartridge has here falls out of the one `new`.
   sizeof is 0x11c; the implicit ctor calls dActor_c C2, stores this class's
   vptr, then dExtCommonModel_c C1 at 0xd4. The null check is the one `new` itself
   emits. */
extern "C" daObjRc_Hane_c *daObjRc_Hane_c_classInit() {
    return new daObjRc_Hane_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
s32 daObjRc_Hane_c::InitResources() {
    BMD_File *file = dExtSimpleModel_c::LoadFile(data_ov036_02114070);
    mModel.SetFile(file, 1, -1);

    /* Two independent bits of the spawn parameter: bit 0 mirrors the wing,
       bit 8 lets it make noise. A mirrored wing starts tipped by 0x2400
       (not a quarter turn; quarter = 0x4000) and half a turn around Y/Z,
       so a mirrored pair reads as one pair of wings rather than two copies
       of the same one. */
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
/* -------------------------------------------------------------------------- */
s32 daObjRc_Hane_c::Behavior() {
    /* mFrame << 10 wraps the frame counter into a full 16-bit turn, and >> 4
       brings it back to the table's entries -- the round trip through s16 is
       what makes the cycle wrap for free. The sine drives yaw (x 2.0), the
       cosine of the same angle pitch (x 0.5): 90° apart, and the yaw
       amplitude is 4× the pitch, not 2×.

       The two branches differ only in the SIGN of the yaw term: a mirrored
       wing sweeps the other way, but still pitches up on the same beat. */
    s16 a = mFrame << 10;
    if (mReverseMotion == 0) {
        mAngleY = mBaseAngleY + FX_MUL(FX_SinIdx((u16)a >> 4), 0x2000);
        mAngleX = mBaseAngleX + FX_MUL(FX_CosIdx((u16)a >> 4), 0x800);
    } else {
        mAngleY = mBaseAngleY - FX_MUL(FX_SinIdx((u16)a >> 4), 0x2000);
        mAngleX = mBaseAngleX + FX_MUL(FX_CosIdx((u16)a >> 4), 0x800);
    }

    mFrame++;

    /* Only the sounding wing resets the counter, so a silent wing's frame
       runs the full 16 bits and wraps on its own -- the table only ever sees
       the low bits either way. */
    if (mPlaySound != 0) {
        if (mFrame == 0x40) {
            Sound::PlayBank3(0x75, mCamSpacePos);
            mFrame = 0;
        }
    }

    UpdateModelTransform();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
s32 daObjRc_Hane_c::Render() {
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* This class's private "put the model where the actor is" helper, and the
 * reason the rocking is visible at all: it composes the actor's position and
 * three angles into the global scratch matrix, then copies the result into
 * mModel.mat4x3 (mModel + 0xc, object offset 0xe0).
 *
 * The rotation order is Y, X, Z -- yaw first, then the pitch the table
 * drives, then roll. */
void daObjRc_Hane_c::UpdateModelTransform() {
    int v[3];

    Vec3_Asr(v, &mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);

    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mAngleZ);

    mModel.mat4x3 = data_020a0e68;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
s32 daObjRc_Hane_c::CleanupResources() {
    data_ov036_02114070.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
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
 * then dExtCommonModel_c's destructor, then dActor_c's. This class's own fields are
 * two s16 triples and three small integers, none of which has a destructor.
 * D0's trailing deallocation is the inherited inline operator delete, which is
 * why nothing here names a heap.
 */

/* File 1702. The compiler registers func_02017ab4 beside the object. */
RcHaneModelFilePtr data_ov036_02114070(ASSET_RC_HANE_BMD);
