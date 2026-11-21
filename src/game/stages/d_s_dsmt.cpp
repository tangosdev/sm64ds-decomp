//cpp
/* dScDSMT_c -- the DS Multi-Play ("DS MulTi") scene, ov007: the host side of
 * single-card download play. It brings the engines up, registers a
 * graphCallback_c with the graph, pumps the session state machine every
 * frame, answers the client's command codes (sound mode, backlight,
 * erase/copy save files, minigame records) and leaves for a save file, the
 * minigame overlays or the minigame menu when the session ends.
 *
 * 20 functions, .text 0x020cc028..0x020ccb54, one TU: the seven dScene_c
 * slots it overrides (0, 3, 6, 9, 12, 16, 17 -- the class adds no new
 * virtual), two slots of the nested dScDSMT_c::graphCallback_c, four small
 * free helpers (DSMT_GetFileBuffer, DSMT_SetFileBuffers, DSMT_SetPendingMusic
 * and DSMT_UpdateBootArgs), the menu's command callback DSMT_MenuCallback,
 * five allocator/file hooks for the menu, and the factory
 * dScDSMT_c_classInit.
 *
 * The destructor is inline in the class body and the key function is
 * InitResources, so this TU emits D1 then D0 -- the cartridge's order --
 * and both vtables. Source order is the reverse of the ROM's: mwccarm
 * emits one .text section per function in reverse source order. Do not
 * reorder.
 *
 * classInit is `return new dScDSMT_c`: the implicit constructor inlines the
 * fBase_c/dBase_c/dScene_c chain and mGraphCallback's vptr pair, and calls
 * the out-of-line arm9 dFdDummy_c::dFdDummy_c() on the fader member.
 */

// leftovers: the DSMT_* helper and hook names are coined from their bodies
// and from the Ov007MenuHooks fields (src/ui/menus/unit020b6eb4.cpp); the
// ROM keeps no names for them. They keep C linkage because the menu reaches
// the six hooks through a function-pointer table. data_0209b340 is the
// Ov007BootArgs table (Ov007Settings.h); this TU is its only reader. The
// dispatcher reads the save block's active-file byte
// (SaveBlock::activeFile). The overlay operands
// pool-load the absolute linker constants ((int)&OVERLAY_100_ID/102_ID);
// plain literals would compile to mov, and decl_common.h's overlay_N names
// do not link.

#include "dScDSMT_c.h"
#include "SpawnInfo.h"
#include "types.h"
#include "Sound.h"
#include "SaveData.h"
#include "Heap.h"
#include "decl_common.h"
#include "nitro/spi/tp.h"
#include "Memory.h"
#include "nitro/gx/gx.h"
#include "GX.h"
#include "nitro/hw/registers.h"
#include "math/CalcLib.h"
#include "Ov007Settings.h"
#include "Ov007Item.h"

extern "C" {
extern FileSaveData *data_0209b33c;
extern u8 data_0209b34b[];
extern u8 data_0209b34e[];
extern u8 data_0209b3d8[];
extern u8 gBacklightEnabled;
extern MinigameSaveData data_0209cae4;
extern u8 gSoftResetPending;
/* The scene's graph callback: the scene registers its own object here and
   clears it on the way out. The frame calls its four slots: GraphCallback0 at
   the frame start (func_02019390), GraphCallback1 before the frame wait
   (func_02019404), GraphCallback2 and GraphCallback3 in the VBlank IRQ
   (func_02019144, func_02019100). dGraph_c::callback_c is the ROM-proven base
   of every scene's nested graphCallback_c (include/dGraph_c.h). */
extern dGraph_c::callback_c *gGraphCallback;
extern int gFrameStep;
extern void *data_020a0ea0;
/* Linker-defined overlay IDs: the address is the overlay number. */
extern int OVERLAY_100_ID;
extern int OVERLAY_102_ID;

void Enable3dEngines();
int GetSoundMode(void);
void SetSoundMode(int mode);
void TurnBacklightOn(void);
void TurnBacklightOff(void);
}
FileSaveData *DSMT_GetFileBuffer(int slot);
void DSMT_SetFileBuffers(FileSaveData *files);
int DSMT_SetPendingMusic(int musicId, unsigned int delayFrames);
void DSMT_UpdateBootArgs(u32 saveSlot);
int func_ov007_020b7090(u16 a0, u16 a1, u16 a2, u16 a3, int arg4);
int func_ov007_020b6eb4(void);
int IsStarCollected(int course, int star);
void StartMinigameMenu(u8 returnToRecRoom);

