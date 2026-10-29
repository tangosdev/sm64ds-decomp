//cpp
/* leftovers:
 * - The flag read-modify-writes spell the load `(int)->flags1` (or flags2):
 *   a plain `flags1 | bit` or `flags1 |= bit` makes mwccarm materialise the
 *   field address in a register first instead of the ROM's two direct
 *   `ldr [r0,#4]` hits. SetDefaultValues' `flags2 |= 8` needs no cast.
 * - SaveFile and ReadFileData keep their materialised-address forms: the
 *   `int* p = ...; *p = *p | ...` two-step is what the ROM compiled. Folding
 *   the offset into the store changes the instruction.
 * - func_02013c84 stays `extern "C"` under its cartridge name: callers in
 *   other TUs link it unmangled, and its body forwards to the member SaveFile.
 * - func_02013e64 likewise: the save-block wipe is a free function, kept C.
 * - func_0205a588 (SDK byte-fill) and CpuCopy8 (SDK unaligned copy) are asm
 *   primitives declared `void(void)` at their definitions; the parameterised
 *   local externs below are how callers spell them, as in every shard.
 */
/* arm9/SaveData -- the save-file manager: coin and star records, cannons,
 * character intros, cap-loss flags, file slots and the minigame block,
 * folded at .text 0x02013650..0x02013e80. The span also carries the free
 * functions that
 * share this TU's concern: func_02013c84 copies a file block and either
 * records the character or forwards to SaveFile, func_02013e64 zeroes the
 * whole save block, and func_020139b8..func_02013a88 are the cap-flag
 * helpers (clear/set/test the 0x8000000<<character and 0x1000000<<character
 * bits) around NumGlowingRabbitsFound. mwccarm emits .text in reverse source order, so the
 * definitions below run ROM-descending; the roster reads ROM-ascending.
 *
 *   NumGlowingRabbitsFound -- rabbit bits 20..27 of the flags word.
 *   func_020139b8..func_02013a88 -- cap-flag clear/set/test helpers.
 *   PlayerLoseCap..CanPlayerHaveCap -- cap-loss flags on the live block.
 *   SaveCurrentFile..SetDefaultValuesMg -- file and minigame writes.
 *   func_02013c84 -- block copy + character record / SaveFile forwarder.
 *   EraseSaveFile..EraseAllSaveData -- slot and whole-block resets.
 *   func_02013e64 -- zero the save block.
 *
 * Every method is static: the ROM bodies take no `this` and reach the single
 * save block at 0x0209caa0 directly. The TU declares it once as an int array.
 * flags1, flags2 and mCharacter are reached as members of the block cast to
 * SaveData. The words with no header name stay indexes (the word at 0xc is
 * [3], the cannon word is [4]): a byte-pointer plus a constant folds that
 * constant into the relocation addend, and the ROM keeps the offset in the
 * load. The minigame block sits at byte 0x44, which on this array is
 * `data_0209caa0 + 0x11` and still pools addend 0x44; `(SaveData*)block + 1`
 * builds 4 bytes longer. The active-slot byte at 0x328 is read through an
 * unsigned char view.
 */

#include "SaveData.h"

extern "C" {
extern int data_0209caa0[];           /* the live save block, one word per element */
extern u8 data_0209f2d8[];            /* gate byte read by CanPlayerHaveCap */
extern u8 data_0209cad2[];            /* coin records; local extern: no field on the byte array */
extern unsigned char data_0209cab4[]; /* star bits per course */
extern unsigned char data_0209f224;   /* last collected star id */
extern void func_0205a588(void* dest, int value, int size);
extern void CpuCopy8(void* dst, void* src, u32 count);
extern void func_020139b8(void);      /* forward: called by func_02013a88 above its definition */
extern void func_0201361c(int idx);   /* local extern: below this TU, at 0x0201361c */
/* local extern: src/SublevelToLevel.c is int(int), but these four callers match only as s8.
 * An int declaration widens each of them by 8 bytes. */
extern s8 SublevelToLevel(s8 levelID);
}

// @symbol func_02013e64
extern "C" void func_02013e64(void)
{
    func_0205a588(data_0209caa0, 0, 0x32c);
}

// @symbol _ZN8SaveData16EraseAllSaveDataEv
u32 SaveData::EraseAllSaveData()
{
    u32 rc;
    rc = SaveData::EraseSaveFile(0, (char*)data_0209caa0);
    rc |= SaveData::EraseSaveFile(1, (char*)data_0209caa0);
    rc |= SaveData::EraseSaveFile(2, (char*)data_0209caa0);
    SaveData::SetDefaultValuesMg((MinigameSaveData*)(data_0209caa0 + 0x11));
    return rc | SaveData::SaveMinigames((MinigameSaveData*)(data_0209caa0 + 0x11));
}

// @symbol _ZN8SaveData16SetDefaultValuesEP12FileSaveData
void SaveData::SetDefaultValues(FileSaveData* fsd_)
{
    func_0205a588(((void*)fsd_), 0, 0x44);
    *(int*)((void*)fsd_) = 0x30303038;      /* magic8000 */
    ((SaveData *)fsd_)->mCharacter = 3;
    ((SaveData *)fsd_)->flags2 |= 8;
    *(unsigned char*)((char*)fsd_ + 0x42) = 0;
}

