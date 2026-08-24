//cpp
// @symbol _ZN12HealingHeart8BehaviorEv
/* HealingHeart::Behavior -- once something is standing in the collider, heal
 * that actor if it is the player (object id 0xbf), then start a 90-frame
 * cooldown. The heart spins slowly while idle and fast while cooling down.
 *
 * The heal amount is 0x100 up to 8 hit points and then 0x100 per four points
 * above that, which is the `(hp - 8) / 4 + 1' shifted into the same 8.8 form.
 *
 * dActor_c::FindWithID, Player::Heal, Sound::PlayBank3, DecIfAbove0_Byte and
 * Math_Function_0203b14c have no headers here, so those five stay extern-C.
 * The actor FindWithID hands back is reached by offset for the same reason the
 * player is in dBgActor_c::KillByMegaChar: this TU has no use for Player.h
 * beyond two words.
 */
#include "HealingHeart.h"

extern "C" {
u8   DecIfAbove0_Byte(u8 *p);
void *_ZN8dActor_c10FindWithIDEj(u32 id);
void _ZN6Player4HealEi(char *player, int amount);
void _ZN5Sound9PlayBank3EjRK7Vector3(u32 id, const Vector3 *pos);
void Math_Function_0203b14c(int *p, int a, int b, int c, int d);
}

int HealingHeart::Behavior()
{
    int spin = 0x1000;
    DecIfAbove0_Byte(&mHealTimer);

    if (mdCcAc_c.otherOwner != 0) {
        if (unk_171 == 0) {
            unk_171 = 1;
            mHealTimer = 0;
        }
        if (mHealTimer == 0) {
            char *other = (char *)_ZN8dActor_c10FindWithIDEj(mdCcAc_c.otherOwner);
            if (other != 0) {
                int isPlayer = (int)(*(u16 *)(other + 0xc) == 0xbf);
                if (isPlayer != 0) {
                    int hp = *(int *)(other + 0x98) / 0x1000;
                    int amount = 0x100;
                    if (hp >= 8) {
                        hp -= 8;
                        amount = (hp / 4 + 1) << 8;
                    }
                    _ZN6Player4HealEi(other, amount);
                    _ZN5Sound9PlayBank3EjRK7Vector3(0x61, (Vector3 *)&mCamSpacePosX);
                }
            }
            mHealTimer = 0x5a;
        }
    } else {
        unk_171 = 0;
    }

    if (mHealTimer > 0x2d)
        spin = 0x8000;
    Math_Function_0203b14c(&unk_16c, spin, 0x400, 0x2000, 0x80);
    mModelAnim.speed = unk_16c;
    mModelAnim.Advance();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}
