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
 * `Eta` is (u16, s8): THIS function's course parameter is a SIGNED byte, which is worth
 * keeping -- widening it to int would spell `Eti` and name a different function. That is
 * a fact about DisplayLevelClearText's own ABI and says nothing about the callee's.
 *
 * func_0201d850 now comes from decl_common.h. It used to be declared here as
 * `int func_0201d850(signed char)` and, in DisplayPauseText, as
 * `int func_0201d850(unsigned char)` -- one function, two signatures, and neither
 * matched the definition's `void func_0201d850(u32)`. Each local declaration had simply
 * been shaped to its own call site's argument, so no conversion was ever emitted and
 * every spelling byte-matched; the bytes could not decide it. See the commit message.
 */
extern "C" {
extern unsigned short data_0209d6d4;  /* selected message ID */
extern unsigned char data_0209d668;   /* window is up */
}

void Message::DisplayLevelClearText(u16 msgID, s8 course)
{
    func_0201d850(course);
    data_0209d6d4 = msgID;
    data_0209d668 = 1;
    func_0201e220(1);
}
