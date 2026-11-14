//cpp
/* leftovers:
 * - func_02013c84 stays `extern "C"` under its cartridge name: callers in
 *   other TUs link it unmangled, and its body forwards to the member SaveFile.
 * - ClearSaveBlock likewise: the save-block wipe is a free function, kept C.
 * - MI_CpuFill8 and MI_CpuCopy8 are assembly
 *   primitives. Their local declarations below use the signatures at their
 *   definitions in src/engine/mi/MI_Memory.c.
 */
/* arm9/SaveData -- the save-file manager: coin and star records, cannons,
 * character intros, cap-loss flags, file slots and the minigame block,
 * folded at .text 0x02013568..0x02013e80. The span also carries the free
 * functions that
 * share this TU's concern: func_02013c84 copies a file block and either
 * records the character or forwards to SaveFile, ClearSaveBlock zeroes the
 * whole save block, and ClearCapHeldByToad..func_02013a88 are the cap-flag
 * helpers (clear/set/test the 0x8000000<<character and 0x1000000<<character
 * bits) around NumGlowingRabbitsFound. The 0x8000000<<character bit says a
 * castle Toad holds that character's lost cap: d_a_ply_methods.cpp sets it
 * (func_02013a00) when a player comes back to the castle capless, the
 * variant-1 Toad in d_a_kinopio.cpp tests it (func_02013a44) to wear the cap
 * and hand it back, and picking the cap up (func_02013a88, from
 * d_a_obj_mario_cap.cpp) clears it with the lost-cap bit. mwccarm emits .text in reverse source order, so the
 * definitions below run ROM-descending; the roster reads ROM-ascending.
 *
 *   func_02013568..func_02013638 -- minigame record tables, seen and unlock bits.
 *   SetCoinRecordIfHigher..func_02013944 -- coin and star records, cannons,
 *     minigame keys, character intros, the rabbit-count bit.
 *   NumGlowingRabbitsFound -- rabbit bits 20..27 of the flags word.
 *   ClearCapHeldByToad..func_02013a88 -- cap-flag clear/set/test helpers.
 *   PlayerLoseCap..CanPlayerHaveCap -- cap-loss flags on the live block.
 *   SaveCurrentFile..SetDefaultValuesMg -- file and minigame writes.
 *   func_02013c84 -- block copy + character record / SaveFile forwarder.
 *   EraseSaveFile..EraseAllSaveData -- slot and whole-block resets.
 *   ClearSaveBlock -- zero the save block.
 *
 * Every method is static: the ROM bodies take no `this` and reach the single
 * save block at 0x0209caa0 directly, declared here as a SaveBlock object
 * (include/SaveBlock.h): the current file (FileSaveData, 0x44 bytes), the minigame block
 * (MinigameSaveData, 0x2e4 bytes, cart slot 3) and the active-file byte at
 * 0x328. The declaration form matters to the bytes: on an `int[]` view
 * mwccarm keeps the block address in one register across func_02013598's
 * branch, while the ROM reloads it per arm, which is what member access on a
 * struct object compiles to. The star bits (0x14), coin records (0x32) and
 * record tables (0x54) are reached through their own data_ symbols, as the
 * ROM's relocations name them.
 */

#include "SaveData.h"
#include "nitro/mi/memory.h"

extern "C" {
extern int data_0209caf4[][5];        /* the minigame record tables, at block + 0x54 */
extern u8 gGameMode;                  /* gate byte read by CanPlayerHaveCap */
extern u8 data_0209cad2[];            /* coin records; local extern: no field on the byte array */
extern unsigned char data_0209cab4[]; /* star bits per course */
extern unsigned char data_0209f224;   /* last collected star id */
/* local extern: no header declares it */
extern int SublevelToLevel(int levelID);
}
extern void ClearCapHeldByToad(void); /* forward: called by func_02013a88 above its definition */
extern void UnlockMinigame(int minigame); /* forward: defined at the end of this file */

void ClearSaveBlock(void) {
    MI_CpuClear8(&data_0209caa0, 0x32c);
}

u32 SaveData::EraseAllSaveData() {
    u32 rc;
    rc = SaveData::EraseSaveFile(0, (char *)&data_0209caa0.file);
    rc |= SaveData::EraseSaveFile(1, (char *)&data_0209caa0.file);
    rc |= SaveData::EraseSaveFile(2, (char *)&data_0209caa0.file);
    SaveData::SetDefaultValuesMg(&data_0209caa0.mg);
    return rc | SaveData::SaveMinigames(&data_0209caa0.mg);
}

void SaveData::SetDefaultValues(FileSaveData *fsd_) {
    MI_CpuClear8(fsd_, 0x44);
    fsd_->magic8000 = 0x30303038;
    fsd_->mCharacter = 3;
    fsd_->flags2 |= 8;
    fsd_->controllerMode = 0;
}

