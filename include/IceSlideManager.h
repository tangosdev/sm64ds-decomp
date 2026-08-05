/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class IceSlideManager: 4 matched functions, 5 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ICESLIDEMANAGER_H
#define ICESLIDEMANAGER_H
#include "types.h"

struct IceSlideManager {
    u8  pad_000[0x5c];
    u8  unk_05c;            /* 0x05c */
    u8  pad_05d[0x3];
    u8  unk_060;            /* 0x060 */
    u8  pad_061[0x3];
    u8  unk_064;            /* 0x064 */
    u8  pad_065[0x6f];
    s16 unk_0d4;            /* 0x0d4 */
    u8  unk_0d6;            /* 0x0d6 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

#endif
