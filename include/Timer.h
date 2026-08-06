/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Timer: 4 matched functions, 3 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef TIMER_H
#define TIMER_H
#include "types.h"

struct Timer {
    /* One 64-bit tick count, not two words. The history pass reads it as s64 from
       seven accesses, and the pre-migration include/Timer.hpp declared
       `s64 mTimeBase` with a sizeof(Timer)==0xc assert. The ROM shows paired 4-byte
       accesses at 0x0 and 0x4 within the same functions -- which is what a 64-bit
       value looks like on a target with no 64-bit register, so it corroborates
       rather than contradicts. Splitting it is why three of the four methods had to
       spell `*(s64*)((char*)this)`. */
    s64 unk_000;            /* 0x000 */
    u8  mIsRunning;            /* 0x008 */
#ifdef __cplusplus
    /* methods */
    s64 GetTime();
    void ResetTimer();
    void StartTimer();
    void StopTimer();
#endif
};

#endif
