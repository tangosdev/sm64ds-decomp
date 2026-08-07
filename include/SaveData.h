/* Hand-edited, against evidence. This file used to carry the
 * "AUTO-GENERATED ... by tools/gen_header.py" banner, which was never true --
 * see notes/runbook-type-reconstruction.md section 2: no such generator was ever
 * committed, and the 242 headers carrying that banner were added in one commit
 * that added zero tools/ files. `tools/gen_header.py --report` exists now and
 * emits a differential; it writes nothing to include/.
 *
 * class SaveData: 12 matched functions, 3 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen.
 *
 * SaveData is NOT polymorphic -- it has no RTTI record and no vtable -- so the
 * key-function rule (runbook section 7) does not apply and its methods can be
 * migrated to real C++ freely. Most are STATIC: the ROM functions take no `this`
 * and read the single global instance at 0x0209caa0 directly. */
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

    /* Static: the ROM bodies take no `this` and reach the global instance at
       0x0209caa0 directly. Declared here so the compiler mangles the names
       instead of each file spelling them by hand. */
    static int CanPlayerHaveCap();
    static void PlayerLoseCap();
    static int HasPlayerLostCap();
    static u8 GetCoinRecord(u32 courseID);
    static s32 IsCharacterUnlocked(u32 character);
#endif
};

#endif
