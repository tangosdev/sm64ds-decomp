//cpp
// @symbol _ZN7Message7EndTalkEv
#include "Message.h"

/* Message::EndTalk() at 0x0201ab04 -- static, no `this`.
 *
 * The inverse of PrepareTalk: restores music to full volume (0x7f) over
 * 0x64cc/4096 of a frame -- a slower ramp back up than the duck down -- and clears
 * the "in talk" flag.
 *
 * Sound::ChangeMusicVolume stays `extern "C"`: by-value `Fix12<int>` parameter,
 * runbook-type-reconstruction.md section 7.
 */
extern "C" {
extern u8 data_0209d664;  /* nonzero while a dialogue box is up */
void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(u32 vol, Fix12i speed);
}

void Message::EndTalk()
{
    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x000064cc);
    data_0209d664 = 0;
}
