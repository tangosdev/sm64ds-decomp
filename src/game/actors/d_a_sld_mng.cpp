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

/* The spawn position, ov019 0x021135d8. Nothing in the ROM names it. */
extern Vector3 data_ov019_021135d8;

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