/* The download-play table, and the menu hooks InitResources hands it with. */
extern Ov007BootArgs data_0209b340;
extern Ov007MenuHooks data_ov007_02103290;

/* Factory: the registry's spawn entry point. */
dScDSMT_c *dScDSMT_c_classInit(void) {
    return new dScDSMT_c;
}

/* The menu hooks. InitResources hands func_ov007_020b7138 the table at
 * data_ov007_02103290, which holds them in Ov007MenuHooks order: alloc,
 * free, query, fileLoad, fileRelease, callback.
 *
 * Heap thunks onto the game heap; `bx ip` sibcalls. */
void *DSMT_MenuAlloc(u32 size) {
    return ((Heap *)data_020a0ea0)->Allocate(size);
}

void DSMT_MenuFree(void *ptr) {
    ((Heap *)data_020a0ea0)->_Deallocate(ptr);
}

void DSMT_MenuMaxAllocSize(void) {
    ((Heap *)data_020a0ea0)->MaxAllocationUnitSize();
}

/* File-op veneers: `#pragma long_calls` emits the pooled absolute tail call. */
#pragma long_calls on
extern "C" void *LoadFile(int);
void *DSMT_MenuLoadFile(int fileId) {
    return LoadFile(fileId);
}
void DSMT_MenuReleaseFile(void *file) {
    Deallocate(file);
}
#pragma long_calls off

/* The menu's command callback: sound/backlight/save-file/minigame codes. */
u8 DSMT_MenuCallback(s32 command) {
    u8 failed = 0;

    if (command == 1) {
        SetSoundMode(2);
    } else if (command == 2) {
        SetSoundMode(0);
    } else if (command == 3) {
        SetSoundMode(1);
    } else if (command == 4) {
        TurnBacklightOn();
    } else if (command == 5) {
        TurnBacklightOff();
    } else if (command == 8) {
        SaveData::EraseSaveFile(0, (char *)DSMT_GetFileBuffer(0));
        func_02013c84(0, DSMT_GetFileBuffer(0), -1, &gSaveBlock.file);
    } else if (command == 9) {
        SaveData::EraseSaveFile(1, (char *)DSMT_GetFileBuffer(1));
        func_02013c84(1, DSMT_GetFileBuffer(1), -1, &gSaveBlock.file);
    } else if (command == 10) {
        SaveData::EraseSaveFile(2, (char *)DSMT_GetFileBuffer(2));
        func_02013c84(2, DSMT_GetFileBuffer(2), -1, &gSaveBlock.file);
    } else if (command == 11) {
        FileSaveData *fileBuffer = DSMT_GetFileBuffer(0);
        if (func_02013c84(0, fileBuffer, 1, DSMT_GetFileBuffer(1)) == 0)
            failed = 1;
        func_02013c84(1, DSMT_GetFileBuffer(1), -1, &gSaveBlock.file);
    } else if (command == 12) {
        FileSaveData *fileBuffer = DSMT_GetFileBuffer(0);
        if (func_02013c84(0, fileBuffer, 2, DSMT_GetFileBuffer(2)) == 0)
            failed = 1;
        func_02013c84(2, DSMT_GetFileBuffer(2), -1, &gSaveBlock.file);
    } else if (command == 13) {
        FileSaveData *fileBuffer = DSMT_GetFileBuffer(1);
        if (func_02013c84(1, fileBuffer, 0, DSMT_GetFileBuffer(0)) == 0)
            failed = 1;
        func_02013c84(0, DSMT_GetFileBuffer(0), -1, &gSaveBlock.file);
    } else if (command == 14) {
        FileSaveData *fileBuffer = DSMT_GetFileBuffer(1);
        if (func_02013c84(1, fileBuffer, 2, DSMT_GetFileBuffer(2)) == 0)
            failed = 1;
        func_02013c84(2, DSMT_GetFileBuffer(2), -1, &gSaveBlock.file);
    } else if (command == 15) {
        FileSaveData *fileBuffer = DSMT_GetFileBuffer(2);
        if (func_02013c84(2, fileBuffer, 0, DSMT_GetFileBuffer(0)) == 0)
            failed = 1;
        func_02013c84(0, DSMT_GetFileBuffer(0), -1, &gSaveBlock.file);
    } else if (command == 16) {
        FileSaveData *fileBuffer = DSMT_GetFileBuffer(2);
        if (func_02013c84(2, fileBuffer, 1, DSMT_GetFileBuffer(1)) == 0)
            failed = 1;
        func_02013c84(1, DSMT_GetFileBuffer(1), -1, &gSaveBlock.file);
    } else if (command == 17) {
        DSMT_SetPendingMusic(0x37, 8);
    } else if (command == 18) {
        DSMT_SetPendingMusic(0x38, 8);
    } else if (command == 20) {
        FileSaveData *files = new FileSaveData[3];
        u32 saveSlot;
        DSMT_SetFileBuffers(files);
        for (saveSlot = 0; (s32)saveSlot < 3; saveSlot++, files++) {
            if (SaveData::ReadFileData(saveSlot, files) == 0)
                failed = 1;
            func_02013c84(saveSlot, files, -1, &gSaveBlock.file);
            DSMT_UpdateBootArgs(saveSlot);
        }
    } else if (command == 21) {
        if (SaveData::ReadMinigameData(&data_0209cae4) == 0)
            failed = 1;
    } else if (command == 22) {
        s32 saveSlot;
        for (saveSlot = 0; saveSlot < 3; saveSlot++) {
            FileSaveData *fileBuffer = DSMT_GetFileBuffer(saveSlot);
            if ((fileBuffer->flags1 & 1) == 0)
                SaveData::EraseSaveFile(saveSlot, (char *)fileBuffer);
        }
    } else if (command == 23) {
        SaveData::SaveMinigames(&data_0209cae4);
    } else if (command == 19) {
        Sound::StopLoadedMusic_Layer1(60);
    }
    DSMT_UpdateBootArgs(gSaveBlock.activeFile);
    return failed;
}

