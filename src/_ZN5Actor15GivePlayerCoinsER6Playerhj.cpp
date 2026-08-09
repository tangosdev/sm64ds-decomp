//cpp
// @symbol _ZN5Actor15GivePlayerCoinsER6Playerhj
/* Actor::GivePlayerCoins(Player &player, u8 count, u32 coinKind) at 0x0201061c.
 *
 * Pays a player for this actor: plays the pickup sound at this actor's own
 * camera-space position, credits the coins, heals by the same amount scaled
 * 0x100, and spawns the coin-count popup above the player, parented to them.
 *
 * A member -- `this` supplies the sound's position, `add r1, r0, #0x74`,
 * mCamSpacePos. Everything else is read off `player`.
 *
 * PLAYER IS A REFERENCE THAT THE ROM NULL-CHECKS. `movs r5, r1` sets the flags
 * off the incoming argument and returns immediately when it is zero. The
 * mangled name says R6Player, so the parameter is a reference and the test is
 * real; both are true at once and the source has to say so. `&player == 0` is
 * how that is spelled, and mwcc 1.2 emits the test rather than folding it away.
 *
 * coinKind is unsigned, and that is not from the name alone: `cmp r4, #3` is
 * followed by `hs`-conditioned returns -- an unsigned compare. count is `h`,
 * and being a register argument its width is not observable here; the name
 * carries it.
 *
 * The two tables are indexed by coinKind: 0x02075238 by word (`ldr r1, [r1, r4,
 * lsl #2]`) for the value each kind is worth, 0x02075230 by halfword (`lsl r3,
 * r4, #1` then `ldrh`) for the popup's actor ID. The spawn position is the
 * player's own, raised 0x96000 -- copied field by field into a stack Vector3,
 * which is what the three load/store pairs at [sp,#8..0x10] are.
 */
#include "Actor.h"
#include "Player.h"

extern "C" {
/* Indexed by coinKind. No named symbol exists for either yet, and inventing one
   means adding to symbols.txt, which is its own change -- see the module
   attribution trap in notes. Kept as the ROM's own names. */
extern u32 data_02075238[]; /* coins each kind is worth */
extern u16 data_02075230[]; /* actor ID of the popup each kind spawns */

void GiveCoins(s32 playerNo, s32 amount); /* 0x0202a7b8 */
}

/* 0x02012664. Declared as a member so the compiler mangles it for us rather
   than having the name hand-spelled; there is no Sound.h yet. */
struct Sound {
    static void PlayBank3(u32 bank, const Vector3 &pos);
};

void Actor::GivePlayerCoins(Player &player, u8 count, u32 coinKind)
{
    if (&player == 0)
        return;
    if (count == 0)
        return;
    if (coinKind >= 3)
        return;

    if (player.mIsUnderwater)
        Sound::PlayBank3(0x12, *(const Vector3 *)&mCamSpacePosX);
    else
        Sound::PlayBank3(0x11, *(const Vector3 *)&mCamSpacePosX);

    s32 amount = count * data_02075238[coinKind];
    GiveCoins(player.mPlayerNo, amount);
    player.Heal(amount << 8);

    /* Through a base pointer, not three field reads: the ROM materialises
       `add r2, r5, #0x5c` once and loads [r2], [r2,#4], [r2,#8] off it.
       Reading player.mPosX/Y/Z directly costs one instruction less (0xf4, not
       0xf8) because the offsets fold into r5. */
    const Vector3 *src = (const Vector3 *)&player.mPosX;
    Vector3 spawnPos;
    spawnPos.x = src->x;
    spawnPos.y = src->y;
    spawnPos.z = src->z;
    spawnPos.y = spawnPos.y + 0x96000;

    Actor *popup = Spawn(data_02075230[coinKind], 8, spawnPos, 0,
                         player.mAreaId, -1);
    if (popup != 0)
        *(Player **)((char *)popup + 0x39c) = &player;
}
