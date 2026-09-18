//cpp
/**
 * Bowser in the Fire Sea's moving arena floor (ov060, KOOPA2BG).
 *
 * A dBgActor_c with a SECOND model and a SECOND moving-mesh collider of its
 * own on top of the base's pair. Behavior spins it by three per-axis angular
 * speeds, re-derives the collider matrix from the actor's position and two of
 * those angles, and writes a second, eighth-scale copy of the same transform
 * into the model. The sound helper keeps one looping handle alive in unk_56c.
 *
 * mwccarm lays .text down in reverse source order, so the definitions below
 * run from the highest ROM address back toward the destructor pair. Do not
 * reorder. The D1/D0 pair itself is unwritten: the destructor is inline in
 * the class body, which emits D1 (0x02117980) then D0 (0x021179d4) -- the
 * cartridge's own order -- and no homeless D2.
 *
 * common.h comes FIRST, and that is load-bearing. Matrix4x3 has two 0x30-byte
 * spellings behind one guard and whichever a TU sees first stands; the two
 * matrix helpers below were written against common.h's flat s32 m[12] and
 * whole-struct-assign it. Put daKpa2Bg_c.h first instead and the OTHER spelling
 * wins -- it reaches math/Matrix.h's .r/.t form through Model.h before it
 * reaches common.h through dBgActor_c.h -- and the two whole-struct assignments
 * grow func_ov060_02117a64 and _02117ae0 by five words each (measured: 0x90 and
 * 0x94 emitted against the ROM's 0x7c and 0x78).
 *
 * PARTIAL PROMOTION. Eight of this class's ten functions are licensed here, the
 * contiguous run 0x02117980..0x02117c30. InitResources (0x02117c30) and
 * daKpa2Bg_c_classInit (0x02117cdc) stay in their own enrolled shards and are
 * NOT written here; the block where they would go says why, and the manifest
 * entry carries the measurements. The short form: one relocation, not a byte.
 *
 * deslop
 * Leftover: func_ov060_02117a3c / _02117a64 / _02117ae0 keep ROM labels and
 *   C linkage. They are this class's own helpers -- nothing outside ov060
 *   calls them -- but naming them is a rename PR, not this one.
 * Leftover: Sound_PlayIfNotActive's handle is kept in unk_56c, spelled s32
 *   in the header and passed as the int the function's own definition takes;
 *   the field's real type is not recovered.
 * Leftover: _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s stays mangled. dBgW_KcMbg.h
 *   declares ::Transform, but the definition is still a free extern-C one, so a
 *   member call would not resolve to it.
 * Leftover: Matrix4x3_FromTranslation's parameter is spelled `struct Mtx43 *`
 *   because that is what its definition spells; it is the same 0x30 bytes as
 *   Matrix4x3. Settling the two is a separate change.
 * Leftover: g_profile_KOOPA2BG lives outside this TU.
 */

#include "common.h"
#include "daKpa2Bg_c.h"
#include "SharedFilePtr.h"

/* The translation-unit-local struct src/Matrix4x3_FromTranslation.c defines its
   parameter with. Spelled here exactly as the definition spells it so
   tools/check_decl_agreement.py sees one interface and not two; it is the same
   0x30 bytes as Matrix4x3, which is what the rest of this file uses. */
struct Mtx43;

/* --------------------------------------------------------------------------
 * The one file-scope extern "C" region, above the first @symbol marker so no
 * member is charged with these mangled spellings.
 * ------------------------------------------------------------------------ */
extern "C" {

/* ov060 .bss, 0x0211affc and 0x0211aff4. */
extern SharedFilePtr daKpa2Bg_c_ModelFile;
extern SharedFilePtr daKpa2Bg_c_ClsnFile;

/* arm9 0x020a0e68 -- the engine's scratch Matrix4x3 both helpers build in. */
extern Matrix4x3 data_020a0e68;

int Sound_PlayIfNotActive(int handle, int a, int b, int c);
void Matrix4x3_FromTranslation(struct Mtx43 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *m, s16 ang);
void Matrix4x3_ApplyInPlaceToRotationZ(Matrix4x3 *m, s16 ang);
void Vec3_Asr(void *dst, void *src, int shift);

/* This TU's own three helpers, declared ahead of their callers. */
void func_ov060_02117a3c(char *self);
void func_ov060_02117a64(char *self);
void func_ov060_02117ae0(char *self);

/* dBgW_KcMbg::Transform -- see the leftovers above. */
void _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(
    dBgW_KcMbg *self, const Matrix4x3 *mat, s16 angY);

}

