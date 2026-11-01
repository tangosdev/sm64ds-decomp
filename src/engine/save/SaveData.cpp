//cpp
/* leftovers:
 * - func_02013c84 stays `extern "C"` under its cartridge name: callers in
 *   other TUs link it unmangled, and its body forwards to the member SaveFile.
 * - func_02013e64 likewise: the save-block wipe is a free function, kept C.
 * - func_0205a588 (SDK byte-fill) and CpuCopy8 (SDK unaligned copy) are asm
 *   primitives declared `void(void)` at their definitions; the parameterised
 *   local externs below are how callers spell them, as in every shard.
 */
/* arm9/SaveData -- the save-file manager: coin and star records, cannons,
 * character intros, cap-loss flags, file slots and the minigame block,
 * folded at .text 0x02013568..0x02013e80. The span also carries the free
 * functions that
 * share this TU's concern: func_02013c84 copies a file block and either
 * records the character or forwards to SaveFile, func_02013e64 zeroes the
 * whole save block, and func_020139b8..func_02013a88 are the cap-flag
 * helpers (clear/set/test the 0x8000000<<character and 0x1000000<<character
 * bits) around NumGlowingRabbitsFound. mwccarm emits .text in reverse source order, so the
 * definitions below run ROM-descending; the roster reads ROM-ascending.
 *
 *   func_02013568..func_02013638 -- minigame record tables, seen and unlock bits.
 *   SetCoinRecordIfHigher..func_02013944 -- coin and star records, cannons,
 *     minigame rabbits, character intros, the rabbit-count bit.
 *   NumGlowingRabbitsFound -- rabbit bits 20..27 of the flags word.
 *   func_020139b8..func_02013a88 -- cap-flag clear/set/test helpers.
 *   PlayerLoseCap..CanPlayerHaveCap -- cap-loss flags on the live block.
 *   SaveCurrentFile..SetDefaultValuesMg -- file and minigame writes.
 *   func_02013c84 -- block copy + character record / SaveFile forwarder.
 *   EraseSaveFile..EraseAllSaveData -- slot and whole-block resets.
 *   func_02013e64 -- zero the save block.
 *
 * Every method is static: the ROM bodies take no `this` and reach the single
 * save block at 0x0209caa0 directly. The TU declares it once as a SaveBlock
 * object: the current file (FileSaveData, 0x44 bytes), the minigame block
 * (MinigameSaveData, 0x2e4 bytes, cart slot 3) and the active-file byte at
 * 0x328. The declaration form matters to the bytes: on an `int[]` view
 * mwccarm keeps the block address in one register across func_02013598's
 * branch, while the ROM reloads it per arm, which is what member access on a
 * struct object compiles to. The star bits (0x14), coin records (0x32) and
 * record tables (0x54) are reached through their own data_ symbols, as the
 * ROM's relocations name them.
 */

#include "SaveData.h"

/* The minigame block: 0x2e4 bytes at block + 0x44, saved to cart slot 3. */
struct MinigameSaveData {
    u32 magic5000; /* 0x00 (block 0x44) -- "5000", written by SetDefaultValuesMg */
    /* 0x04 (block 0x48) -- unlocked minigames, one bit each in bits 0..0x1b
       (func_0201361c). Bit 31 is not a minigame: dScMiniGm_c's init
       (d_s_mini_gm.cpp) sets it once it has written the default records. */
    u32 unlocked;
    u32 seenLo; /* 0x08 (block 0x4c) -- menu "seen" bits for minigames 0..0x1b */
    u8 seenHi;  /* 0x0c (block 0x50) -- the same for minigames 0x1c.. */
    u8 pad_00d[0x2e4 - 0xd];
};

/* The live save block at 0x0209caa0: the current file, the minigame block and
   the active-file byte; func_02013e64 clears all 0x32c bytes. */
struct SaveBlock {
    FileSaveData file;   /* 0x000 */
    MinigameSaveData mg; /* 0x044 */
    u8 activeFile;       /* 0x328 */
};

