/* Hand-edited, against evidence. This file used to carry the
 * "AUTO-GENERATED ... by tools/gen_header.py" banner, which was never true --
 * see notes/runbook-type-reconstruction.md section 2: no such generator was ever
 * committed, and the 242 headers carrying that banner were added in one commit
 * that added zero tools/ files. `tools/gen_header.py --report` exists now and
 * emits a differential; it writes nothing to include/.
 *
 * SaveData: the save-file manager. 18 of its 20 methods are defined in
 * src/engine/save/SaveData.cpp, beside the free functions folded with them;
 * the two cart I/O methods are in src/engine/save/SaveData2.cpp. FileSaveData
 * (include/SaveBlock.h) is the 0x44-byte save-file record those methods move.
 *
 * SaveData is NOT polymorphic -- it has no RTTI record and no vtable -- so the
 * key-function rule (runbook section 7) does not apply and its methods can be
 * migrated to real C++ freely. They are all STATIC: the ROM functions take no
 * `this` and read the single save block at 0x0209caa0 directly. SaveData
 * carries no data of its own; the record type is FileSaveData, the name the
 * ROM's mangled signatures give it (`EP12FileSaveData`). */
#ifndef SAVEDATA_H
#define SAVEDATA_H
#include "types.h"
#include "SaveBlock.h"

#ifdef __cplusplus
struct SaveData {
    /* methods */

    /* Static, like the rest: both ROM bodies take one pointer in r0 and never a
       `this`. They were declared non-static here, with the r0 pointer modelled as
       `this` and the declared parameter left unread; the mangled names, the ROM
       bodies and every caller all say otherwise, and both bodies reproduce byte
       for byte with the parameter spelled honestly. */
    static void SetDefaultValues(FileSaveData * fsd_);
    static void SetDefaultValuesMg(MinigameSaveData * mg_);

    /* Static: the ROM bodies take no `this` and reach the global instance at
       0x0209caa0 directly. Declared here so the compiler mangles the names
       instead of each file spelling them by hand.

       Every parameter type below is read off the mangled name, not chosen:
       `Ej` is u32, `Ei` is s32, `EPc` is char*. SetCoinRecordIfHigher is
       (int, u8), `Eih`: its body compiles alike with an s8 or an int course,
       and both callers pass a level index wider than s8 with no narrowing in
       the ROM; the coin count is u8 because the body compares it unextended
       (an int there changes the bytes). */
    static int CanPlayerHaveCap();
    static void PlayerLoseCap();
    static int HasPlayerLostCap();
    static u8 GetCoinRecord(u32 courseID);
    static s32 IsCharacterUnlocked(u32 character);
    static void SetCharacterIntro(s32 character);
    static void SetCoinRecordIfHigher(int courseID, u8 coins);
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
};
#endif

#endif
