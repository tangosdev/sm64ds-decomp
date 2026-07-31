#include "types.h"
/* _ZN7Message7EndTalkEv at 0x0201ab04, size=0x34
 * Message::EndTalk - restores music volume to 0x7f at rate 0x64cc/4096,
 * clears the "in talk" flag at 0x0209d664.
 */
extern u8 unk_0209d664;  /* 0x0209d664 */

extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(u32 vol, Fix12i speed);

void _ZN7Message7EndTalkEv(void)
{
    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x000064cc);
    unk_0209d664 = 0;
}
