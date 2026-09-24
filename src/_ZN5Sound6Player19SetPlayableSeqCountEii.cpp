//cpp
// @symbol _ZN5Sound6Player19SetPlayableSeqCountEii
/* recovered: shared header, real C++ method
 *
 * Writes the +0x18 field of a 0x1c-byte record in the 32-record table at
 * data_020a4d6c, selected by `index`. The whole ROM body is six instructions:
 *
 *     mov r2, #0x1c        ; record stride
 *     mul r2, r0, r2       ; r0 is the INDEX, not `this` -- see Sound.h
 *     lsl r0, r1, #0x10
 *     ldr r1, [pc, #8]     ; data_020a4d6c + 0x18
 *     lsr r0, r0, #0x10    ; count, zero-extended to 16 bits
 *     str r0, [r1, r2]     ; ...stored as a full WORD
 *
 * The literal is 0x020a4d84, which the symbol list also names data_020a4d84.
 * That is not a second table. No other relocation points there, the table's
 * other users reach the same word as record field +0x18 (func_0204fc40 sets
 * it to 1, func_0204f63c reads it back as the limit), and mwccarm folds the
 * member offset into the literal, which is why the store below compiles to
 * these six instructions.
 *
 * The narrowing and the store width disagree on purpose, and that is the
 * finding: `count` is truncated to 16 bits and then written as a 32-bit word,
 * so the two bytes ABOVE the count are always cleared as a side effect. A
 * `strh` would have left them alone. Whatever shares that word with the count
 * cannot survive a call to this function.
 */
#include "Sound.h"
#include "SoundPlayerRecord.h"

extern "C" {
extern SoundPlayerRecord data_020a4d6c[];
}

void Sound::Player::SetPlayableSeqCount(int index, int count)
{
    data_020a4d6c[index].mPlayableSeqCount = (unsigned short)count;
}
