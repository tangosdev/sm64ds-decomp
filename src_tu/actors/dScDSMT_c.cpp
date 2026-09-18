//cpp
/* dScDSMT_c -- the DS Download Play ("DS MulTi") scene, ov007.
 *
 * The scene the cartridge runs while it is serving single-card multiplayer:
 * it brings the engines up, hands the graph a callback object, pumps the
 * download-play state machine every frame and dispatches on its verdict --
 * load one of the three save files, pull in the minigame overlays and fade
 * out, open the minigame menu, or fade back.
 *
 * 13 functions, .text 0x020cc028..0x020cc600: the seven dScene_c slots every
 * direct child overrides (0, 3, 6, 9, 12, 16, 17 -- the class adds no new
 * virtual), two slots of the nested dScDSMT_c::graphCallback_c, and four free
 * helpers that share the TU. D1/D0 are not written here: the header's inline
 * destructor plus the key function (InitResources) emits them, D1 first, the
 * ROM's order.
 *
 * Source order is the reverse of the ROM's -- mwccarm emits one .text section
 * per function, in reverse source order. Do not reorder.
 *
 * The factory dScDSMT_c_classInit (0x020ccad0) is NOT in this TU. It does not
 * abut the run: six unattributed functions sit between InitResources' end
 * (0x020cc600) and it, so tools/tu_map.py's unit stops at 0x020cc600 and the
 * shard-count extension that absorbs a zero-gap <Class>_classInit does not
 * reach. This TU therefore has no factory and no `new`, and declares no
 * `_ZTV9dScDSMT_c` -- the vtable is still emitted, by the key function, and
 * banked as compiler-only output.
 *
 * Reconciliation leftovers, all four of them declaration merges the legacy
 * one-function files never had to make:
 *
 * - data_0209b340 arrived as `int[]` (include/decl_common.h, used by
 *   InitResources and CleanupResources for word stores at indices 0, 1 and
 *   0x27) and as `u8[]` (the legacy src/func_ov007_020cc168.c, which stores
 *   BYTES at +8/+9 and strides the record table one byte at a time). Both
 *   readings are right about their own accesses and they cannot share one
 *   declaration. decl_common.h's `int[]` wins, because the tree already
 *   carries it and because the word stores are what the cartridge does --
 *   `strne r1,[r0,#4]` for index 1. DSMT_B casts that one array to its byte
 *   view for the one member that needs it, rather than adding a second
 *   declaration of the symbol.
 * - data_0209caa0 arrived as `int[]` (func_ov007_020cc168) and as a bare
 *   `char` whose address Behavior takes. `int[]` wins: it is the tree's
 *   plurality (twelve files) and it types 020cc168's `& 1` test of word 1.
 *   Behavior passes the array, which decays to the same address.
 * - data_0209b33c arrived four ways across the four legacy files --
 *   `struct E *`, `int[]`, `char *`, `void *`. It is one pointer variable
 *   holding one buffer of 0x44-byte save-file records: func_ov007_020cc0cc
 *   indexes it with an `mla` by 0x44, Behavior reaches record `idx` the same
 *   way, func_ov007_020cc0e4 overwrites the pointer word itself, and
 *   CleanupResources frees it and nulls it. The `struct E *` spelling is the
 *   one that states the stride, so it is the one kept; the other three
 *   members are rewritten onto it and each still byte-matches.
 * - func_ov007_020b7090 arrived as `(u16,u16,u16,u16,int)` (Behavior, which
 *   passes four 16-bit words out of func_0203dabc's buffer) and as
 *   `(int,int,int,int,int)` (InitResources, which passes five zeroes and so
 *   cannot distinguish them). The u16 form wins because it is the one the
 *   bytes constrain.
 * - overlay_100 / overlay_102. Behavior arrived spelling the two overlay-id
 *   operands `overlay_64` / `overlay_66`, names include/decl_common.h
 *   declares but no symbols.txt under config/ defines; InitResources arrived
 *   spelling the same two addresses `overlay_100` / `overlay_102`, which
 *   config/arm9/symbols.txt does define. The literal pool at 0x020cc450 and
 *   0x020cc454 holds 0x64 and 0x66 -- one pair of addresses, two names. The
 *   defined spelling is used throughout. The `&` is load-bearing: the plain
 *   constants compile to `mov` and the function comes out eight bytes short.
 *   Neither name links, though -- 0x64 and 0x66 lie outside every module's
 *   sections, so no delinked object defines them and mwldarm refuses the link
 *   the moment either function is enrolled `complete`. That, and not this
 *   file, is why the range is still served from ROM bytes; the manifest entry
 *   records the measurement.
 *
 * `struct E` is a file-local shadow type, not a recovered one: only its size
 * is proven, by the 0x44 stride. The same spelling is in
 * src/func_ov007_020cc600.c, which calls func_ov007_020cc0cc.
 *
 * The four func_ov007_* helpers are written free here, and that is a
 * reconstruction choice, not a deduction. The image preserves no original
 * linker symbol table, so `func_ov007_*` are address-derived analysis labels
 * and RTTI supplies class identities, not function spellings (see
 * notes/tu-promotion-conventions.md section 1 and
 * notes/symbol-name-provenance.md). None of the four takes the scene object,
 * and 020cc0cc/020cc168 are called from outside this run
 * (src/func_ov007_020cc600.c), which is consistent with external linkage but
 * does not prove the original translation-unit cut. Original ownership and
 * form stay uncertain; the free form is only what this TU reproduces.
 */

