/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MgBounceAndPounce: 7 matched functions, 2 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MGBOUNCEANDPOUNCE_H
#define MGBOUNCEANDPOUNCE_H
#include "types.h"

/* fwd */
struct arg;
struct b_;
struct MgBounceAndPounce {
    u8  pad_000[0x47e4];
    u8  unk_47e4;           /* 0x47e4 */
    u8  pad_47e5[0x81b];
    u8  unk_5000;           /* 0x5000 */
#ifdef __cplusplus
    /* methods */
    int BeforeBehavior();
    int BeforeInitResources();
    void AfterCleanupResources(unsigned int b_);
    void AfterRender(unsigned int arg);
#endif
};

#endif
