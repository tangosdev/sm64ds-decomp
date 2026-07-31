/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class SaveData: 12 matched functions, 3 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SAVEDATA_H
#define SAVEDATA_H
#include "types.h"

/* fwd */
struct FileSaveData;
struct MinigameSaveData;
struct fsd_;
struct mg_;
struct SaveData {
    u8  pad_000[0x8];
    u8  unk_008;            /* 0x008 */
    u8  pad_009[0x38];
    u8  unk_041;            /* 0x041 */
    u8  unk_042;            /* 0x042 */
#ifdef __cplusplus
    /* methods */
    void SetDefaultValues(FileSaveData * fsd_);
    void SetDefaultValuesMg(MinigameSaveData * mg_);
#endif
};

#endif