#include "dScDSMT_c.h"
#include "types.h"
#include "Sound.h"
#include "decl_Heap.h"
#include "decl_Scene.h"
#include "decl_common.h"

/* The download-play save-slot buffer's record type. Only the 0x44 stride is
 * proven -- by func_ov007_020cc0cc's `mov r1,#0x44; mla r0,r1,r0,r2` -- so the
 * body stays opaque. Same file-local spelling as src/func_ov007_020cc600.c,
 * the one caller outside this TU. */
struct E { unsigned char d[0x44]; };

/* The byte view of decl_common.h's `int data_0209b340[]`, for the one member
 * that stores bytes into it. A cast of a fixed array address, not a second
 * declaration of the symbol -- see the note above. */
#define DSMT_B ((u8 *)data_0209b340)

/* ROM symbols this TU references that no header in the tree declares yet,
 * spelt by their exact final names under C linkage, the way decl_common.h
 * spells its own. Everything else these functions call comes from Sound.h,
 * decl_Heap.h, decl_Scene.h or decl_common.h and is deliberately NOT repeated
 * here. */
extern "C" {
extern struct E *data_0209b33c;
extern int data_0209caa0[];
extern u8 data_0209b34b[];
extern u8 data_0209b34e[];
extern u8 data_0209b3d8[];
extern u8 data_0208ee3c[];
extern u8 data_0209f1e0;
extern int data_0209d4a8;
extern int data_0208ee44;

int func_ov007_020b7090(u16 a0, u16 a1, u16 a2, u16 a3, int arg4);
void func_ov007_020b6eb4(void *self);
void func_0203cbc0(void *buffer);
void Enable3dEngines();
int GetSoundMode(void);
int IsStarCollected(int course, int star);
unsigned short DecIfAbove0_Short(unsigned short *p);
void StartMinigameMenu(u8 returnToRecRoom);
int _ZN4Heap10SetDefaultEv(int heap);
int _ZN8SaveData19IsCharacterUnlockedEj(u32 character);
int _ZN8SaveData13GetCoinRecordEj(u32 course);
}

/* The two GX entry points this scene uses have no header of their own.
 * decl_common.h carries them under their mangled spellings; the namespace form
 * below emits the same two symbols and is what the recovered source read
 * like. */
namespace GX {
void DispOn();
void DisableAllBanks();
}

/* -------------------------------------------------------------------------- */
/* [12] 0x020cc4c0 -- vtable slot 0, KEY FUNCTION.
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
    func_02017e94((int)&overlay_100);
    func_02017e94((int)&overlay_102);

    if (data_0209d524 != 0)
        func_0201a428();

    data_0209b340[0x27] = data_0209d6fc;
    *(volatile u16 *)0x4000304 =
        (*(volatile u16 *)0x4000304 & 0xfffffdf1) | 0x20e;
    GX::DispOn();
    *(volatile u32 *)0x4001000 |= 0x10000;
    Enable3dEngines();
    ::Initialise3dGraphics(0);
    data_0209d4a8 = (int)&unk_050;
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
/* [11] 0x020cc45c -- vtable slot 3. Unregisters the graph callback, hands the
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
/* [10] 0x020cc2cc -- vtable slot 6. Pumps the download-play state machine and
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
            int h1 = func_0201a458();
            int h2 = _ZN4Heap10SetDefaultEv(h1);
            LoadArchive(1);
            LoadTextNarcs();
            _ZN4Heap6RescueEv(h1);
            _ZN4Heap10SetDefaultEv(h2);
            LoadOverlay((int)&overlay_100);
            LoadOverlay((int)&overlay_102);
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
/* [9] 0x020cc2b0 -- vtable slot 9. Forwards `this` to the overlay's own
 * download-play draw routine and reports success unconditionally. */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9dScDSMT_c6RenderEv
