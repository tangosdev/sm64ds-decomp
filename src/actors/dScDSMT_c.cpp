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
 * free helpers (a save-file slot lookup, installing the save-file buffer,
 * arming a deferred music change, and the per-file summary the client
 * displays), the client's command dispatcher func_ov007_020cc600, five allocator/file veneers for
 * the interface the client calls, and the factory dScDSMT_c_classInit.
 *
 * The destructor is inline in the class body and the key function is
 * InitResources, so this TU emits D1 then D0 -- the cartridge's order --
 * and both vtables. Source order is the reverse of the ROM's: mwccarm
 * emits one .text section per function in reverse source order. Do not
 * reorder.
 *
 * classInit stays a literal construction bridge: the fader member's
 * constructor is the out-of-line arm9 _ZN10dFdDummy_cC1Ev, so
 * `new dScDSMT_c` would not spell the same call.
 *
 * Leftovers:
 * - The four func_ov007_* helpers, the dispatcher and the wrappers keep
 *   address-derived names; the image carries no original symbol table for
 *   them.
 * - data_0209b340 is decl_common.h's `int[]` (word stores at 0, 1 and
 *   0x27); func_ov007_020cc168 writes it bytewise, so byte accesses take a
 *   cast view of the same array rather than a second declaration.
 * - The overlay operands are the FS_OVERLAY_ID idiom: `(int)&OVERLAY_100_ID`
 *   and `(int)&OVERLAY_102_ID` pool-load the absolute linker constants the
 *   generated arm9.lcf defines (values 0x64/0x66 -- the cartridge's pool
 *   words at 0x020cc450/0x020cc454). Plain literals are immediate-encodable
 *   and would compile to `mov`, not the cartridge's loads; decl_common.h's
 *   `overlay_N` names are dsd bookkeeping rows the lcf never defines, so
 *   they do not link.
 */

#include "dScDSMT_c.h"
#include "types.h"
#include "Sound.h"
#include "SaveData.h"
#include "Heap.h"
#include "decl_common.h"

/* One save file, the 0x44-byte unit SaveData::ReadFileData and
 * SaveData::EraseSaveFile move; include/SaveData.h only forward-declares
 * it. Layout as the save block's callers spell it. */
struct FileSaveData {
    u32 magic8000;
    u32 flags1;
    u32 flags2;
    u32 minigameRabbits;
    u32 cannonUnlocked;
    u8  stars[30];
    u8  coinRecords[15];
    u8  currentCharacter;
    u8  controllerMode;
    u8  unk43;
};

extern "C" {
extern FileSaveData *data_0209b33c;
extern u8 data_0209b34b[];
extern u8 data_0209b34e[];
extern u8 data_0209b3d8[];
extern u8 data_0208ee3c[];
extern int data_0209caa0[];
extern u8 data_0209cae4[];
extern u8 data_0209f1e0;
extern void *data_0209d4a8;
extern int data_0208ee44;
extern void *data_020a0ea0;
extern int data_0208e4b8[];
extern int data_0208ee14[];
extern int data_ov007_021032e8[];
extern int data_ov007_021032b0[];
/* Linker-defined overlay IDs: the address is the overlay number. */
extern int OVERLAY_100_ID;
extern int OVERLAY_102_ID;

FileSaveData *func_ov007_020cc0cc(int idx);
void func_ov007_020cc0e4(FileSaveData *files);
int func_ov007_020cc118(int a, unsigned int b);
void func_ov007_020cc168(u32 idx);
int func_ov007_020b7090(u16 a0, u16 a1, u16 a2, u16 a3, int arg4);
void func_ov007_020b6eb4(void *callback);
void func_0203cbc0(void *buffer);
void Enable3dEngines();
int GetSoundMode(void);
int IsStarCollected(int course, int star);
unsigned short DecIfAbove0_Short(unsigned short *p);
void StartMinigameMenu(u8 returnToRecRoom);
void SetSoundMode(int mode);
void TurnBacklightOn(void);
void TurnBacklightOff(void);
void *_ZN7fBase_cC2Ev(void *p);
struct dFdDummy_c *_ZN10dFdDummy_cC1Ev(struct dFdDummy_c *p);
}

