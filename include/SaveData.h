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
    /* 0x000 -- "8000" as four ASCII bytes, written by SetDefaultValues
       (0x30303038 little-endian). The file-format magic. */
    u32 magic8000;
    /* 0x004 -- per-character cap state. Bit 24+n is set when character n has
       lost their cap; PlayerLoseCap sets it, HasPlayerLostCap tests it. */
    u32 flags1;
    /* 0x008 -- character unlocks in the low bits (bit 0 Mario, 1 Luigi,
       2 Wario ...) and the eight glowing rabbits at bits 20..27.
       WIDTH CORRECTED: this was declared `u8 unk_008`, but every access is a
       32-bit read-modify-write -- IsCharacterUnlocked and SetCharacterIntro
       index it as s32[2], and NumGlowingRabbitsFound loads a whole int from
       +0x8. The u8 was the marker the generator emitted, not the width. */
    s32 flags2;
    u8  pad_00c[0x35];
    u8  unk_041;            /* 0x041 -- current character */
    u8  unk_042;            /* 0x042 */
#ifdef __cplusplus
    /* methods */
    void SetDefaultValues(FileSaveData * fsd_);
    void SetDefaultValuesMg(MinigameSaveData * mg_);

    /* Static: the ROM bodies take no `this` and reach the global instance at
       0x0209caa0 directly. Declared here so the compiler mangles the names
       instead of each file spelling them by hand.

       Every parameter type below is read off the mangled name, not chosen:
       `Ej` is u32, `Ei` is s32, `Eah` is (s8, u8), `EPc` is char*. Widening
       SetCoinRecordIfHigher's first parameter to int would spell `Ei` and name
       a different function. */
    static int CanPlayerHaveCap();
    static void PlayerLoseCap();
    static int HasPlayerLostCap();
    static u8 GetCoinRecord(u32 courseID);
    static s32 IsCharacterUnlocked(u32 character);
    static void SetCharacterIntro(s32 character);
    static void SetCoinRecordIfHigher(s8 courseID, u8 coins);
    static u8 CountStarsCollectedInLevel(u32 courseID);
    static int NumGlowingRabbitsFound();

    /* Cart I/O and whole-file operations. */
    static int ReadDataFromCart(char* buf, u32 len, u32 slot);
    static int SaveDataToCart(char* data, u32 size, u32 fileID);
    static int ReadFileData(u32 fileID, FileSaveData* dest);
    static int SaveFile(u32 fileID, FileSaveData* data);
    static int EraseSaveFile(u32 fileID, char* saveArea);
    static int SaveCurrentFile();
    static u32 EraseAllSaveData();
    static int SaveMinigames(MinigameSaveData* data);
    static int ReadMinigameData(MinigameSaveData* dest);
#endif
};

#endif
