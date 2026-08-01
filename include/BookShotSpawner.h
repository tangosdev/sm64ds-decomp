/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BookShotSpawner: 5 matched functions, 5 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BOOKSHOTSPAWNER_H
#define BOOKSHOTSPAWNER_H
#include "types.h"

struct BookShotSpawner {
    u8  pad_000[0x5c];
    u8  unk_05c;            /* 0x05c */
    u8  pad_05d[0x31];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x2];
    u8  unk_092;            /* 0x092 */
    u8  pad_093[0x39];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
    u16 unk_0d4;            /* 0x0d4 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
#endif
};

#endif
