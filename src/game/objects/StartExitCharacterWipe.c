#include "types.h"
/* StartExitCharacterWipe at 0x020298d8
 * Starts the exit wipe for the current character (per-character wipe table).
 */
extern u8 data_0209caa0[]; /* save data; +0x41 = current character */
extern u8 data_020755bc[]; /* character -> wipe type table */
extern void StartExitFaderWipe(int wipeType);

void StartExitCharacterWipe(void)
{
    StartExitFaderWipe(data_020755bc[data_0209caa0[0x41]]);
}