int SaveData::ReadFileData(u32 fileID, FileSaveData *dest) {
    s32 result = SaveData::ReadDataFromCart((char *)dest, 0x44, fileID);
    if (result) {
        SaveData::SetDefaultValues(dest);
        if (result == 2)
            return 1;
        return 0;
    }
    dest->minigameRabbits &= data_0209caa0.mg.unlocked;
    return 1;
}

int SaveData::SaveFile(u32 fileID, FileSaveData *data) {
    data->flags1 |= 1;
    if (SaveData::SaveDataToCart((char *)data, 0x44, fileID) == 0)
        return 1;
    return 0;
}

int SaveData::EraseSaveFile(u32 fileID, char *saveArea) {
    SaveData::SetDefaultValues((FileSaveData *)saveArea);
    if (SaveData::SaveDataToCart(saveArea, 0x44, fileID) == 0)
        return 1;
    return 0;
}

extern "C" int func_02013c84(u8 charID, FileSaveData *src, s32 fileIndex, FileSaveData *dest) {
    MI_CpuCopy8(src, dest, 0x44);
    if (fileIndex < 0) {
        data_0209caa0.activeFile = charID;
        return 1;
    }
    return SaveData::SaveFile(fileIndex, dest);
}

void SaveData::SetDefaultValuesMg(MinigameSaveData *mg_) {
    MI_CpuClear8(mg_, 0x2e4);
    mg_->magic5000 = 0x30303035;
}

int SaveData::ReadMinigameData(MinigameSaveData *dest) {
    s32 result = SaveData::ReadDataFromCart((char *)dest, 0x2e4, 3);
    if (result) {
        SaveData::SetDefaultValuesMg(dest);
        if (result == 2)
            return 1;
        return 0;
    }
    return 1;
}

int SaveData::SaveMinigames(MinigameSaveData *data) {
    if (SaveData::SaveDataToCart((char *)data, 0x2e4, 3) == 0)
        return 1;
    return 0;
}

int SaveData::SaveCurrentFile() {
    if (!SaveData::SaveFile(data_0209caa0.activeFile, &data_0209caa0.file))
        return 0;
    return SaveData::SaveMinigames(&data_0209caa0.mg);
}

int SaveData::CanPlayerHaveCap() {
    if (data_0209caa0.file.mCharacter != 3) {
        int b = (int)(gGameMode == 1);
        if (b == 0)
            return 1;
    }
    return 0;
}

int SaveData::HasPlayerLostCap() {
    if (!SaveData::CanPlayerHaveCap())
        return 0;
    return data_0209caa0.file.flags1 & (0x1000000u << data_0209caa0.file.mCharacter);
}

void SaveData::PlayerLoseCap() {
    if (!SaveData::CanPlayerHaveCap())
        return;
    data_0209caa0.file.flags1 |= (0x1000000u << data_0209caa0.file.mCharacter);
}

void func_02013a88(void) {
    if (!SaveData::CanPlayerHaveCap())
        return;
    data_0209caa0.file.flags1 &= ~(0x1000000u << data_0209caa0.file.mCharacter);
    ClearCapHeldByToad();
}

extern "C" int func_02013a44(void) {
    if (!SaveData::CanPlayerHaveCap())
        return 0;
    return data_0209caa0.file.flags1 & (0x8000000u << data_0209caa0.file.mCharacter);
}

void func_02013a00(void) {
    if (!SaveData::CanPlayerHaveCap())
        return;
    data_0209caa0.file.flags1 |= (0x8000000u << data_0209caa0.file.mCharacter);
}

void ClearCapHeldByToad(void) {
    if (!SaveData::CanPlayerHaveCap())
        return;
    data_0209caa0.file.flags1 &= ~(0x8000000u << data_0209caa0.file.mCharacter);
}

int SaveData::NumGlowingRabbitsFound() {
    int count = 0;
    int f = data_0209caa0.file.flags2;
    unsigned int mask = 0x100000;
    int i = 0;
    do {
        i++;
        if (f & mask)
            count++;
        mask <<= 1;
    } while (i < 8);
    return count;
}

/* Sets flags2 bit (0x100000 << rabbit count). At 7 rabbits, also sets flags1 bit 0x80. */
void func_02013944(void) {
    u32 count = SaveData::NumGlowingRabbitsFound();
    data_0209caa0.file.flags2 |= (0x100000u << count);
    if ((s32)count >= 7)
        data_0209caa0.file.flags1 |= 0x80u;
}

s32 SaveData::IsCharacterUnlocked(u32 character) {
    return data_0209caa0.file.flags2 & (1 << character);
}

void SaveData::SetCharacterIntro(s32 character) {
    data_0209caa0.file.flags2 |= 1 << character;
}

