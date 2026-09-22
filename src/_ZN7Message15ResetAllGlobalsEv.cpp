//cpp
// @symbol _ZN7Message15ResetAllGlobalsEv
#include "Message.h"

/* Message::ResetAllGlobals() at 0x0201fcac -- static, no `this`.
 *
 * Clears the message subsystem between scenes: hands off to the unnamed reset at
 * 0x0201eaac, then clears the flag at 0x0209d660. That is a different byte from
 * the 0x0209d664 PrepareTalk/EndTalk use -- adjacent, but not the same flag.
 */
extern "C" {
int func_0201eaac();
extern unsigned char data_0209d660;
}

void Message::ResetAllGlobals()
{
    func_0201eaac();
    data_0209d660 = 0;
}
