//cpp
// @symbol _ZN6Player15St_Talk_CleanupEv
/* recovered: named members + shared header, real C++ method
 *
 * Leaving the talk state: clear the "in conversation" bit (0x800000) on the
 * actor being talked to, on the player, and on the global at data_0209b454;
 * clear bit 3 on data_0209f318+0x154; and restore music layer 1 if this state
 * had overridden it.
 *
 * mTalkActor is declared u8 in Player.h but holds an actor pointer -- +0xb0
 * is read through it, which is ActorBase's mFlags. Reached through a cast
 * here rather than retyping the field, because Player::St_Talk_Main reaches
 * it the same way and the two should change together.
 */
#include "Player.h"
extern u32 data_0209b454;
extern char* data_0209f318;
extern "C" {
extern void _ZN5Sound22LoadAndSetMusic_Layer1Ei(int);
}

int Player::St_Talk_Cleanup()
{
    void* p = *(void**)((char*)&mTalkActor);
    if (p) {
        *(u32*)(((long long)(int)((char*)p + 0xb0))) &= ~0x800000;
        *(void**)((char*)&mTalkActor) = 0;
    }
    mFlags &= ~0x800000;
    data_0209b454 &= ~0x800000;
    *(u32*)(((long long)(int)((char*)data_0209f318 + 0x154))) &= ~8;
    if (unk_725) {
        _ZN5Sound22LoadAndSetMusic_Layer1Ei(0x39);
        unk_725 = 0;
    }
    return 1;
}
