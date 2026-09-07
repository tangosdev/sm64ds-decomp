//cpp
// @symbol _ZN13OneUpMushroom13OnTurnIntoEggER6Player
// recovered name: OneUpMushroom_OnTurnIntoEgg
/* da1up_c::OnTurnIntoEgg -- vtable slot 19, verified against ov002 relocs.txt:
 * _ZTV13OneUpMushroom (0x021083c8) + 0x4c -> 0x020af2b0, exactly this
 * placeholder's former address (former name func_ov002_020af2b0).
 * Matched byte-for-byte with mwccarm 2004/b56 (ov002).
 */
#include "OneUpMushroom.h"
#include "Player.h"
#include "Sound.h"

extern "C" {
extern void GiveLives(int);
extern int func_ov002_020af684(OneUpMushroom *thiz, int variant, Player *player);
extern void func_ov002_020bdf8c(Player *player);
}

int OneUpMushroom::OnTurnIntoEgg(Player &player)
{
    if (mMushroomType == 0xb) {
        return func_ov002_020af684(this, 5, &player);
    }
    if (mMushroomType == 0xc) {
        return func_ov002_020af684(this, 7, &player);
    }
    unsigned isMatch = (actorID == 0x114);
    if (isMatch) {
        Vector3 vec;
        Sound::PlayBank3(0x6e, *(Vector3 *)&mCamSpacePosX);
        GiveLives(1);
        vec.x = mPosX;
        vec.y = mPosY;
        vec.z = mPosZ;
        vec.y += 0xb4000;
        Spawn(0x14b, 8, vec, 0, mAreaId, -1);
        KillAndTrackInDeathTable();
    } else {
        func_ov002_020bdf8c(&player);
        KillAndTrackInDeathTable();
    }
}
