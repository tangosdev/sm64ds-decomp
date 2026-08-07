//cpp
// @symbol _ZN7Message21DisplayLevelClearTextEta
#include "decl_common.h"
#include "Message.h"

/* Message::DisplayLevelClearText(u16 msgID, s8 course) at 0x0201cf80 -- static.
 *
 * The "course clear" banner. Unlike the save-menu prompt it does not reset the
 * subsystem first -- it primes per-course state at 0x0201d850, then selects the
 * message, raises the window flag and asks for layout style 1.
 *
 * `Eta` is (u16, s8): the course index is a SIGNED byte, which is worth keeping --
 * widening it to int would spell `Eti` and name a different function.
 */
extern "C" {
extern unsigned short data_0209d6d4;  /* selected message ID */
extern unsigned char data_0209d668;   /* window is up */
int func_0201d850(signed char course);
}

void Message::DisplayLevelClearText(u16 msgID, s8 course)
{
    func_0201d850(course);
    data_0209d6d4 = msgID;
    data_0209d668 = 1;
    func_0201e220(1);
}
