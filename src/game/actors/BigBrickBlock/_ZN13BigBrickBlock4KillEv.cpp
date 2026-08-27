//cpp
// @symbol _ZN13BigBrickBlock4KillEv
/* BigBrickBlock::Kill() at ov002 0x020b38a0, 0x210 bytes -- vtable slot 31.
 *
 * ATTRIBUTED BY THE VTABLE. _ZTV13BigBrickBlock (ov002 0x02108adc) carries
 * 0x020b38a0 at slot 31 -- vtable + 0x7c -- while _ZTV10dBgActor_c carries
 * _ZN10dBgActor_c4KillEv at the same slot and both tables carry dActor_c's 0x020100dc
 * at slot 30. So this is this class's own override of the one virtual dBgActor_c
 * adds. The file's old `// recovered name: BigBrickBlock_Kill` comment happened
 * to land on the right class; _ZTI13daObjBlockL_c, the RTTI record it was
 * derived from, is the same class under its Japanese name.
 *
 * ONE CLASS, FIVE ACTOR IDS, which is why this function is a switchboard rather
 * than five overrides: BrickBlock, BigBrickBlock, BrickBlockSwitchActivated,
 * BlackBrickBlock and FortressTowerWall all allocate 816 bytes, run
 * dBgActor_c::dBgActor_c and store _ZTV13BigBrickBlock. See include/BigBrickBlock.h.
 *
 *   0x11  the block that holds a star. If mStarID is set it releases the star:
 *         through dActor_c::UntrackAndSpawnStar when param1's second byte is 0xff
 *         (the star was tracked, so the marker slot has to be freed too), and
 *         through dActor_c::Spawn of actor 0xb2 with the id tagged 0x40 otherwise.
 *         Its poof rides 0x96000 -- 150 20.12 units -- up instead of 0x64000,
 *         and uses particle 0xf.
 *   0x0f  and 0x11 when the star path did not fire: three coins, but only when
 *         mLinkedActor is null. NotifyLinkedActor handles that actor just below.
 *   0x2e  particle 0x48 rather than 0xa.
 *   0x13  the switch-activated one: it sets mBroken and returns WITHOUT
 *         destroying itself, because Behavior re-enables it from the event bit.
 *         Every other id falls through to MarkForDestruction.
 *
 * THE COMPARISON INTERMEDIATES ARE LOAD-BEARING, all five of them. `if (actorID
 * == 0x11)` folds the test into the branch -- cmp/beq, two instructions. The ROM
 * materialises the comparison into a register first and then tests THAT:
 * cmp/moveq #1/movne #0/cmp #0/b, five. Writing the int is what asks for the
 * second shape. Argued in full in src/_ZN16daObjFallBlock_c4KillEv.cpp; here the
 * first one is doubly load-bearing, because the else-branch reuses the very
 * register it was materialised into for `isF || is11`.
 *
 * The last Vector3 is memberwise on purpose: Vector3 declares a destructor
 * (types.h), so a whole-object assignment compiles to an ldm/stm pair, four
 * instructions where the ROM has six.
 *
 * Particle::System::NewSimple and dActor_c::SpawnCoins stay spelled as their
 * mangled names -- both take Fix12<int> BY VALUE and declaring the true types
 * changes how the caller passes them. include/dActor_c.h says the same thing at the
 * bottom of the class, src/_ZN10dBgActor_c4KillEv.cpp argues it in full, and
 * notes/mwccarm-codegen.md 6az is the rule. */
#include "BigBrickBlock.h"
#include "Sound.h"

extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 id, Fix12i x, Fix12i y, Fix12i z);
extern "C" void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(
    void *self, const Vector3 &pos, u32 count, s32 speed, s16 delay);

void BigBrickBlock::Kill()
{
    Vector3 starPos;
    Vector3 pos;
    Vector3 coinPos;
    Vector3 dustPos;

    int is11 = (actorID == 0x11);
    if (is11 && mStarID != 0) {
        starPos.x = mPosX;
        starPos.y = mPosY;
        starPos.z = mPosZ;
        starPos.y += 0x96000;

        if (((param1 >> 8) & 0xff) == 0xff) {
            UntrackAndSpawnStar(mTrackStarID, mStarID, starPos, 4);
        } else {
            Spawn(0xb2, mStarID | 0x40, starPos, 0, mAreaId, -1);
        }
    } else {
        int isF = (actorID == 0xf);
        if (isF || is11) {
            if (mLinkedActor == 0) {
                coinPos.x = mPosX;
                coinPos.y = mPosY;
                coinPos.z = mPosZ;
                _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(this, coinPos, 3, 0xf000, 0);
            }
        }
    }

    {
        Fix12i z = mPosZ;
        Fix12i y = mPosY + 0x64000;
        Fix12i x = mPosX;
        pos.x = x;
        pos.z = z;
        pos.y = y;
    }

    u32 kind = 0xa;
    int f11 = (actorID == 0x11);
    if (f11) {
        pos.y = mPosY + 0x96000;
        kind = 0xf;
    }
    int f2e = (actorID == 0x2e);
    if (f2e) {
        kind = 0x48;
    }
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(kind, pos.x, pos.y, pos.z);

    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);

    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);

    NotifyLinkedActor();

    int f13 = (actorID == 0x13);
    if (f13) {
        mBroken = 1;
        return;
    }

    if (HasNonzeroAngleX()) {
        SpawnSoundObj(1);
    }
    MarkForDestruction();
}