int func_020138dc(void) {
    int count = 0;
    int i;
    int f = data_0209caa0.file.minigameRabbits;
    for (i = 0; i < 0x1c; i++) {
        if (f & (1 << i))
            count++;
    }
    return count;
}

extern "C" int func_02013890(int rabbit, int character) {
    if (character < 0) {
        int f = data_0209caa0.file.minigameRabbits;
        int m = 0xf << (rabbit * 4);
        return (f & m) == m;
    } else {
        int f = data_0209caa0.file.minigameRabbits;
        return f & (1 << (character | (rabbit << 2)));
    }
}

/* A rabbit's minigame key, collected by one character (daObj_Mip_Key_c):
   marks it in the file and unlocks the minigame of the same index. */
void CollectMinigameKey(int rabbit, int character) {
    int minigame = (rabbit << 2) | character;
    data_0209caa0.file.minigameRabbits |= 1 << minigame;
    UnlockMinigame(minigame);
}

int IsCannonOpen(int n) {
    return data_0209caa0.file.cannonFlags & (1 << n);
}

void OpenCannon(int n) {
    data_0209caa0.file.cannonFlags |= 1 << n;
}

int IsCannonOpenInLevel(s8 levelID) {
    return IsCannonOpen(SublevelToLevel(levelID));
}

void OpenCannonInLevel(s8 levelID) {
    OpenCannon(SublevelToLevel(levelID));
}

int IsStarCollected(s32 courseID, s32 starID) {
    return ((1 << starID) & data_0209cab4[courseID]) & 0xff;
}

void CollectStar(int courseID, int starID) {
    data_0209cab4[courseID] |= 1 << starID;
    data_0209f224 = starID;
}

u8 SaveData::CountStarsCollectedInLevel(u32 courseID) {
    u8 count = 0;
    s32 star;
    for (star = 0; star < 8; star++) {
        if (IsStarCollected(courseID, star))
            count++;
    }
    return count;
}

extern "C" u8 CountStarsCollectedInLevelToDisplay(s32 courseID) {
    u8 total;
    s32 course;
    if (courseID <= 0xe)
        return SaveData::CountStarsCollectedInLevel((u32)courseID);
    total = 0;
    course = 0xf;
    do {
        total = (total + SaveData::CountStarsCollectedInLevel((u32)course)) & 0xff;
        course++;
    } while (course <= 0x1d);
    return total;
}

int IsStarCollectedInLevel(s8 levelID, s32 starID) {
    return IsStarCollected(SublevelToLevel(levelID), starID);
}

void CollectStarInLevel(s8 levelID, s32 starID) {
    CollectStar(SublevelToLevel(levelID), starID);
}

u8 NumStars(void) {
    u8 count = 0;
    s8 level = 0;
    s32 star;
    for (level = 0; level < 0x1e; level++) {
        for (star = 0; star < 8; star++) {
            if (IsStarCollected(level, star))
                count++;
        }
    }
    return count;
}

u8 SaveData::GetCoinRecord(u32 courseID) {
    return data_0209cad2[courseID];
}

void SaveData::SetCoinRecordIfHigher(int courseID, u8 coins) {
    if (coins > data_0209cad2[courseID])
        data_0209cad2[courseID] = coins;
}

/* The minigame block's helpers, at the bottom of the TU's span. mg.unlocked
 * holds the unlocked-minigame bits: CollectMinigameKey sets bit
 * (rabbit << 2) | character there and in the file's minigameRabbits. mg.seenLo/seenHi are the minigame
 * menu's seen bits (ov005 dScMiniGm_c::IsNew reads "not seen", ClearNew sets
 * one). The record tables at block + 0x54, five words each, are what
 * src/minigames/d_s_mg_base.cpp reads and writes. */

int func_02013638(int minigame) {
    return data_0209caa0.mg.unlocked & (1 << minigame);
}

void UnlockMinigame(int minigame) {
    data_0209caa0.mg.unlocked |= 1 << minigame;
}

int func_020135d0(int minigame) {
    int res;
    if (minigame < 0x1c) {
        res = (data_0209caa0.mg.seenLo & (1 << minigame)) ? 0 : 1;
    } else {
        res = (data_0209caa0.mg.seenHi & (1 << (minigame - 0x1c))) ? 0 : 1;
    }
    return res;
}

/* For minigame < 0x1c the byte store below shifts by a negative count, which
 * on ARM sets no bit; only the word store takes effect. */
void func_02013598(int minigame) {
    if (minigame < 0x1c)
        data_0209caa0.mg.seenLo |= 1 << minigame;
    data_0209caa0.mg.seenHi |= 1 << (minigame - 0x1c);
}

int func_02013580(int table, int index) {
    return data_0209caf4[table][index];
}

void func_02013568(int table, int index, int value) {
    data_0209caf4[table][index] = value;
}
