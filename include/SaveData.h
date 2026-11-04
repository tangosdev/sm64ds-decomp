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
 * is the 0x44-byte save-file record those methods move. Offsets/widths are
 * observed, not guessed.
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

/* fwd */
struct MinigameSaveData;

/* One save file: 0x44 bytes, the unit ReadFileData/SaveFile/EraseSaveFile
   move to and from cart slots 0..2. The live copy is the first 0x44 bytes of
   the save block at 0x0209caa0, and the file-select scene (d_s_dsmt.cpp)
   keeps three of them while copying files. */
struct FileSaveData {
    /* 0x000 -- "8000" as four ASCII bytes, written by SetDefaultValues
       (0x30303038 little-endian). The file-format magic. */
    u32 magic8000;
    /* 0x004 -- per-character cap state. Bit 24+n is set when character n has
       lost their cap; PlayerLoseCap sets it, HasPlayerLostCap tests it. */
    u32 flags1;
    /* 0x008 -- character unlocks in the low bits (bit 0 Mario, 1 Luigi,
       2 Wario ...), FILE_FLAGS2_INTRO_DONE at bit 7 (enum FileSaveFlags2
       below), and the eight glowing rabbits at bits 20..27.
       WIDTH CORRECTED: this was declared `u8 unk_008`, but every access is
       32 bits wide -- IsCharacterUnlocked tests the whole word,
       SetCharacterIntro read-modify-writes it, and NumGlowingRabbitsFound
       loads it as an int. The u8 was the marker the generator emitted, not
       the width. */
    s32 flags2;
    /* 0x00c -- one bit per minigame rabbit caught, bit (rabbit << 2) | b.
       daMip_c tests it with func_02013890(mRabbitId, player->param1), where
       b < 0 asks for all four; func_02013868 sets it and unlocks the minigame
       bit of the same index in MinigameSaveData. ReadFileData ANDs it with
       that unlock word when a file is loaded. */
    u32 minigameRabbits;
    /* 0x010 -- one bit per course whose cannon is open (OpenCannon). */
    u32 cannonFlags;
    /* 0x014 -- star bits, one byte per course (data_0209cab4 in the live
       block). */
    u8 stars[30];
    /* 0x032 -- coin records, one byte per main course (data_0209cad2). */
    u8 coinRecords[15];
    u8 mCharacter; /* 0x041 -- current character */
    /* 0x042 -- the active player's controller mode: SetDefaultValues clears
       it, and InitControllerMode/SetControllerMode (d_s_stage_methods.cpp)
       keep it in step with the per-controller record. */
    u8 controllerMode;
};

/* Named bits of FileSaveData::flags2 (the live word is 0x0209caa8). */
enum FileSaveFlags2 {
    /* The opening camera sweep has played for this file. The only writer is
       the intro Lakitu: daC_Jugem_c::StateIntroMain sets it when the sweep
       hands the camera back, and the Lakitu's variant 1 enters the intro
       while it is clear. Outside VS mode, normal play waits on it: dMeter_c
       and dMap_c draw the in-course HUD only once it is set, dScStage_c
       refuses pause and clears camera touch input before it, the message
       box ignores the advance button before it, and LevelObjects'
       IntroAllowed() tests it clear. At runtime, clearing it in a course
       hides the whole HUD and turns the bottom map to a north-up zoomed
       view with a star marker. */
    FILE_FLAGS2_INTRO_DONE = 0x80
};

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