/* --------------------------------------------------------------------------
 * ABOVE THE LICENSED BOUNDARY, AND DELIBERATELY NOT WRITTEN HERE.
 *
 *   _ZN10daKpa2Bg_c13InitResourcesEv   0x02117c30   src/_ZN10daKpa2Bg_c13InitResourcesEv.cpp
 *   daKpa2Bg_c_classInit               0x02117cdc   src/d_a_kpa2_bg.c
 *
 * Both were written here first and both reproduced their cartridge bytes from
 * this file (tools/match.py, 2004/b56: MATCH). They are still not here, for two
 * different reasons.
 *
 * InitResources cannot be licensed: its literal pool loads the CLPS block at
 * 0x021115bc, ov060/relocs.txt leaves that cross-overlay load ambiguous across
 * twenty modules, no symbols.txt names the address, and its delinks.txt entry is
 * therefore the one entry in this run without `complete`. daKpa2Bg_c_classInit
 * sits ABOVE it at 0x02117cdc, so no contiguous complete span reaches the factory
 * without crossing it.
 *
 * Carrying unlicensed copies is what fails, not writing them. tools/objisolate.py
 * only deadstrips a duplicate body whose symbol has VAGUE linkage (STB_LOPROC);
 * an ordinary out-of-line member or a free function is STB_GLOBAL, and the build
 * refuses it -- "duplicate deadstrip needs vague linkage, got STB_GLOBAL". That is
 * why the destructor pair below CAN stay (an in-class body is vague-linkage) and
 * these two cannot. Both shards stay enrolled and keep supplying their addresses.
 *
 * Nothing is lost by their absence. With the destructor inline, this class's key
 * function is Behavior, so this is still the translation unit that emits
 * _ZTV10daKpa2Bg_c and both destructor variants -- exactly the bytes the licensed
 * run has to produce.
 * ------------------------------------------------------------------------ */

// @symbol _ZN10daKpa2Bg_c8BehaviorEv
s32 daKpa2Bg_c::Behavior()
{
    /* mAngleX / mAngleY / mAngleZ, reached through rematerialised pointers.
       The plain member form (`mAngleX = mAngleX + mAngleXSpeed;`) is five
       words SHORTER than the ROM here -- measured, 0x54 against 0x68 -- so
       the three stores stay spelled the way the cartridge computes them. */
    s16 *angX = (s16 *)(((int)((char *)this) + 0x8c));
    s16 *angY = (s16 *)(((int)((char *)this) + 0x8e));

    *angX = *angX + mAngleXSpeed;
    *angY = *angY + mAngleYSpeed;

    {
        s16 *angZ = (s16 *)(((int)((char *)this) + 0x90));
        *angZ = *angZ + mAngleZSpeed;
    }

    func_ov060_02117ae0((char *)this);
    func_ov060_02117a64((char *)this);
    return 1;
}

// @symbol _ZN10daKpa2Bg_c6RenderEv
s32 daKpa2Bg_c::Render()
{
    mModel2.Render(0);
    return 1;
}

// @symbol _ZN10daKpa2Bg_c16CleanupResourcesEv
s32 daKpa2Bg_c::CleanupResources()
{
    if (mMovingMeshCollider2.IsEnabled()) {
        mMovingMeshCollider2.Disable();
    }
    daKpa2Bg_c_ModelFile.Release();
    daKpa2Bg_c_ClsnFile.Release();
    return 1;
}

extern "C" {

/* The model's own transform: the same build as func_ov060_02117a64 but at an
   eighth of the actor's position, written to mModel2's matrix (0x340). */
// @symbol func_ov060_02117ae0
void func_ov060_02117ae0(char *self)
{
    int pos[4];
    Vec3_Asr(&pos, self + 0x5c, 3);
    Matrix4x3_FromTranslation((struct Mtx43 *)&data_020a0e68, pos[0], pos[1],
                              pos[2]);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16 *)(self + 0x8c));
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(s16 *)(self + 0x90));
    *(Matrix4x3 *)(self + 0x340) = data_020a0e68;
}

/* The collider's transform: position, then X and Z rotation, into mClsnMat
   (0x2ec), then handed to the second collider with the Y angle. */
// @symbol func_ov060_02117a64
void func_ov060_02117a64(char *self)
{
    Matrix4x3_FromTranslation((struct Mtx43 *)&data_020a0e68,
                              *(int *)(self + 0x5c), *(int *)(self + 0x60),
                              *(int *)(self + 0x64));
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16 *)(self + 0x8c));
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(s16 *)(self + 0x90));
    *(Matrix4x3 *)(self + 0x2ec) = data_020a0e68;
    _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(
        (dBgW_KcMbg *)(self + 0x374), (Matrix4x3 *)(self + 0x2ec),
        *(s16 *)(self + 0x8e));
}

/* Keeps one looping sound handle alive in unk_56c. */
// @symbol func_ov060_02117a3c
void func_ov060_02117a3c(char *self)
{
    *(int *)(self + 0x56c) =
        Sound_PlayIfNotActive(*(int *)(self + 0x56c), 3, 0x95, 0);
}

}

// @symbol _ZN10daKpa2Bg_cD1Ev
// @symbol _ZN10daKpa2Bg_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header emits D1
   then D0 -- the cartridge's order, 0x02117980 then 0x021179d4 -- and no D2. */
