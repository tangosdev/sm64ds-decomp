//cpp
/* Warp trigger volume (WARPKUN) -- ov002/daWarpkun_c.
 *
 * An invisible trigger: while a Player overlaps its cylinder, Behavior fires
 * WarpPlayer for that player's number and the destination carried in the
 * spawn param, then latches mAngleY until the cylinder empties. mAngleY is
 * that warp-in-progress latch here, not an angle.
 *
 * param1 (spawn param) layout, read by InitResources and Behavior:
 *   bits 0-3   trigger radius step; radius is (n + 1) * 64 units
 *   bits 4-7   trigger height step, same scale
 *   bits 12+   warp destination step; WarpPlayer gets ((param1 >> 12) + 1) & 0xff
 *
 * deslop
 * Leftover: SetRanges / dCcAc_c::Init stay TU-local mangled scalar wrappers
 *   (Fix12i): dActor_c.h documents SetRanges but declares no member -- a
 *   typed extern "C" there is overload poison (S13) -- and dCcAc_c::Init's
 *   header form takes Fix12<int> by value (6az).
 * Leftover: g_profile_WARPKUN stays outside the TU (S14): this entry owns
 *   .text only.
 * Leftover: Behavior's widened `isPlayer` bool: folding it into the if
 *   above changes the function size and DIFFs (S8, measured in this TU).
 */

#include "daWarpkun_c.h"
#include "Player.h"
#include "decl_common.h"

/* actorID 0xbf is the Player: Behavior casts the hit actor to Player * and
   reads mPlayerNo / CanWarp off it. No shared header names the id. */
static const u16 kPlayerActorID = 0xbf;

/* The one file-scope extern "C" region. IsPlayerWarping / WarpPlayer come
   from decl_common.h; these two wrappers stay TU-local per S13. */
extern "C" {
extern int _ZTV11daWarpkun_c[];
extern void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(
    dActor_c *actor, Fix12i offsetY, Fix12i radius,
    Fix12i clipDistance, Fix12i farDistance);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    dCcAc_c *clsn, dActor_c *actor, Fix12i radius, Fix12i height,
    u32 flags, u32 vulnFlags);
}

extern "C" {

/* ROM ordinal 7 -- actor-table C ABI factory. */
// @symbol daWarpkun_c_classInit
daWarpkun_c *daWarpkun_c_classInit()
{
    return new daWarpkun_c();
}

}

/* ROM ordinal 6 -- key function: sizes the clip volume and the trigger
   cylinder from the spawn param's low two nibbles. Init's flags are 2
   (dCc_c.h documents only bit 0x1) and vulnFlags 0x400000 (the player bit
   on dCc_c.h's bit-table reading). */
// @symbol _ZN11daWarpkun_c13InitResourcesEv
s32 daWarpkun_c::InitResources()
{
    /* Two textual loads of param1 (here and in radius) CSE into the ROM's
       single ldr; routing radius through spawnParam swaps the register
       assignment (r5/r6) and DIFFs. height reads the local because the
       SetRanges call below may clobber this->param1. */
    u32 spawnParam = param1;
    Fix12i radius = ((param1 & 0xf) + 1) << 0x12;
    _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(
        this, 0, radius, radius + 0x7d0000, 0);
    Fix12i height = (((spawnParam >> 4) & 0xf) + 1) << 0x12;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mdCcAc_c, this, radius, height, 2, 0x400000);
    return 1;
}

/* ROM ordinal 5 -- vtable slot 6. `mAngleY == 0' gates the warp; the found
   actor's actorID/mPlayerNo are real Player members once cast. */
// @symbol _ZN11daWarpkun_c8BehaviorEv
s32 daWarpkun_c::Behavior()
{
    u32 hitId = mdCcAc_c.otherOwner;
    if (hitId != 0) {
        if (mAngleY == 0) {
            dActor_c *hit = dActor_c::FindWithID(hitId);
            if (hit != 0) {
                /* Widened bool: folding this into the if above changes
                   Behavior's size and DIFFs (S8). */
                int isPlayer = (hit->actorID == kPlayerActorID) ? 1 : 0;
                if (isPlayer != 0) {
                    Player *player = (Player *)hit;
                    if (IsPlayerWarping(player->mPlayerNo) != 0) {
                        mAngleY = 1;
                    } else if (player->CanWarp()) {
                        u32 warpParam = (param1 >> 0xc) + 1;
                        WarpPlayer(player->mPlayerNo, warpParam & 0xff);
                    }
                }
            }
        }
    } else {
        mAngleY = 0;
    }

    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

/* ROM ordinal 4 -- vtable slot 9. */
// @symbol _ZN11daWarpkun_c6RenderEv
s32 daWarpkun_c::Render()
{
    return 1;
}

/* ROM ordinal 3 -- vtable slot 12. */
// @symbol _ZN11daWarpkun_c16OnPendingDestroyEv
void daWarpkun_c::OnPendingDestroy()
{
}

/* ROM ordinal 2 -- vtable slot 3. */
// @symbol _ZN11daWarpkun_c16CleanupResourcesEv
s32 daWarpkun_c::CleanupResources()
{
    return 1;
}

/* ROM ordinal 1 -- the deleting destructor, vtable slot 17. No definition
   here: the inline ~daWarpkun_c() in the header makes the compiler
   synthesize D0 itself, byte-identical to the ROM. */
// @symbol _ZN11daWarpkun_cD0Ev

/* ROM ordinal 0 -- vtable slot 16. No definition here either: the inline
   destructor in include/daWarpkun_c.h emits D1 (see the header for why that
   placement is load-bearing). */
// @symbol _ZN11daWarpkun_cD1Ev
