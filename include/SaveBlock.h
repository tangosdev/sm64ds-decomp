/* The save block's record types: FileSaveData (one save file, the type the
 * ROM's mangled SaveData signatures name, `EP12FileSaveData`),
 * MinigameSaveData and SaveBlock (the live block at 0x0209caa0 they make up).
 * Offsets/widths are observed, not guessed.
 *
 * They live apart from SaveData.h so a TU can read the block without taking
 * the SaveData class: that class's named parameter declarations consume
 * compiler numbers, so including SaveData.h in a TU that did not already
 * include it renumbers that TU's `@NNN` locals. */
#ifndef SAVEBLOCK_H
#define SAVEBLOCK_H
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

/* The one save block, data_0209caa0, is declared `extern SaveBlock` by each
   reader rather than here: d_a_ply_methods.cpp still declares block-scope
   `int[]`/`u8[]` views of it, which a declaration in this header would
   contradict. Readers reach its fields as members. On an `int[]` view mwccarm
   keeps the block address in one register across SaveData's func_02013598
   branch, while the ROM reloads it per arm, which is what member access on the
   struct compiles to. */

#endif

#endif
