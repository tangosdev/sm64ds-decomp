//cpp
// @symbol _ZN7Message11PrepareTalkEv
#include "Message.h"

/* Message::PrepareTalk() at 0x0201ab38 -- static, no `this`.
 *
 * Ducks the music before dialogue: volume to 0x40 over 0xc999/4096 of a frame, and
 * raises the "in talk" flag. EndTalk is the exact inverse.
 *
 * Sound::ChangeMusicVolume stays an `extern "C"` free function on purpose. Its
 * second parameter is a by-value `Fix12<int>`, and declaring a method with a
 * by-value class parameter is a measured dead end -- mwccarm homes r0-r3 to the
 * stack for it, +0x14, on all 25 sweep versions at every optimisation level. See
 * runbook-type-reconstruction.md section 7.
 */
extern "C" {
extern u8 data_0209d664;  /* nonzero while a dialogue box is up */
void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(u32 vol, Fix12i speed);
}

void Message::PrepareTalk()
{
    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x40, 0x0000c999);
    data_0209d664 = 1;
}