/* InitResources (key function): engines on, graph callback registered, session seeded. */
s32 dScDSMT_c::InitResources() {
    if (gSoftResetPending != 0)
        data_0209b340.status = 1;

    func_02023544();
    UnloadArchives();
    func_02017e94((int)&OVERLAY_100_ID);
    func_02017e94((int)&OVERLAY_102_ID);

    if (data_0209d524 != 0)
        func_0201a428();

    data_0209b340.messages = (void *)gMessageBankBase;
    GX_SetPower(GX_POWER_ALL);
    GX_DispOn();
    GXS_DispOn();
    Enable3dEngines();
    ::Initialise3dGraphics(0);
    gGraphCallback = &mGraphCallback;
    dScene_c::SetFaders(&fader);
    gFrameStep = 1;
    GX::DisableAllBanks();
    data_ov007_02103260 = -1;
    data_ov007_02103290.func_ov007_020b7138(&data_0209b340);
    func_ov007_020b7090(0, 0, 0, 0, 0);
    Sound::LoadInitialGroup(2);
    return 1;
}

/* CleanupResources: unregister callback, record exit reason, release voice group. */
s32 dScDSMT_c::CleanupResources() {
    gGraphCallback = 0;
    dScene_c::SetAndStopColorFader();
    data_0209b340.bootFlags = func_ov007_020b6f4c();
    data_0209b340.status = 2;
    Sound::UnsetPlayerVoiceGroup();
    ::operator delete[](data_0209b33c);
    data_0209b33c = 0;
    return 1;
}