extern "C" {
extern SaveBlock data_0209caa0;       /* the live save block */
extern int data_0209caf4[][5];        /* the minigame record tables, at block + 0x54 */
extern u8 data_0209f2d8[];            /* gate byte read by CanPlayerHaveCap */
extern u8 data_0209cad2[];            /* coin records; local extern: no field on the byte array */
extern unsigned char data_0209cab4[]; /* star bits per course */
extern unsigned char data_0209f224;   /* last collected star id */
extern void func_0205a588(void *dest, int value, int size);
extern void CpuCopy8(void *dst, void *src, u32 count);
extern void func_020139b8(void);      /* forward: called by func_02013a88 above its definition */
extern "C" void func_0201361c(int n); /* forward: defined at the end of this file */
/* local extern: src/SublevelToLevel.cpp is int(int), but these four callers match only as s8.
 * An int declaration widens each of them by 8 bytes. */
extern s8 SublevelToLevel(s8 levelID);
}

// @symbol func_02013e64
extern "C" void func_02013e64(void) {
    func_0205a588(&data_0209caa0, 0, 0x32c);
}

// @symbol _ZN8SaveData16EraseAllSaveDataEv
u32 SaveData::EraseAllSaveData() {
    u32 rc;
    rc = SaveData::EraseSaveFile(0, (char *)&data_0209caa0.file);
    rc |= SaveData::EraseSaveFile(1, (char *)&data_0209caa0.file);
    rc |= SaveData::EraseSaveFile(2, (char *)&data_0209caa0.file);
    SaveData::SetDefaultValuesMg(&data_0209caa0.mg);
    return rc | SaveData::SaveMinigames(&data_0209caa0.mg);
}

// @symbol _ZN8SaveData16SetDefaultValuesEP12FileSaveData
void SaveData::SetDefaultValues(FileSaveData *fsd_) {
    func_0205a588(fsd_, 0, 0x44);
    fsd_->magic8000 = 0x30303038;
    fsd_->mCharacter = 3;
    fsd_->flags2 |= 8;
    fsd_->controllerMode = 0;
}

// @symbol _ZN8SaveData12ReadFileDataEjP12FileSaveData
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

// @symbol _ZN8SaveData8SaveFileEjP12FileSaveData
int SaveData::SaveFile(u32 fileID, FileSaveData *data) {
    data->flags1 |= 1;
    if (SaveData::SaveDataToCart((char *)data, 0x44, fileID) == 0)
        return 1;
    return 0;
}

// @symbol _ZN8SaveData13EraseSaveFileEjPc
int SaveData::EraseSaveFile(u32 fileID, char *saveArea) {
    SaveData::SetDefaultValues((FileSaveData *)saveArea);
    if (SaveData::SaveDataToCart(saveArea, 0x44, fileID) == 0)
        return 1;
    return 0;
}

// @symbol func_02013c84
extern "C" int func_02013c84(u8 charID, FileSaveData *dest, s32 fileIndex, FileSaveData *src) {
    CpuCopy8(dest, src, 0x44);
    if (fileIndex < 0) {
        data_0209caa0.activeFile = charID;
        return 1;
    }
    return SaveData::SaveFile(fileIndex, src);
}

// @symbol _ZN8SaveData18SetDefaultValuesMgEP16MinigameSaveData
void SaveData::SetDefaultValuesMg(MinigameSaveData *mg_) {
    func_0205a588(mg_, 0, 0x2e4);
    mg_->magic5000 = 0x30303035;
}

// @symbol _ZN8SaveData16ReadMinigameDataEP16MinigameSaveData
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

// @symbol _ZN8SaveData13SaveMinigamesEP16MinigameSaveData
int SaveData::SaveMinigames(MinigameSaveData *data) {
    if (SaveData::SaveDataToCart((char *)data, 0x2e4, 3) == 0)
        return 1;
    return 0;
}

