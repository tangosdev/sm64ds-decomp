//cpp
// @symbol _ZN5Sound6Player19SetPlayableSeqCountEii
/* recovered: shared header, real C++ method
 *
 * Writes one field of a 0x1c-byte record in the table at data_020a4d84,
 * selected by `index`. The whole ROM body is six instructions:
 *
 *     mov r2, #0x1c        ; record stride
 *     mul r2, r0, r2       ; r0 is the INDEX, not `this` -- see Sound.h
 *     lsl r0, r1, #0x10
 *     ldr r1, [pc, #8]     ; data_020a4d84
 *     lsr r0, r0, #0x10    ; count, zero-extended to 16 bits
 *     str r0, [r1, r2]     ; ...stored as a full WORD
 *
 * The narrowing and the store width disagree on purpose, and that is the
 * finding: `count` is truncated to 16 bits and then written as a 32-bit word,
 * so the two bytes ABOVE the count are always cleared as a side effect. A
 * `strh` would have left them alone. Whatever shares that word with the count
 * cannot survive a call to this function.
 */
#include "Sound.h"

extern "C" {
extern char data_020a4d84[];
}

void Sound::Player::SetPlayableSeqCount(int index, int count)
{
    *(unsigned int *)(data_020a4d84 + index * 0x1c) = (unsigned short)count;
}