/* Behavior: pump the session state machine and dispatch its return code. */
s32 dScDSMT_c::Behavior() {
    int result;

    if (data_ov007_02103260 >= 0 && DecIfAbove0_Short(&data_ov007_02104c28) == 0) {
        Sound::LoadAndSetMusic_Layer1(data_ov007_02103260);
        data_ov007_02103260 = -1;
    }

    {
        TPData *tp = func_0203dabc(func_0203da9c());
        result = func_ov007_020b7090(tp->x, tp->y, tp->touch, tp->validity,
                                     func_0203dae4(func_0203da9c()));
    }

    if ((unsigned int)(result - 3) <= 2) {
        int saveSlot = 0;
        if (result == 4) {
            saveSlot = 1;
        } else if (result == 5) {
            saveSlot = 2;
        }
        func_02013c84(saveSlot, data_0209b33c + saveSlot, -1, &gSaveBlock.file);
        StartFile(1, 0);
    } else if (result == 6) {
        if (data_0209d524 == 0) {
            Heap *archiveHeap = (Heap *)func_0201a458();
            Heap *previousHeap = archiveHeap->SetDefault();
            LoadArchive(1);
            LoadTextNarcs();
            archiveHeap->Rescue();
            previousHeap->SetDefault();
            LoadOverlay((int)&OVERLAY_100_ID);
            LoadOverlay((int)&OVERLAY_102_ID);
        }
        dScene_c::StartSceneFade(6, 0, 0x7fff);
    } else if (result == 7) {
        StartMinigameMenu(0);
    } else if (result == 2) {
        dScene_c::StartSceneFade(1, 0, 0x7fff);
    }

    dScene_c::SetFaders(&fader);
    return 1;
}

/* Render: forward to the overlay's draw routine. */
s32 dScDSMT_c::Render() {
    func_ov007_020b7040();
    return 1;
}

/* OnPendingDestroy: no unwind; the slot exists for dScene_c's teardown. */
void dScDSMT_c::OnPendingDestroy() {}

/* Refresh the boot-args table: the sound mode and backlight, then save file
 * `saveSlot`'s summary (in use, characters, stars and coin records). */
void DSMT_UpdateBootArgs(u32 saveSlot) {
    int highestUnlockedPlusOne;
    int character;
    int course;

    if (GetSoundMode() == 0) {
        data_0209b340.soundModeItem = 1;
    } else if (GetSoundMode() == 1) {
        data_0209b340.soundModeItem = 2;
    } else if (GetSoundMode() == 2) {
        data_0209b340.soundModeItem = 0;
    }

    data_0209b340.backlightOn = gBacklightEnabled;
    data_0209b34b[saveSlot] = (gSaveBlock.file.flags1 & 1) != 0;

    highestUnlockedPlusOne = 3;
    character = 2;
    do {
        if (SaveData::IsCharacterUnlocked(character))
            break;
        highestUnlockedPlusOne--;
        character--;
    } while (character >= 0);
    data_0209b34e[saveSlot] = highestUnlockedPlusOne;
    for (course = 0; course < 0xf; course++) {
        data_0209b340.firstStar[saveSlot][course] = IsStarCollected(course, 0) != 0;
        data_0209b340.otherStars[saveSlot][course] =
            CountStarsCollectedInLevelToDisplay(course) - data_0209b340.firstStar[saveSlot][course];
        data_0209b340.coinRecords[saveSlot][course] = SaveData::GetCoinRecord(course);
    }

    data_0209b3d8[saveSlot] = CountStarsCollectedInLevelToDisplay(0x1d);
}

/* Arm the deferred music change Behavior commits. Declines if one is pending. */
int DSMT_SetPendingMusic(int musicId, unsigned int delayFrames) {
    if (data_ov007_02103260 >= 0) {
        return 0;
    }
    data_ov007_02103260 = musicId;
    data_ov007_02104c28 = delayFrames;
    Sound::StopLoadedMusic_Layer1(delayFrames);
    return 1;
}

/* graphCallback_c slot 0: decline (`mov r0,#0`), against dGraph_c's default 1. */
int dScDSMT_c::graphCallback_c::GraphCallback0() {
    return 0;
}

/* graphCallback_c slot 2: hand the callback object to the ov007 routine. */
int dScDSMT_c::graphCallback_c::GraphCallback2() {
    func_ov007_020b6eb4();
    return 0;
}

/* Install the three save-file buffers the callback reads and writes. */
void DSMT_SetFileBuffers(FileSaveData *files) {
    data_0209b33c = files;
}

