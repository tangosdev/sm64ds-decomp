//cpp
// @symbol _ZN11CrazedCrate13OnTurnIntoEggER6Player
/* recovered: real C++ method, vtable slot 19. func_ov080_02125318 used raw
 * (char*)self/void*player casts and free-function extern "C" declarations
 * for calls that are real members on dActor_c/fBase_c/Player; those are now
 * real member calls. Particle::System::NewSimple has no header yet, so it
 * stays an extern "C" free function under its mangled name. */
#include "CrazedCrate.h"
#include "Sound.h"
#include "Player.h"

extern "C" {
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
}

int CrazedCrate::OnTurnIntoEgg(Player &player)
{
    if (player.IsCollectingCap()) {
        GivePlayerCoins(player, 5, 0);
    }
    Vector3 vec;
    Vector3 vec2;
    int x = mPosX;
    int z = mPosZ;
    int y = mPosY + 0xb4000;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    ((int *)&vec2)[0] = ((int *)&vec)[0];
    ((int *)&vec2)[1] = ((int *)&vec)[1];
    ((int *)&vec2)[2] = ((int *)&vec)[2];
    PoofDustAt(vec2);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(6, vec.x, vec.y, vec.z);
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    MarkForDestruction();
}
