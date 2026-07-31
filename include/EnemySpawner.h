/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class EnemySpawner: 4 matched functions, 5 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ENEMYSPAWNER_H
#define ENEMYSPAWNER_H
#include "types.h"

struct EnemySpawner {
    u8  pad_000[0x90];
    s16 unk_090;            /* 0x090 */
    u8  pad_092[0x3a];
    s8  unk_0cc;            /* 0x0cc */
    u8  pad_0cd[0x7];
    s16 unk_0d4;            /* 0x0d4 */
    u8  pad_0d6[0x2];
    s32 unk_0d8;            /* 0x0d8 */
    u8  unk_0dc;            /* 0x0dc */
#ifdef __cplusplus
    /* methods */
    int InitResources();
#endif
};

#endif