/* Address of save file `slot`'s buffer (0x44 stride is in the ROM's mla). */
FileSaveData *DSMT_GetFileBuffer(int slot) {
    return &data_0209b33c[slot];
}

extern "C" {
/* Behavior consumes a nonnegative pending music ID after the delay; -1 is idle.
 * Keep the configured identity while its shared declaration is reserved. */
int data_ov007_02103260 = -1;
BaseSpawnInfo<dScDSMT_c> g_profile_DSMT = {dScDSMT_c_classInit, 1, 3};

/* The menu's memory and file hooks plus its command callback, in
 * Ov007MenuHooks order. InitResources hands the record to
 * func_ov007_020b7138, which installs the six entries. */
Ov007MenuHooks data_ov007_02103290 = {
    (int)DSMT_MenuAlloc,    (int)DSMT_MenuFree,        (int)DSMT_MenuMaxAllocSize,
    (int)DSMT_MenuLoadFile, (int)DSMT_MenuReleaseFile, (int (*)(int))DSMT_MenuCallback,
};

/* Pending-music delay, counted down by Behavior before it applies the ID. */
extern unsigned short data_ov007_02104c28;
}

/* The item-node animation index arrays. func_ov007_020ade58 reaches them only
 * through data_ov007_020ccb7c's animIndices pointers, and each keeps its ROM
 * symbol. The ROM orders them item0, item7, item4, item8, items3And6, item2,
 * item5, item1; mwcc does not emit separate constants in declaration order,
 * so the manifest records the emitted address of each and the production link
 * puts them back in ROM order. Items 3 and 6 share one array. */
extern "C" const u8 data_ov007_020ccb54[4] = {0, 0, 0, 0}; /* item 0 */
extern "C" const u8 data_ov007_020ccb58[4] = {0, 0, 0, 0}; /* item 7 */
extern "C" const u8 data_ov007_020ccb5c[4] = {1, 0, 0, 0}; /* item 4 */
extern "C" const u8 data_ov007_020ccb60[4] = {0, 0, 0, 0}; /* item 8 */
extern "C" const u8 data_ov007_020ccb64[4] = {0, 0, 0, 0}; /* items 3 and 6 */
extern "C" const u8 data_ov007_020ccb68[4] = {4, 0, 0, 0}; /* item 2 */
extern "C" const u8 data_ov007_020ccb6c[4] = {2, 0, 0, 0}; /* item 5 */
extern "C" const u8 data_ov007_020ccb70[12] = {0, 1, 2, 3, 0, 1, 2, 3, 4, 5, 0, 0}; /* item 1 */

/* One row per menu item: the sprite bank, the node's animation count and the
 * array above whose indices select entries in that bank. */
extern "C" const Ov007NodeSprites data_ov007_020ccb7c[9] = {
    {5, 1, {0}, data_ov007_020ccb54}, {0, 10, {0}, data_ov007_020ccb70},
    {0, 1, {0}, data_ov007_020ccb68}, {3, 1, {0}, data_ov007_020ccb64},
    {3, 1, {0}, data_ov007_020ccb5c}, {3, 1, {0}, data_ov007_020ccb6c},
    {3, 1, {0}, data_ov007_020ccb64}, {4, 1, {0}, data_ov007_020ccb58},
    {0, 1, {0}, data_ov007_020ccb60},
};

/* The nine item placement rows func_ov007_020ade58 hands
 * func_ov007_020aea6c: kind, home position and slide distance. */
extern "C" const Ov007ItemLayout data_ov007_020ccbc4[9] = {
    {1, 12, 176, 0, 64}, {1, 128, 164, 0, 64}, {1, 232, 11, 0, -32},
    {5, 33, 93, 0, 0},   {5, 162, 61, 0, 0},   {5, 230, 78, 0, 0},
    {5, 213, 67, 0, 0},  {7, 14, 40, 0, -256}, {9, 128, 136, 0, 100},
};

/* Menu selection table: [selected slot * 3 + target slot] -> item id. */
extern "C" const int data_ov007_020ccc20[9] = {
    0, 11, 12, 13, 0, 14, 15, 16, 0,
};

/* BG map palette index for the unselected (0) and selected (1) mode. */
extern "C" const u8 data_ov007_020ccc44[4] = {3, 4, 0, 0};