// @symbol _ZN8SaveData15SaveCurrentFileEv
int SaveData::SaveCurrentFile() {
    if (!SaveData::SaveFile(data_0209caa0.activeFile, &data_0209caa0.file))
        return 0;
    return SaveData::SaveMinigames(&data_0209caa0.mg);
}

// @symbol _ZN8SaveData16CanPlayerHaveCapEv
int SaveData::CanPlayerHaveCap() {
    if (data_0209caa0.file.mCharacter != 3) {
        int b = (int)(data_0209f2d8[0] == 1);
        if (b == 0)
            return 1;
    }
    return 0;
}

// @symbol _ZN8SaveData16HasPlayerLostCapEv
int SaveData::HasPlayerLostCap() {
    if (!SaveData::CanPlayerHaveCap())
        return 0;
    return data_0209caa0.file.flags1 & (0x1000000u << data_0209caa0.file.mCharacter);
}

// @symbol _ZN8SaveData13PlayerLoseCapEv
void SaveData::PlayerLoseCap() {
    if (!SaveData::CanPlayerHaveCap())
        return;
    data_0209caa0.file.flags1 |= (0x1000000u << data_0209caa0.file.mCharacter);
}

// @symbol func_02013a88
extern "C" void func_02013a88(void) {
    if (!SaveData::CanPlayerHaveCap())
        return;
    data_0209caa0.file.flags1 &= ~(0x1000000u << data_0209caa0.file.mCharacter);
    func_020139b8();
}

// @symbol func_02013a44
extern "C" int func_02013a44(void) {
    if (!SaveData::CanPlayerHaveCap())
        return 0;
    return data_0209caa0.file.flags1 & (0x8000000u << data_0209caa0.file.mCharacter);
}

// @symbol func_02013a00
extern "C" void func_02013a00(void) {
    if (!SaveData::CanPlayerHaveCap())
        return;
    data_0209caa0.file.flags1 |= (0x8000000u << data_0209caa0.file.mCharacter);
}

// @symbol func_020139b8
extern "C" void func_020139b8(void) {
    if (!SaveData::CanPlayerHaveCap())
        return;
    data_0209caa0.file.flags1 &= ~(0x8000000u << data_0209caa0.file.mCharacter);
}

// @symbol _ZN8SaveData22NumGlowingRabbitsFoundEv
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
// @symbol func_02013944
extern "C" void func_02013944(void) {
    u32 count = SaveData::NumGlowingRabbitsFound();
    data_0209caa0.file.flags2 |= (0x100000u << count);
    if ((s32)count >= 7)
        data_0209caa0.file.flags1 |= 0x80u;
}

// @symbol _ZN8SaveData19IsCharacterUnlockedEj
s32 SaveData::IsCharacterUnlocked(u32 character) {
    return data_0209caa0.file.flags2 & (1 << character);
}

// @symbol _ZN8SaveData17SetCharacterIntroEi
void SaveData::SetCharacterIntro(s32 character) {
    data_0209caa0.file.flags2 |= 1 << character;
}

// @symbol func_020138dc
extern "C" int func_020138dc(void) {
    int count = 0;
    int i;
    int f = data_0209caa0.file.minigameRabbits;
    for (i = 0; i < 0x1c; i++) {
        if (f & (1 << i))
            count++;
    }
    return count;
}

// @symbol func_02013890
extern "C" int func_02013890(int a, int b) {
    if (b < 0) {
        int f = data_0209caa0.file.minigameRabbits;
        int m = 0xf << (a * 4);
        return (f & m) == m;
    } else {
        int f = data_0209caa0.file.minigameRabbits;
        return f & (1 << (b | (a << 2)));
    }
}

// @symbol func_02013868
extern "C" void func_02013868(int a, int b) {
    int idx = (a << 2) | b;
    data_0209caa0.file.minigameRabbits |= 1 << idx;
    func_0201361c(idx);
}

// @symbol IsCannonOpen
extern "C" int IsCannonOpen(int n) {
    return data_0209caa0.file.cannonFlags & (1 << n);
}