// @symbol _ZN8SaveData12ReadFileDataEjP12FileSaveData
int SaveData::ReadFileData(u32 fileID, FileSaveData* dest)
{
    char* buf = (char*)dest;
    s32 result = SaveData::ReadDataFromCart(buf, 0x44, fileID);
    if (result) {
        SaveData::SetDefaultValues((FileSaveData*)buf);
        if (result == 2)
            return 1;
        return 0;
    }
    {
        int* p = (int*)(buf + 0xc);
        *p = *p & *(int*)((char*)&data_0209caa0 + 0x48);
    }
    return 1;
}

// @symbol _ZN8SaveData8SaveFileEjP12FileSaveData
int SaveData::SaveFile(u32 fileID, FileSaveData* data)
{
    int *ip = (int *)&((SaveData *)data)->flags1;
    *ip = *ip | 1;
    if (SaveData::SaveDataToCart((char*)data, 0x44, fileID) == 0)
        return 1;
    return 0;
}

// @symbol _ZN8SaveData13EraseSaveFileEjPc
int SaveData::EraseSaveFile(u32 fileID, char* saveArea)
{
    SaveData::SetDefaultValues((FileSaveData*)saveArea);
    if (SaveData::SaveDataToCart(saveArea, 0x44, fileID) == 0)
        return 1;
    return 0;
}

// @symbol func_02013c84
extern "C" int func_02013c84(u8 charID, FileSaveData* dest, s32 fileIndex, FileSaveData* src)
{
    CpuCopy8(dest, src, 0x44);
    if (fileIndex < 0) {
        ((unsigned char *)data_0209caa0)[0x328] = charID;
        return 1;
    }
    return SaveData::SaveFile(fileIndex, src);
}

// @symbol _ZN8SaveData18SetDefaultValuesMgEP16MinigameSaveData
void SaveData::SetDefaultValuesMg(MinigameSaveData * mg_)
{
  func_0205a588(((void*)mg_), 0, 0x2e4);
  *(int*)((void*)mg_) = 0x30303035;
}

// @symbol _ZN8SaveData16ReadMinigameDataEP16MinigameSaveData
int SaveData::ReadMinigameData(MinigameSaveData* dest)
{
    s32 result = SaveData::ReadDataFromCart((char*)dest, 0x2e4, 3);
    if (result) {
        SaveData::SetDefaultValuesMg(dest);
        if (result == 2)
            return 1;
        return 0;
    }
    return 1;
}

// @symbol _ZN8SaveData13SaveMinigamesEP16MinigameSaveData
int SaveData::SaveMinigames(MinigameSaveData* data)
{
    if (SaveData::SaveDataToCart((char*)data, 0x2e4, 3) == 0)
        return 1;
    return 0;
}

// @symbol _ZN8SaveData15SaveCurrentFileEv
int SaveData::SaveCurrentFile()
{
    if (!SaveData::SaveFile(((unsigned char *)data_0209caa0)[0x328], (FileSaveData*)data_0209caa0))
        return 0;
    return SaveData::SaveMinigames((MinigameSaveData*)(data_0209caa0 + 0x11));
}

// @symbol _ZN8SaveData16CanPlayerHaveCapEv
int SaveData::CanPlayerHaveCap()
{
    if (((SaveData*)data_0209caa0)->mCharacter != 3) {
        int b = (int)(data_0209f2d8[0] == 1);
        if (b == 0)
            return 1;
    }
    return 0;
}

// @symbol _ZN8SaveData16HasPlayerLostCapEv
int SaveData::HasPlayerLostCap()
{
    if (!SaveData::CanPlayerHaveCap())
        return 0;
    return ((SaveData*)data_0209caa0)->flags1 & (0x1000000u << ((SaveData*)data_0209caa0)->mCharacter);
}

// @symbol _ZN8SaveData13PlayerLoseCapEv
void SaveData::PlayerLoseCap()
{
    if (!SaveData::CanPlayerHaveCap())
        return;
    ((SaveData *)data_0209caa0)->flags1 = (int)((SaveData *)data_0209caa0)->flags1 |
                                          (0x1000000u << ((SaveData *)data_0209caa0)->mCharacter);
}

// @symbol func_02013a88
extern "C" void func_02013a88(void)
{
    if (!SaveData::CanPlayerHaveCap())
        return;
    ((SaveData *)data_0209caa0)->flags1 = (int)((SaveData *)data_0209caa0)->flags1 &
                                          ~(0x1000000u << ((SaveData *)data_0209caa0)->mCharacter);
    func_020139b8();
}

// @symbol func_02013a44
extern "C" int func_02013a44(void)
{
    if (!SaveData::CanPlayerHaveCap())
        return 0;
    return ((SaveData *)data_0209caa0)->flags1 &
           (0x8000000u << ((SaveData *)data_0209caa0)->mCharacter);
}

