//cpp
// @symbol _ZN7Message19DisplaySaveMenuTextEt
#include "decl_common.h"
#include "Message.h"

/* Message::DisplaySaveMenuText(u16 msgID) at 0x0201e314 -- static, no `this`.
 *
 * Opens the save-menu prompt: selects the message, resets the subsystem the same
 * way ResetAllGlobals does (clear 0x0209d660, then the reset at 0x0201eaac), then
 * raises the "window up" flag and asks for layout style 0.
 *
 * `Et` in the mangled name is u16, so msgID is 16-bit and not the `int` it would
 * be tempting to widen it to.
 */
extern "C" {
extern unsigned short data_0209d6d4;  /* selected message ID */
extern unsigned char data_0209d660;
extern unsigned char data_0209d6a8;
extern unsigned char data_0209d668;   /* window is up */
}

void Message::DisplaySaveMenuText(u16 msgID)
{
    data_0209d6d4 = msgID;
    data_0209d660 = 0;
    func_0201eaac();
    data_0209d6a8 = 0;
    data_0209d668 = 1;
    func_0201e220(0);
}
