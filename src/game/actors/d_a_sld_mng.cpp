//cpp
/* Reconstructed ov019/daSldMng_c translation unit.
 *
 * Ice slide manager. RTTI ov019:0x0211338c names daSldMng_c; overlay_actors
 * maps ov019 ICE_SLIDE_MANAGER (356). ov019 also has RACING_PENGUIN -- this
 * TU is the slide manager.
 *
 * mwccarm emits ordinary function sections in reverse source order. Keep the
 * factory first. The inline destructor declared last in daSldMng_c emits the
 * retail D1/D0 pair first and emits no D2 body.
 *
 * deslop
 * Leftover: Sound::PlaySub stays mangled (Fix12<int> by value, wall 6az;
 *   this TU's Behavior measured 0xa4 -> 0xb4 in method form).
 * Leftover: data_ov019_021135d8 keeps its ROM-address name (the Vector3
 *   spawn position; nothing in the ROM names it).
 * Leftover: g_profile_SLIDER_MANAGER (ov019 0x021133a8) stays outside this
 *   TU (S14) -- defining it here would emit .data this entry is not
 *   licensed to own.
 * Leftover: unk_0d0 (header) has no observed meaning; pad_0d7 is never
 *   dereferenced.
 */

#include "daSldMng_c.h"

extern "C" {
extern u16 DecIfAbove0_Short(u16 *timer);
/* ABI wall: spelling this as Sound::PlaySub(..., Fix12<int>, bool) makes
 * mwccarm home the by-value Fix12 in an 8-byte stack slot, growing Behavior
 * from 0xa4 to 0xb8. The cartridge passes the same raw fixed-point word in
 * r3, so the measured scalar call view is retained. */
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(
    u32 soundID, u32 volume, u32 pan, Fix12i distance, bool loop);
}

/* The spawn position, ov019 0x021135d8. Nothing in the ROM names it.
 * The cartridge's initializer stores the three fields and registers
 * _ZN7Vector3D1Ev, so the object sits in .bss and mwcc's
 * __sinit_d_a_sld_mng.cpp emits the stores. The wrapper is local; the
 * manifest aliases its destructor to the ROM's _ZN7Vector3D1Ev. */
struct SldMngVec3 : Vector3 {
    SldMngVec3(Fix12i a, Fix12i b, Fix12i c) { x = a; y = b; z = c; }
    ~SldMngVec3();
};

SldMngVec3 data_ov019_021135d8(-0x12a0000, 0x700000, -0x200000);

/* Reconstructed source-style name. SM64DS proves daSldMng_c through RTTI,
 * allocation size, vtable identity, and the SLIDER_MANAGER registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: daSldMng_c_Spawn. */
// @symbol daSldMng_c_classInit
extern "C" daSldMng_c *daSldMng_c_classInit()
{
    return new daSldMng_c();
}

// @symbol _ZN10daSldMng_c13InitResourcesEv
int daSldMng_c::InitResources()
{
    mPosX = data_ov019_021135d8.x;
    mPosY = data_ov019_021135d8.y;
    mPosZ = data_ov019_021135d8.z;
    mKillTimer = 0x78;
    return 1;
}

// @symbol _ZN10daSldMng_c8BehaviorEv
int daSldMng_c::Behavior()
{
    switch (mState) {
    case 0:
        if (DistToCPlayer() < 0x180000) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(
                0x20, 0x14, 0x7f, 0x15666, false);
            ++mState;
        }
        break;
    case 1:
        if (DecIfAbove0_Short(&mKillTimer) == 0) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(
                0x20, 0x7f, 0, 0x15666, false);
            KillAndTrackInDeathTable();
        }
        break;
    }
    return 1;
}
// @symbol _ZN10daSldMng_cD1Ev
// @symbol _ZN10daSldMng_cD0Ev
/* -------------------------------------------------------------------------- */
/* ROM ordinals 0 and 1 -- _ZN10daSldMng_cD1Ev 0x0211261c size 0x24 and       */
/* _ZN10daSldMng_cD0Ev 0x02112640 size 0x38 -- are NOT written here.         */
/*                                                                            */
/* The destructor is defined INLINE in include/daSldMng_c.h. Written           */
/* out-of-line here the real destructor makes mwccarm emit D0 BEFORE D1, the   */
/* reverse of the cartridge's order, which objisolate refuses for the whole    */
/* translation unit, and it emits a third D2 body with no ROM home. The inline */
/* definition gives the retail D1/D0 pair in ROM order and no D2, while        */
/* InitResources -- declared out-of-line above and first in the class body --  */
/* keeps this TU as the class's key-function TU, so it still owns the complete */
/* _ZTV/_ZTI/_ZTS group declared in this entry's compiler_only_output. D0 is   */
/* that destructor plus dActor_c's inherited inline `operator delete`; slot 17 */
/* is the deleting variant.                                                    */
/*                                                                            */
/* The body is genuinely empty: the class adds no member with a destructor to  */
/* dActor_c, so the ROM's D1 is a vptr store and the base chain, nothing more. */
/* -------------------------------------------------------------------------- */
