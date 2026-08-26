//cpp
// @symbol _ZN15IceSlideManager8BehaviorEv
#include "IceSlideManager.h"

extern "C" u16 DecIfAbove0_Short(u16 *timer);

/* ABI wall: spelling this as Sound::PlaySub(..., Fix12<int>, bool) makes
 * mwccarm home the by-value Fix12 in an 8-byte stack slot, growing this body
 * from 0xa4 to 0xb8. The cartridge passes the same raw fixed-point word in r3,
 * so retain the measured scalar call view while using real actor methods. */
extern "C" int _ZN5Sound7PlaySubEjjj5Fix12IiEb(
    u32 soundID, u32 volume, u32 pan, Fix12i distance, bool loop);

int IceSlideManager::Behavior()
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