/* The two GX entry points this scene uses have no header of their own.
 * decl_common.h carries them under their mangled spellings; the namespace
 * form below emits the same two symbols. */
namespace GX {
void DispOn();
void DisableAllBanks();
}

/* -------------------------------------------------------------------------- */
/* 0x020ccad0 -- the factory. The literal construction bridge: allocates 0x64,
 * runs the fBase_c base constructor, walks the vtable chain fBase_c -> dScene_c
 * -> dScDSMT_c (the store is spelled with the data_ov007 aliases of the two
 * vtables this TU emits -- the same aliasing dScMB_c uses: ov007's _ZTV rows
 * sit at the vtable address point, and a TU-local _ZTV store spelled by name
 * trips objisolate's defined-then-externalised survey), ORs the two spawn-flag
 * bits at +0x13, gives the graphCallback_c sub-object at +0x50 its
 * base-then-derived vptr pair, and finishes the dFdDummy_c member at +0x54 via
 * the arm9 constructor _ZN10dFdDummy_cC1Ev. */
/* -------------------------------------------------------------------------- */
// @symbol dScDSMT_c_classInit
extern "C" int *dScDSMT_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(0x64);
    if (p) {
        unsigned char *f;
        _ZN7fBase_cC2Ev(p);
        p[0] = (int)data_0208e4b8;
        p[0] = (int)_ZTV8dScene_c;
        f = (unsigned char *)((char *)p + 0x13);
        *f |= 1;
        *f |= 4;
        p[0] = (int)data_ov007_021032e8;
        p[0x50 / 4] = (int)data_0208ee14;
        p[0x50 / 4] = (int)data_ov007_021032b0;
        _ZN10dFdDummy_cC1Ev((struct dFdDummy_c *)((char *)p + 0x54));
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* 0x020ccab4 / 0x020cca98 / 0x020cca80 -- heap thunks for the interface the
 * client calls: forward onto the game heap at data_020a0ea0. All three are
 * `bx ip` sibcalls under -interworking. */
/* -------------------------------------------------------------------------- */
// @symbol func_ov007_020ccab4
extern "C" void *func_ov007_020ccab4(u32 a) {
    return ((Heap *)data_020a0ea0)->Allocate(a);
}

// @symbol func_ov007_020cca98
extern "C" void func_ov007_020cca98(void *a) {
    ((Heap *)data_020a0ea0)->_Deallocate(a);
}

// @symbol func_ov007_020cca80
extern "C" void func_ov007_020cca80(void) {
    (void)((Heap *)data_020a0ea0)->MaxAllocationUnitSize();
}

/* -------------------------------------------------------------------------- */
/* 0x020cca74 / 0x020cca68 -- file-op veneers, three-word `ldr ip,[pc] / bx ip`
 * thunks. #pragma long_calls makes mwccarm emit the pooled absolute tail call
 * the ROM uses to reach the targets. */
/* -------------------------------------------------------------------------- */
#pragma long_calls on
extern "C" int LoadFile(int);
// @symbol func_ov007_020cca74
extern "C" int func_ov007_020cca74(int a) {
    return LoadFile(a);
}
// @symbol func_ov007_020cca68
extern "C" void func_ov007_020cca68(void *p) {
    Deallocate(p);
}
#pragma long_calls off

/* -------------------------------------------------------------------------- */
/* 0x020cc600 -- the client's command dispatcher. Answers one command code:
 * sound mode and backlight requests, per-file erase and copy (3-file buffer
 * at data_0209b33c, records read through func_ov007_020cc0cc), deferred music
 * changes through func_ov007_020cc118, a fresh read of all three save files,
 * minigame-record load/store, then refreshes the per-file summary for the
 * current file before returning a status byte. */
/* -------------------------------------------------------------------------- */
// @symbol func_ov007_020cc600
extern "C" u8 func_ov007_020cc600(s32 arg)
{
    u8 r = 0;

    if (arg == 1) {
        SetSoundMode(2);
    } else if (arg == 2) {
        SetSoundMode(0);
    } else if (arg == 3) {
        SetSoundMode(1);
    } else if (arg == 4) {
        TurnBacklightOn();
    } else if (arg == 5) {
        TurnBacklightOff();
    } else if (arg == 8) {
        SaveData::EraseSaveFile(0, (char *)func_ov007_020cc0cc(0));
        func_02013c84(0, func_ov007_020cc0cc(0), -1, data_0209caa0);
    } else if (arg == 9) {
        SaveData::EraseSaveFile(1, (char *)func_ov007_020cc0cc(1));
        func_02013c84(1, func_ov007_020cc0cc(1), -1, data_0209caa0);
    } else if (arg == 10) {
        SaveData::EraseSaveFile(2, (char *)func_ov007_020cc0cc(2));
        func_02013c84(2, func_ov007_020cc0cc(2), -1, data_0209caa0);
    } else if (arg == 11) {
        FileSaveData *p = func_ov007_020cc0cc(0);
        if (func_02013c84(0, p, 1, func_ov007_020cc0cc(1)) == 0)
            r = 1;
        func_02013c84(1, func_ov007_020cc0cc(1), -1, data_0209caa0);
    } else if (arg == 12) {
        FileSaveData *p = func_ov007_020cc0cc(0);
        if (func_02013c84(0, p, 2, func_ov007_020cc0cc(2)) == 0)
            r = 1;
        func_02013c84(2, func_ov007_020cc0cc(2), -1, data_0209caa0);
    } else if (arg == 13) {
        FileSaveData *p = func_ov007_020cc0cc(1);
        if (func_02013c84(1, p, 0, func_ov007_020cc0cc(0)) == 0)
            r = 1;
        func_02013c84(0, func_ov007_020cc0cc(0), -1, data_0209caa0);
    } else if (arg == 14) {
        FileSaveData *p = func_ov007_020cc0cc(1);
        if (func_02013c84(1, p, 2, func_ov007_020cc0cc(2)) == 0)
            r = 1;
        func_02013c84(2, func_ov007_020cc0cc(2), -1, data_0209caa0);
    } else if (arg == 15) {
        FileSaveData *p = func_ov007_020cc0cc(2);
        if (func_02013c84(2, p, 0, func_ov007_020cc0cc(0)) == 0)
            r = 1;
        func_02013c84(0, func_ov007_020cc0cc(0), -1, data_0209caa0);
    } else if (arg == 16) {
        FileSaveData *p = func_ov007_020cc0cc(2);
        if (func_02013c84(2, p, 1, func_ov007_020cc0cc(1)) == 0)
            r = 1;
        func_02013c84(1, func_ov007_020cc0cc(1), -1, data_0209caa0);
    } else if (arg == 17) {
        func_ov007_020cc118(0x37, 8);
    } else if (arg == 18) {
        func_ov007_020cc118(0x38, 8);
    } else if (arg == 20) {
        u8 *sb = (u8 *)_ZN6Memory13operator_new2Ej(0xcc);
        u32 i;
        func_ov007_020cc0e4((FileSaveData *)sb);
        for (i = 0; (s32)i < 3; i++, sb += 0x44) {
            if (SaveData::ReadFileData(i, (FileSaveData *)sb) == 0)
                r = 1;
            func_02013c84(i, sb, -1, data_0209caa0);
            func_ov007_020cc168(i);
        }
    } else if (arg == 21) {
        if (SaveData::ReadMinigameData((MinigameSaveData *)data_0209cae4) == 0)
            r = 1;
    } else if (arg == 22) {
        s32 i;
        for (i = 0; i < 3; i++) {
            FileSaveData *e = func_ov007_020cc0cc(i);
            if ((e->flags1 & 1) == 0)
                SaveData::EraseSaveFile(i, (char *)e);
        }
    } else if (arg == 23) {
        SaveData::SaveMinigames((MinigameSaveData *)data_0209cae4);
    } else if (arg == 19) {
        Sound::StopLoadedMusic_Layer1(0x3c);
    }
    func_ov007_020cc168(((u8 *)data_0209caa0)[0x328]);
    return r;
}

/* -------------------------------------------------------------------------- */
/* 0x020cc4c0 -- vtable slot 0, the key function.
 *
 * Brings the scene up: clears the archives and the two minigame-menu overlays
 * a previous scene may have left loaded, turns the 3D engine and both display
 * engines on, registers the graphCallback_c sub-object with the graph
 * (data_0209d4a8), hands dScene_c this object's own `fader`, seeds the
 * download-play session out of data_0209b340 and asks for sound group 2. */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9dScDSMT_c13InitResourcesEv
s32 dScDSMT_c::InitResources()
{
    if (data_0209f1e0 != 0)
        data_0209b340[1] = 1;

    func_02023544();
    UnloadArchives();
    func_02017e94((int)&OVERLAY_100_ID);
    func_02017e94((int)&OVERLAY_102_ID);

    if (data_0209d524 != 0)
        func_0201a428();

    data_0209b340[0x27] = data_0209d6fc;
    *(volatile u16 *)0x4000304 =
        (*(volatile u16 *)0x4000304 & 0xfffffdf1) | 0x20e;
    GX::DispOn();
    *(volatile u32 *)0x4001000 |= 0x10000;
    Enable3dEngines();
    ::Initialise3dGraphics(0);
    data_0209d4a8 = &unk_050;
    dScene_c::SetFaders(&fader);
    data_0208ee44 = 1;
    GX::DisableAllBanks();
    data_ov007_02103260 = -1;
    func_ov007_020b7138(&data_ov007_02103290, data_0209b340);
    func_ov007_020b7090(0, 0, 0, 0, 0);
    Sound::LoadInitialGroup(2);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* 0x020cc45c -- vtable slot 3. Unregisters the graph callback, hands the
 * colour fader back, records the exit reason in data_0209b340, then releases
 * the download-play voice group and the save-slot buffer. */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9dScDSMT_c16CleanupResourcesEv
s32 dScDSMT_c::CleanupResources()
{
    data_0209d4a8 = 0;
    dScene_c::SetAndStopColorFader();
    data_0209b340[0] = func_ov007_020b6f4c();
    data_0209b340[1] = 2;
    Sound::UnsetPlayerVoiceGroup();
    func_0203cbc0(data_0209b33c);
    data_0209b33c = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* 0x020cc2cc -- vtable slot 6. Pumps the download-play state machine and
 * dispatches on its return code: 3..5 load one of three saved files, 6 pulls
 * in the minigame overlays and fades out, 7 opens the minigame menu, 2 fades
 * back. Ends by handing its own `fader` member to dScene_c. */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9dScDSMT_c8BehaviorEv
s32 dScDSMT_c::Behavior()
{
    int result;

    if (data_ov007_02103260 >= 0 && DecIfAbove0_Short(&data_ov007_02104c28) == 0) {
        Sound::LoadAndSetMusic_Layer1(data_ov007_02103260);
        data_ov007_02103260 = -1;
    }

    func_0203da9c();
    {
        u16 *p = func_0203dabc();
        int arg4;
        func_0203da9c();
        arg4 = func_0203dae4();
        result = func_ov007_020b7090(p[0], p[1], p[2], p[3], arg4);
    }

    if ((unsigned int)(result - 3) <= 2) {
        int idx = 0;
        if (result == 4) {
            idx = 1;
        } else if (result == 5) {
            idx = 2;
        }
        func_02013c84(idx, data_0209b33c + idx, -1, data_0209caa0);
        StartFile(1, 0);
    } else if (result == 6) {
        if (data_0209d524 == 0) {
            Heap *h1 = (Heap *)func_0201a458();
            Heap *h2 = h1->SetDefault();
            LoadArchive(1);
            LoadTextNarcs();
            h1->Rescue();
            h2->SetDefault();
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

/* -------------------------------------------------------------------------- */
/* 0x020cc2b0 -- vtable slot 9. Forwards `this` to the overlay's own
 * download-play draw routine and reports success unconditionally. */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9dScDSMT_c6RenderEv
s32 dScDSMT_c::Render()
{
    func_ov007_020b7040(this);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* 0x020cc2ac -- vtable slot 12. Nothing to unwind; the slot exists only so
 * dScene_c's teardown has something to call. */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9dScDSMT_c16OnPendingDestroyEv
void dScDSMT_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* 0x020cc168 -- fills in guest slot `idx` of the download-play record table:
 * sound mode, the frame-rate byte, the preference bit out of data_0209caa0,
 * how many of the three extra characters are unlocked, and then per course
 * whether its first star is taken, how many others are, and the coin record.
 * Writes BYTES into data_0209b340, which the class members read as words. */
/* -------------------------------------------------------------------------- */
// @symbol func_ov007_020cc168
extern "C" void func_ov007_020cc168(u32 idx)
{
    int chr;
    int i;
    int j;
    u8 *rec;

    if (GetSoundMode() == 0) {
        ((u8 *)data_0209b340)[8] = 1;
    } else if (GetSoundMode() == 1) {
        ((u8 *)data_0209b340)[8] = 2;
    } else if (GetSoundMode() == 2) {
        ((u8 *)data_0209b340)[8] = 0;
    }

    ((u8 *)data_0209b340)[9] = data_0208ee3c[0];
    data_0209b34b[idx] = (data_0209caa0[1] & 1) ? 1 : 0;

    chr = 3;
    i = 2;
    do {
        if (SaveData::IsCharacterUnlocked((u32)i)) break;
        chr--;
        i--;
    } while (i >= 0);
    data_0209b34e[idx] = chr;

    rec = &((u8 *)data_0209b340)[idx * 0xf];
    for (j = 0; j < 0xf; j++) {
        rec[0x6b] = IsStarCollected(j, 0) ? 1 : 0;
        rec[0x11] = CountStarsCollectedInLevelToDisplay(j) - rec[0x6b];
        rec[0x3e] = SaveData::GetCoinRecord((u32)j);
        rec++;
    }

    data_0209b3d8[idx] = CountStarsCollectedInLevelToDisplay(0x1d);
}

/* -------------------------------------------------------------------------- */
/* 0x020cc118 -- arms the deferred music change Behavior later commits:
 * remembers track `a` and a countdown `b`, and stops the current layer-1 music
 * over the same number of frames. Declines if one is already pending. */
/* -------------------------------------------------------------------------- */
// @symbol func_ov007_020cc118
extern "C" int func_ov007_020cc118(int a, unsigned int b)
{
    if (data_ov007_02103260 >= 0) {
        return 0;
    }
    data_ov007_02103260 = a;
    data_ov007_02104c28 = (unsigned short)b;
    Sound::StopLoadedMusic_Layer1(b);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* 0x020cc110 -- dScDSMT_c::graphCallback_c slot 0. Declines it: the ROM body
 * is `mov r0,#0; bx lr`, against dGraph_c::callback_c's default of 1. */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9dScDSMT_c15graphCallback_c14GraphCallback0Ev
int dScDSMT_c::graphCallback_c::GraphCallback0()
{
    return 0;
}

/* -------------------------------------------------------------------------- */
/* 0x020cc0f4 -- dScDSMT_c::graphCallback_c slot 2. Hands the callback object
 * itself to func_ov007_020b6eb4 and declines the slot. The callee is still an
 * unnamed ov007 free function; it takes this object, not the owning scene. */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9dScDSMT_c15graphCallback_c14GraphCallback2Ev
int dScDSMT_c::graphCallback_c::GraphCallback2()
{
    func_ov007_020b6eb4(this);
    return 0;
}

/* -------------------------------------------------------------------------- */
/* 0x020cc0e4 -- installs the save-slot buffer pointer. Writes the pointer
 * word itself, not through it. */
/* -------------------------------------------------------------------------- */
// @symbol func_ov007_020cc0e4
extern "C" void func_ov007_020cc0e4(FileSaveData *files)
{
    data_0209b33c = files;
}

/* -------------------------------------------------------------------------- */
/* 0x020cc0cc -- address of save-slot record `idx`. The `mov r1,#0x44;
 * mla r0,r1,r0,r2` is where the 0x44 record stride is proven. */
/* -------------------------------------------------------------------------- */
// @symbol func_ov007_020cc0cc
extern "C" FileSaveData *func_ov007_020cc0cc(int idx)
{
    return &data_0209b33c[idx];
}
