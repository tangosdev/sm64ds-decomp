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
// the six hooks through a function-pointer table. data_0209b340
// is decl_common.h's int[], but it is the Ov007BootArgs table
// (Ov007Settings.h), so every access goes through the sBootArgs view. The
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


extern FileSaveData *data_0209b33c;
extern u8 data_0209b34b[];
extern u8 data_0209b34e[];
extern u8 data_0209b3d8[];
extern u8 data_0208ee3c;
extern MinigameSaveData data_0209cae4;
extern u8 gSoftResetPending;
/* The scene's graph callback: the scene registers its own object here and
   clears it on the way out. The frame calls its four slots: GraphCallback0 at
   the frame start (func_02019390), GraphCallback1 before the frame wait
   (func_02019404), GraphCallback2 and GraphCallback3 in the VBlank IRQ
   (func_02019144, func_02019100). dGraph_c::callback_c is the ROM-proven base
   of every scene's nested graphCallback_c (include/dGraph_c.h). */
extern dGraph_c::callback_c *gGraphCallback;
extern int data_0208ee44;
extern void *data_020a0ea0;
/* Linker-defined overlay IDs: the address is the overlay number. */
extern int OVERLAY_100_ID;
extern int OVERLAY_102_ID;

FileSaveData *DSMT_GetFileBuffer(int slot);
void DSMT_SetFileBuffers(FileSaveData *files);
int DSMT_SetPendingMusic(int musicId, unsigned int delayFrames);
void DSMT_UpdateBootArgs(u32 saveSlot);
int func_ov007_020b7090(u16 a0, u16 a1, u16 a2, u16 a3, int arg4);
int func_ov007_020b6eb4(void);
extern "C" void Enable3dEngines();
extern "C" int GetSoundMode(void);
int IsStarCollected(int course, int star);
void StartMinigameMenu(u8 returnToRecRoom);
extern "C" void SetSoundMode(int mode);
extern "C" void TurnBacklightOn(void);
extern "C" void TurnBacklightOff(void);


/* data_0209b340 (decl_common.h's int[]) is the download-play table. */
#define sBootArgs (*(Ov007BootArgs *)data_0209b340)

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
    (void)((Heap *)data_020a0ea0)->MaxAllocationUnitSize();
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
        func_02013c84(0, DSMT_GetFileBuffer(0), -1, &data_0209caa0.file);
    } else if (command == 9) {
        SaveData::EraseSaveFile(1, (char *)DSMT_GetFileBuffer(1));
        func_02013c84(1, DSMT_GetFileBuffer(1), -1, &data_0209caa0.file);
    } else if (command == 10) {
        SaveData::EraseSaveFile(2, (char *)DSMT_GetFileBuffer(2));
        func_02013c84(2, DSMT_GetFileBuffer(2), -1, &data_0209caa0.file);
    } else if (command == 11) {
        FileSaveData *fileBuffer = DSMT_GetFileBuffer(0);
        if (func_02013c84(0, fileBuffer, 1, DSMT_GetFileBuffer(1)) == 0)
            failed = 1;
        func_02013c84(1, DSMT_GetFileBuffer(1), -1, &data_0209caa0.file);
    } else if (command == 12) {
        FileSaveData *fileBuffer = DSMT_GetFileBuffer(0);
        if (func_02013c84(0, fileBuffer, 2, DSMT_GetFileBuffer(2)) == 0)
            failed = 1;
        func_02013c84(2, DSMT_GetFileBuffer(2), -1, &data_0209caa0.file);
    } else if (command == 13) {
        FileSaveData *fileBuffer = DSMT_GetFileBuffer(1);
        if (func_02013c84(1, fileBuffer, 0, DSMT_GetFileBuffer(0)) == 0)
            failed = 1;
        func_02013c84(0, DSMT_GetFileBuffer(0), -1, &data_0209caa0.file);
    } else if (command == 14) {
        FileSaveData *fileBuffer = DSMT_GetFileBuffer(1);
        if (func_02013c84(1, fileBuffer, 2, DSMT_GetFileBuffer(2)) == 0)
            failed = 1;
        func_02013c84(2, DSMT_GetFileBuffer(2), -1, &data_0209caa0.file);
    } else if (command == 15) {
        FileSaveData *fileBuffer = DSMT_GetFileBuffer(2);
        if (func_02013c84(2, fileBuffer, 0, DSMT_GetFileBuffer(0)) == 0)
            failed = 1;
        func_02013c84(0, DSMT_GetFileBuffer(0), -1, &data_0209caa0.file);
    } else if (command == 16) {
        FileSaveData *fileBuffer = DSMT_GetFileBuffer(2);
        if (func_02013c84(2, fileBuffer, 1, DSMT_GetFileBuffer(1)) == 0)
            failed = 1;
        func_02013c84(1, DSMT_GetFileBuffer(1), -1, &data_0209caa0.file);
    } else if (command == 17) {
        DSMT_SetPendingMusic(0x37, 8);
    } else if (command == 18) {
        DSMT_SetPendingMusic(0x38, 8);
    } else if (command == 20) {
        u8 *fileBytes = (u8 *)::operator new[](0xcc);
        u32 saveSlot;
        DSMT_SetFileBuffers((FileSaveData *)fileBytes);
        for (saveSlot = 0; (s32)saveSlot < 3; saveSlot++, fileBytes += 0x44) {
            if (SaveData::ReadFileData(saveSlot, (FileSaveData *)fileBytes) == 0)
                failed = 1;
            func_02013c84(saveSlot, fileBytes, -1, &data_0209caa0.file);
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
        Sound::StopLoadedMusic_Layer1(0x3c);
    }
    DSMT_UpdateBootArgs(data_0209caa0.activeFile);
    return failed;
}

