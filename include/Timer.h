/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Timer: 4 matched functions, 3 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef TIMER_H
#define TIMER_H
#include "types.h"

struct Timer {
    s32 unk_000;            /* 0x000 */
    s32 unk_004;            /* 0x004 */
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