s32 dScDSMT_c::Render()
{
    func_ov007_020b7040(this);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* [8] 0x020cc2ac -- vtable slot 12. Nothing to unwind; the slot exists only so
 * dScene_c's teardown has something to call. */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9dScDSMT_c16OnPendingDestroyEv
void dScDSMT_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* [7] 0x020cc168 -- fills in guest slot `idx` of the download-play record
 * table: sound mode, the frame-rate byte, the preference bit out of
 * data_0209caa0, how many of the three extra characters are unlocked, and then
 * per course whether its first star is taken, how many others are, and the
 * coin record. Writes BYTES into data_0209b340, which the class members read
 * as words -- see the note at the top of this file. */
/* -------------------------------------------------------------------------- */
// @symbol func_ov007_020cc168
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov007_020cc168(u32 idx) {
    int chr;
    int i;
    int j;
    u8 *rec;

    if (GetSoundMode() == 0) {
        DSMT_B[8] = 1;
    } else if (GetSoundMode() == 1) {
        DSMT_B[8] = 2;
    } else if (GetSoundMode() == 2) {
        DSMT_B[8] = 0;
    }

    DSMT_B[9] = data_0208ee3c[0];
    data_0209b34b[idx] = (data_0209caa0[1] & 1) ? 1 : 0;

    chr = 3;
    i = 2;
    do {
        if (_ZN8SaveData19IsCharacterUnlockedEj((u32)i)) break;
        chr--;
        i--;
    } while (i >= 0);
    data_0209b34e[idx] = chr;

    rec = &DSMT_B[idx * 0xf];
    for (j = 0; j < 0xf; j++) {
        rec[0x6b] = IsStarCollected(j, 0) ? 1 : 0;
        rec[0x11] = CountStarsCollectedInLevelToDisplay(j) - rec[0x6b];
        rec[0x3e] = _ZN8SaveData13GetCoinRecordEj((u32)j);
        rec++;
    }

    data_0209b3d8[idx] = CountStarsCollectedInLevelToDisplay(0x1d);
}
}

/* -------------------------------------------------------------------------- */
/* [6] 0x020cc118 -- arms the deferred music change Behavior later commits:
 * remembers track `a` and a countdown `b`, and stops the current layer-1 music
 * over the same number of frames. Declines if one is already pending. */
/* -------------------------------------------------------------------------- */
// @symbol func_ov007_020cc118
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov007_020cc118(int a, unsigned int b) {
    if (data_ov007_02103260 >= 0) {
        return 0;
    }
    data_ov007_02103260 = a;
    data_ov007_02104c28 = (short)b;
    Sound::StopLoadedMusic_Layer1(b);
    return 1;
}
}

/* -------------------------------------------------------------------------- */
/* [5] 0x020cc110 -- dScDSMT_c::graphCallback_c slot 0. Declines it: the ROM
 * body is `mov r0,#0; bx lr`, against dGraph_c::callback_c's default of 1. */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9dScDSMT_c15graphCallback_c14GraphCallback0Ev
int dScDSMT_c::graphCallback_c::GraphCallback0()
{
    return 0;
}

/* -------------------------------------------------------------------------- */
/* [4] 0x020cc0f4 -- dScDSMT_c::graphCallback_c slot 2. Hands the callback
 * object itself to func_ov007_020b6eb4 and declines the slot. The callee is
 * still an unnamed ov007 free function; it takes this object, not the owning
 * scene. */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9dScDSMT_c15graphCallback_c14GraphCallback2Ev
int dScDSMT_c::graphCallback_c::GraphCallback2()
{
    func_ov007_020b6eb4(this);
    return 0;
}

/* -------------------------------------------------------------------------- */
/* [3] 0x020cc0e4 -- installs the save-slot buffer pointer. Writes the pointer
 * word itself, not through it. */
/* -------------------------------------------------------------------------- */
// @symbol func_ov007_020cc0e4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov007_020cc0e4(int v) { data_0209b33c = (struct E *)v; }
}

/* -------------------------------------------------------------------------- */
/* [2] 0x020cc0cc -- address of save-slot record `idx`. The `mov r1,#0x44;
 * mla r0,r1,r0,r2` is where the 0x44 record stride is proven. */
/* -------------------------------------------------------------------------- */
// @symbol func_ov007_020cc0cc
extern "C" {  /* .c-derived member: C linkage for the whole block */
struct E *func_ov007_020cc0cc(int idx){
  return &data_0209b33c[idx];
}
}

/* [1] [0] 0x020cc070 (D0) and 0x020cc028 (D1) -- neither is written here.
 * The header's inline destructor is what emits the pair, D1 first, the ROM's
 * order; out of line mwccarm emits D0 ahead of the written D1 plus a homeless
 * D2 the ROM never carried. The key function is InitResources -- the first
 * DECLARED non-inline virtual -- and defining it above emits the vtable whose
 * slots 16/17 odr-use both halves. The empty bodies still reproduce all 0xa4
 * bytes: the inlined vptr stores, ~dFdDummy_c on `fader` at +0x54, fBase_c's
 * own teardown, and D0's inherited operator delete. See
 * include/dScDSMT_c.h. */