// @symbol OpenCannon
extern "C" void OpenCannon(int n) {
    data_0209caa0.file.cannonFlags |= 1 << n;
}

// @symbol IsCannonOpenInLevel
extern "C" int IsCannonOpenInLevel(s8 levelID) {
    s8 courseID = SublevelToLevel(levelID);
    return IsCannonOpen(courseID);
}

// @symbol OpenCannonInLevel
extern "C" void OpenCannonInLevel(s8 levelID) {
    s8 courseID = SublevelToLevel(levelID);
    OpenCannon(courseID);
}

// @symbol IsStarCollected
extern "C" int IsStarCollected(s32 courseID, s32 starID) {
    return ((1 << starID) & data_0209cab4[courseID]) & 0xff;
}

// @symbol CollectStar
extern "C" void CollectStar(int a, int b) {
    data_0209cab4[a] |= 1 << b;
    data_0209f224 = b;
}

// @symbol _ZN8SaveData26CountStarsCollectedInLevelEj
u8 SaveData::CountStarsCollectedInLevel(u32 courseID) {
    u8 count = 0;
    s32 star;
    for (star = 0; star < 8; star++) {
        if (IsStarCollected(courseID, star))
            count++;
    }
    return count;
}

// @symbol CountStarsCollectedInLevelToDisplay
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

// @symbol IsStarCollectedInLevel
extern "C" int IsStarCollectedInLevel(s8 levelID, s32 starID) {
    s8 courseID = SublevelToLevel(levelID);
    return IsStarCollected(courseID, starID);
}

// @symbol CollectStarInLevel
extern "C" void CollectStarInLevel(s8 levelID, s32 starID) {
    s8 courseID = SublevelToLevel(levelID);
    CollectStar(courseID, starID);
}

// @symbol NumStars
extern "C" u8 NumStars(void) {
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

// @symbol _ZN8SaveData13GetCoinRecordEj
u8 SaveData::GetCoinRecord(u32 courseID) {
    return data_0209cad2[courseID];
}

// @symbol _ZN8SaveData21SetCoinRecordIfHigherEih
void SaveData::SetCoinRecordIfHigher(int courseID, u8 coins) {
    if (coins > data_0209cad2[courseID])
        data_0209cad2[courseID] = coins;
}

/* The minigame block's helpers, at the bottom of the TU's span. mg.unlocked
 * holds the unlocked-minigame bits: func_02013868 sets bit (a << 2) | b there
 * and in the file's minigameRabbits. mg.seenLo/seenHi are the minigame
 * menu's seen bits (ov005 MiniGm_IsNew reads "not seen", MiniGm_ClearNew sets
 * one). The record tables at block + 0x54, five words each, are what
 * src/minigames/d_s_mg_base.cpp reads and writes. */

// @symbol func_02013638
extern "C" int func_02013638(int n) {
    return data_0209caa0.mg.unlocked & (1 << n);
}

// @symbol func_0201361c
extern "C" void func_0201361c(int n) {
    data_0209caa0.mg.unlocked |= 1 << n;
}

// @symbol func_020135d0
extern "C" int func_020135d0(int n) {
    int res;
    if (n < 0x1c) {
        res = (data_0209caa0.mg.seenLo & (1 << n)) ? 0 : 1;
    } else {
        res = (data_0209caa0.mg.seenHi & (1 << (n - 0x1c))) ? 0 : 1;
    }
    return res;
}

/* For n < 0x1c the byte store below shifts by a negative count, which on ARM
 * sets no bit; only the word store takes effect. */
// @symbol func_02013598
extern "C" void func_02013598(int n) {
    if (n < 0x1c)
        data_0209caa0.mg.seenLo |= 1 << n;
    data_0209caa0.mg.seenHi |= 1 << (n - 0x1c);
}

// @symbol func_02013580
extern "C" int func_02013580(int table, int index) {
    return data_0209caf4[table][index];
}

// @symbol func_02013568
extern "C" void func_02013568(int table, int index, int value) {
    data_0209caf4[table][index] = value;
}