// @symbol func_02013a00
extern "C" void func_02013a00(void)
{
    if (!SaveData::CanPlayerHaveCap())
        return;
    ((SaveData *)data_0209caa0)->flags1 = (int)((SaveData *)data_0209caa0)->flags1 |
                                          (0x8000000u << ((SaveData *)data_0209caa0)->mCharacter);
}

// @symbol func_020139b8
extern "C" void func_020139b8(void)
{
    if (!SaveData::CanPlayerHaveCap())
        return;
    ((SaveData *)data_0209caa0)->flags1 = (int)((SaveData *)data_0209caa0)->flags1 &
                                          ~(0x8000000u << ((SaveData *)data_0209caa0)->mCharacter);
}

// @symbol _ZN8SaveData22NumGlowingRabbitsFoundEv
int SaveData::NumGlowingRabbitsFound()
{
    int count = 0;
    int f = ((SaveData *)data_0209caa0)->flags2;
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
extern "C" void func_02013944(void)
{
    u32 count = SaveData::NumGlowingRabbitsFound();
    ((SaveData *)data_0209caa0)->flags2 =
        (int)((SaveData *)data_0209caa0)->flags2 | (0x100000u << count);
    if ((s32)count >= 7)
        ((SaveData *)data_0209caa0)->flags1 = (int)((SaveData *)data_0209caa0)->flags1 | 0x80u;
}

// @symbol _ZN8SaveData19IsCharacterUnlockedEj
s32 SaveData::IsCharacterUnlocked(u32 character)
{
    return ((SaveData *)data_0209caa0)->flags2 & (1 << character);
}

// @symbol _ZN8SaveData17SetCharacterIntroEi
void SaveData::SetCharacterIntro(s32 character)
{
    ((SaveData *)data_0209caa0)->flags2 = (int)((SaveData *)data_0209caa0)->flags2 | 1 << character;
}

// @symbol func_020138dc
extern "C" int func_020138dc(void)
{
    int count = 0;
    int i;
    int f = data_0209caa0[3];
    for (i = 0; i < 0x1c; i++) {
        if (f & (1 << i))
            count++;
    }
    return count;
}

// @symbol func_02013890
extern "C" int func_02013890(int a, int b)
{
    if (b < 0) {
        int f = data_0209caa0[3];
        int m = 0xf << (a * 4);
        return (f & m) == m;
    } else {
        int f = data_0209caa0[3];
        return f & (1 << (b | (a << 2)));
    }
}

// @symbol func_02013868
extern "C" void func_02013868(int a, int b)
{
    int idx = (a << 2) | b;
    data_0209caa0[3] |= 1 << idx;
    func_0201361c(idx);
}

// @symbol IsCannonOpen
extern "C" int IsCannonOpen(int n)
{
    return data_0209caa0[4] & (1 << n);
}

// @symbol OpenCannon
extern "C" void OpenCannon(int n)
{
    data_0209caa0[4] |= 1 << n;
}

// @symbol IsCannonOpenInLevel
extern "C" int IsCannonOpenInLevel(s8 levelID)
{
    s8 courseID = SublevelToLevel(levelID);
    return IsCannonOpen(courseID);
}

// @symbol OpenCannonInLevel
extern "C" void OpenCannonInLevel(s8 levelID)
{
    s8 courseID = SublevelToLevel(levelID);
    OpenCannon(courseID);
}

// @symbol IsStarCollected
extern "C" int IsStarCollected(s32 courseID, s32 starID)
{
    return ((1 << starID) & data_0209cab4[courseID]) & 0xff;
}

// @symbol CollectStar
extern "C" void CollectStar(int a, int b)
{
    data_0209cab4[a] |= 1 << b;
    data_0209f224 = b;
}

// @symbol _ZN8SaveData26CountStarsCollectedInLevelEj
u8 SaveData::CountStarsCollectedInLevel(u32 courseID)
{
    u8 count = 0;
    s32 star;
    for (star = 0; star < 8; star++) {
        if (IsStarCollected(courseID, star))
            count++;
    }
    return count;
}

// @symbol CountStarsCollectedInLevelToDisplay
extern "C" u8 CountStarsCollectedInLevelToDisplay(s32 courseID)
{
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
extern "C" int IsStarCollectedInLevel(s8 levelID, s32 starID)
{
    s8 courseID = SublevelToLevel(levelID);
    return IsStarCollected(courseID, starID);
}

// @symbol CollectStarInLevel
extern "C" void CollectStarInLevel(s8 levelID, s32 starID)
{
    s8 courseID = SublevelToLevel(levelID);
    CollectStar(courseID, starID);
}

// @symbol NumStars
extern "C" u8 NumStars(void)
{
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
u8 SaveData::GetCoinRecord(u32 courseID)
{
    return data_0209cad2[courseID];
}

// @symbol _ZN8SaveData21SetCoinRecordIfHigherEih
void SaveData::SetCoinRecordIfHigher(int courseID, u8 coins) {
    if (coins > data_0209cad2[courseID])
        data_0209cad2[courseID] = coins;
}
