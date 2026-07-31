#include "types.h"
/* _ZN7Message11PrepareTalkEv at 0x0201ab38, size=0x34
 * Message::PrepareTalk - ducks music volume to 0x40 at rate 0xc999/4096,
 * sets the "in talk" flag at 0x0209d664 to 1.
 */
extern u8 unk_0209d664;  /* 0x0209d664 */

extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(u32 vol, Fix12i speed);

void _ZN7Message11PrepareTalkEv(void)
{
    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x40, 0x0000c999);
    unk_0209d664 = 1;
}
