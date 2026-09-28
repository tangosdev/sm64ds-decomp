//cpp
// @symbol func_ov002_020d7430
#include "Player.h"
#include "Sound.h"

extern "C" {
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(char* player, Vector3* pos,
    unsigned int damage, int knockback, unsigned int arg4,
    unsigned int arg5, unsigned int arg6);
void func_ov002_020d718c(char* player);
// State globals still have incompatible declarations across the legacy callers.
int _ZN6Player7IsStateERNS_5StateE(char* player, void* state);
void _ZN6Player11ChangeStateERNS_5StateE(char* player, void* state);
extern char data_ov002_02110034;
extern char data_ov002_0211013c;

void func_ov002_020d7430(Player& player)
{
    char* c = reinterpret_cast<char*>(&player);
    dActor_c* mouthActor = reinterpret_cast<dActor_c*>(player.mObjInMouth);
    if (mouthActor->OnYoshiTryEat() == 2) {
        Vector3 pos;
        pos.x = player.mPosX;
        pos.y = player.mPosY;
        pos.z = player.mPosZ;
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(c, &pos, 1, 0xc000, 1, 0, 0);
        return;
    }
    player.func_ov002_020c9e18();
    mouthActor = reinterpret_cast<dActor_c*>(player.mObjInMouth);
    mouthActor->OnTurnIntoEgg(player);
    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_02110034)) {
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211013c);
    }
    func_ov002_020d718c(c);
    Sound::PlayCharVoice(0, 0xfd,
        *reinterpret_cast<const Vector3*>(&player.mCamSpacePosX));
}
}