/* InitResources (key function): engines on, graph callback registered, session seeded. */
s32 dScDSMT_c::InitResources() {
    if (gSoftResetPending != 0)
        sBootArgs.status = 1;

    func_02023544();
    UnloadArchives();
    func_02017e94((int)&OVERLAY_100_ID);
    func_02017e94((int)&OVERLAY_102_ID);

    if (data_0209d524 != 0)
        func_0201a428();

    sBootArgs.messages = (void *)data_0209d6fc;
    GX_SetPower(GX_POWER_ALL);
    GX_DispOn();
    GXS_DispOn();
    Enable3dEngines();
    ::Initialise3dGraphics(0);
    gGraphCallback = &mGraphCallback;
    dScene_c::SetFaders(&fader);
    data_0208ee44 = 1;
    GX::DisableAllBanks();
    data_ov007_02103260 = -1;
    func_ov007_020b7138(&data_ov007_02103290, data_0209b340);
    func_ov007_020b7090(0, 0, 0, 0, 0);
    Sound::LoadInitialGroup(2);
    return 1;
}

/* CleanupResources: unregister callback, record exit reason, release voice group. */
s32 dScDSMT_c::CleanupResources() {
    gGraphCallback = 0;
    dScene_c::SetAndStopColorFader();
    sBootArgs.bootFlags = func_ov007_020b6f4c();
    sBootArgs.status = 2;
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
        func_02013c84(saveSlot, data_0209b33c + saveSlot, -1, &data_0209caa0.file);
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
    func_ov007_020b7040(this);
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
        sBootArgs.soundModeItem = 1;
    } else if (GetSoundMode() == 1) {
        sBootArgs.soundModeItem = 2;
    } else if (GetSoundMode() == 2) {
        sBootArgs.soundModeItem = 0;
    }

    sBootArgs.backlightOn = data_0208ee3c;
    data_0209b34b[saveSlot] = (data_0209caa0.file.flags1 & 1) != 0;

    highestUnlockedPlusOne = 3;
    character = 2;
    do {
        if (SaveData::IsCharacterUnlocked((u32)character))
            break;
        highestUnlockedPlusOne--;
        character--;
    } while (character >= 0);
    data_0209b34e[saveSlot] = highestUnlockedPlusOne;
    for (course = 0; course < 0xf; course++) {
        sBootArgs.firstStar[saveSlot][course] = IsStarCollected(course, 0) != 0;
        sBootArgs.otherStars[saveSlot][course] =
            CountStarsCollectedInLevelToDisplay(course) - sBootArgs.firstStar[saveSlot][course];
        sBootArgs.coinRecords[saveSlot][course] = SaveData::GetCoinRecord((u32)course);
    }

    data_0209b3d8[saveSlot] = CountStarsCollectedInLevelToDisplay(0x1d);
}

/* Arm the deferred music change Behavior commits. Declines if one is pending. */
int DSMT_SetPendingMusic(int musicId, unsigned int delayFrames) {
    if (data_ov007_02103260 >= 0) {
        return 0;
    }
    data_ov007_02103260 = musicId;
    data_ov007_02104c28 = (unsigned short)delayFrames;
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


/* Behavior consumes a nonnegative pending music ID after the delay; -1 is idle.
 * Keep the configured identity while its shared declaration is reserved. */
int data_ov007_02103260 = -1;
BaseSpawnInfo<dScDSMT_c> g_profile_DSMT = {dScDSMT_c_classInit, 1, 3};

