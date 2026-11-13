//cpp
/* leftovers:
 * - defer_codegen off stays, and the file stays in ROM order, the
 *   same form as d_s_entry.cpp directly below. Reordering moves the
 *   run.
 * - This is the upper part of dScEntry_c's translation unit. The
 *   lower part is d_s_entry.cpp (0x02115ab8..0x021165b0), an
 *   intact object that owns the vtable, the typeinfo and the entry_*
 *   .data claims. The two now abut. Joining them makes one intact
 *   object over 0x02115ab8..0x0211a854 that must also own the
 *   unit's .bss step sets and its initializer __sinit_ov075_0211bb00,
 *   which is still a shard. That join is its own promotion.
 * - The host steps at 0x02119640..0x02119c38 are members with coined
 *   names (recruit, solo confirm, solo start, session start).
 * - The transition helpers use native Step member-pointer copies.
 * - The destructor, the key function, is in d_s_entry.cpp. This file
 *   defines no virtual the class declares first, so it emits no
 *   dScEntry_c vtable or typeinfo.
 * - OamAnimation() and icon_c() stay out of line, closing the run
 *   after the RESULT factory. Both profile factories are
 *   `return new dScEntry_c`.
 * - The header names the entry object and shared step workspace. The
 *   transition helpers use the typed pending StepSet pointer at 0x6c.
 * - InitResources keeps its `int isResult = (actorID == 7)' flag locals.
 *   Folding the first one into its condition rewrites the function
 *   (999 words, measured).
 * - The arm9 globals and the dEntObj_c helpers it calls keep their
 *   linker names. Naming belongs with recovered members.
 *
 * dScEntry_c, the course-entry / results scene. ov075 .text
 * 0x021165b0..0x0211a854: the round tally, the two InitResources
 * halves, the settings, quit, rematch and results steps, the
 * course-select and match-loading steps, the player-select and sync
 * steps, the
 * waiting-text render, the helpers through SetSteps, the
 * six scene methods, the RESULT factory, then the OamAnimation and
 * icon_c constructors. The ENTRY factory at 0x0211a854 is not in
 * this file. It sits past the constructor tail that closes this
 * run. The code after it is reached from dEntObj_c (d_ent_obj.cpp)
 * and through tables __sinit_ov075_0211c51c reads, not from this
 * class.
 */
#pragma defer_codegen off

#include "types.h"
#include "nitro/hw/registers.h"
#include "PlayerInput.h"
#include "private/controller_input.h"
#include "Sound.h"
#include "decl_common.h"
#include "dScEntry_c.h"
#include "dFdBrightness_c.h"
#include "AsyncJob.h"
#include "Heap.h"

/* The Nitro SDK's sub-engine palette loader. No header in the tree
 * declares the namespace yet; d_s_star_sel_methods.cpp spells it the
 * same way. */

extern "C" void func_020308b4(void);
extern int data_0209fc68;

extern "C" void Enable3dEngines(void);
void func_0200f2cc(void);
int func_0203d9b4(void);
extern "C" void LoadTextNarcs(void);
extern "C" int LoadArchive(int idx);
void func_0200f220(void);
extern "C" void *LoadFile(int handle);
extern "C" void SetBg0Offset(int a, int b);
extern "C" void SetBg2Offset(int a, int b);
extern "C" void SetBg3Offset(int a, int b);
extern "C" void SetSubBg0Offset(int a, int b);
extern "C" void SetSubBg2Offset(int a, int b);
extern "C" void SetSubBg3Offset(int a, int b);
void LoadFont3D(void);
int func_0203da3c(void);
extern "C" void UnloadArchive(int i);
extern "C" void func_02030aa4(int arg);
int func_0203da9c(void);
extern "C" void func_02020028(int player, unsigned char mode);

/* The scene's graph callback: the scene registers its own object here and
   clears it on the way out. The frame calls its four slots: GraphCallback0 at
   the frame start (func_02019390), GraphCallback1 before the frame wait
   (func_02019404), GraphCallback2 and GraphCallback3 in the VBlank IRQ
   (func_02019144, func_02019100). dGraph_c::callback_c is the ROM-proven base
   of every scene's nested graphCallback_c (include/dGraph_c.h). */
extern dGraph_c::callback_c *gGraphCallback;
extern u8 gMainScreenLayers;
extern u8 gSubScreenLayers;
extern s32 data_0208ee44;

/* The scene manager Behavior polls through its slot 5. */
extern dFdBrightness_c *gSceneFader;

#include "dEntObj_c.h"
#include "OAM.h"
#include "OamAttr.h"
#include "nitro/gx/g2.h"
#include "dBase_c.h"
#include "nitro/gx/gx_bgcnt.h"
#include "Message.h"
#include "nitro/gx/gx_load.h"
#include "nitro/mi/memory.h"
#include "nitro/fx/fx_trig.h"

int func_0203d974(void);
extern "C" u8 func_02020168(void);
extern "C" void func_02020304(void);
extern "C" void func_020200e0(void);
extern "C" void func_02020334(void);
extern "C" void func_02020124(void);
int func_0203d890(void);
extern "C" u32 LoadCompressedFileAt(int, void *);
int Sound_PlayIfNotActive(int, int, int, short);

/* The entry scene's one global graph callback, ov075 .bss 0x0211d71c. The
   generated decl_common.h catch-all declares that storage `void *`, so its real
   type is spelled once here instead of at each of the eighteen call sites. */
#define ENTRY_GRAPH_CALLBACK ((dScEntry_c::graphCallback_c *)&sEntryGraphCallback)

void func_020338b0(int, int, short, int);
void func_02033464(int, int, int);
extern "C" void func_02030a38(void);
extern "C" void func_02030a78(void);
extern "C" void func_02030a6c(void);
void func_020337ec(void);
int TouchArea_Update(void *, int);
void func_ov001_020ab3f0(void *);

extern u8 data_0208f274;
extern OamAttr *data_0208a0f8[];
extern dScEntry_c::StepSet data_ov075_0211d918;
// Behavior slot of the player-sync step set, at data_ov075_0211d918 + 8.
extern dScEntry_c::Step data_ov075_0211d920;
extern dScEntry_c::StepSet data_ov075_0211d900;
extern dScEntry_c::StepSet data_ov075_0211d8b8;
extern OamAttr *data_ov075_0211c968[];
extern u8 data_0209fc58;
extern u8 data_0209fc5c[];
extern u8 data_0209fc5d[];
extern dScEntry_c::StepSet data_ov075_0211d8a0;
extern dScEntry_c::StepSet data_ov075_0211d8d0;

/* Course-select and match-loading steps, directly below the player-select
 * run in ROM order. The declarations below are the union of the six shards'
 * extern blocks, minus names this file already declares. */

int func_0203d8fc(void);
int func_0201a1bc(void);
void func_0203d930(void);
extern unsigned char EntryCourseLoadStarted;
void func_0202deac(int level);
void func_0203d918();
extern u8 data_0208a0e0;
extern "C" u8 func_020202dc(void);
extern "C" u8 func_020200b8(void);
extern "C" void func_02020078(u32 flags);
extern "C" void func_0202029c(u32 idx);
extern s8 data_ov075_0211c6ec[];
extern u8 gGameMode;
extern u8 data_0209b2e4;

extern dScEntry_c::StepSet data_ov075_0211d798;
extern dScEntry_c::StepSet data_ov075_0211d7f8;
extern struct OamAttr data_ov075_0211cba4[];

/* Result tally, the two InitResources halves and the settings, quit,
 * rematch and results steps, in ROM order ahead of the course-select
 * steps. */

extern s8 gNumStageStars[];
extern s16 gNumCoins[];
extern u8 data_0209b2f0[];
extern u8 data_0209b2ec[];
extern u8 data_0209b2ed[];
extern u8 data_0209fc50;
extern u8 data_0209fc58;
extern char data_0209ee90[];
extern u8 data_0208ee3c;
extern PadData data_020a0e58[];
extern ControllerInput data_0209f498[];
extern int data_ov075_0211c990[];
extern int data_ov075_0211c97c[];
extern int data_ov075_0211c9a4[];
extern int data_ov075_0211ca08[];
extern int data_ov075_0211c9b8[];
extern int *data_ov075_0211c9cc[];
extern OamAttr *data_ov075_0211c9e0[];
extern OamAttr *data_ov075_0211c9f4[];
extern short data_ov075_0211d72c[];
extern short data_ov075_0211d740[];
extern char data_ov075_0211c790[];
extern char data_ov075_0211c8b0[];
extern OamAttr data_ov075_0211cb84;
extern int data_ov075_0211ca64;
extern int data_ov075_0211ca6c;
extern int data_ov075_0211ca74;
extern int data_ov075_0211cc44;
extern int data_ov075_0211cc4c;
extern int data_ov075_0211cc54;
extern int data_ov075_0211cc5c;
extern void *data_0208a0e4[];
extern fBase_c *gCurrentScene;
extern int OVERLAY_100_ID;
extern int OVERLAY_102_ID;

extern "C" int GetLanguageIndex(void);
extern "C" void SetBg1Offset(int a, int b);
void func_02033390(int a);
void func_02032f9c(int a);
extern "C" int func_02030958(int val);
void func_0200f13c(void);
extern "C" void DecompressLZ16(void *src, void *dst);
extern "C" int GetSoundMode(void);
extern "C" void SetSoundMode(int mode);
extern "C" void TurnBacklightOn(void);
extern "C" void TurnBacklightOff(void);
void SetControllerMode(unsigned char mode);
void func_02033bb8(int a);
extern "C" void UnloadOverlay(int id);
extern "C" void func_02020190(void);
extern "C" void func_02030994(void);
extern "C" void func_020201c0(void);
void func_0202048c(void *dst, int a, int b);
extern "C" void func_02020768(char *thiz);
void func_02020820(OamAnimation *anim, int *src, int screen, int loops, u8 enabled, u8 affine);
void func_020331fc(int a, int b, int c);
void PrepareVsMode(void);
void LoadLevelNoReturn(int levelID, u32 entranceID, u32 mode, u32 arg3);
extern u8 data_02092778;
extern "C" u8 func_02020364(int player);
extern "C" u8 func_020201f0(int);

extern dScEntry_c::StepSet data_ov075_0211d7e0;
extern dScEntry_c::StepSet data_ov075_0211d810;
extern dScEntry_c::StepSet data_ov075_0211d828;
extern dScEntry_c::StepSet data_ov075_0211d870;
extern dScEntry_c::StepSet data_ov075_0211d888;
extern dScEntry_c::StepSet data_ov075_0211d8e8;
extern short data_ov075_0211b5d4[];

void dScEntry_c::TallyRoundWinners() {
    int max;
    u8 b;
    int t;
    u8 *pb;
    int i;
    int n;
    u8 *pc;

    i = 0;
    pc = data_0209b2f0;
    do {
        i = i + 1;
        *pc = 0;
        pc = pc + 1;
    } while (i < 4);

    if (gNumStageStars[0] != 0 || gNumStageStars[1] != 0 || gNumStageStars[2] != 0 ||
        gNumStageStars[3] != 0 || gNumCoins[0] != 0 || gNumCoins[1] != 0 || gNumCoins[2] != 0 ||
        gNumCoins[3] != 0) {
        max = 0;
        i = max;
        pc = (u8 *)gNumStageStars;
        do {
            t = (s8)*pc;
            i = i + 1;
            pc = pc + 1;
            if (max < t) {
                max = t & 0xff;
            }
        } while (i < 4);

        n = 0;
        pb = (u8 *)gNumStageStars;
        pc = data_0209b2f0;
        i = n;
        do {
            t = (s8)*pb;
            i = i + 1;
            pb = pb + 1;
            if (t >= max) {
                *pc = 1;
                n = n + 1;
            } else {
                *pc = 0;
            }
            pc = pc + 1;
        } while (i < 4);

        if (n >= 2) {
            {
                int w;
                w = 0;
                pc = data_0209b2f0;
                i = w;
                do {
                    if (*pc != 0 && w < gNumCoins[i]) {
                        w = gNumCoins[i] & 0xff;
                    }
                    i = i + 1;
                    pc = pc + 1;
                } while (i < 4);

                pc = data_0209b2f0;
                i = 0;
                do {
                    if (*pc != 0 && gNumCoins[i] < w) {
                        *pc = 0;
                    }
                    i = i + 1;
                    pc = pc + 1;
                } while (i < 4);
            }
        }

        pc = data_0209b2f0;
        pb = data_0209b2ec;
        i = 0;
        do {
            if (*pc != 0 && *pb < 99) {
                *pb = *pb + 1;
            }
            i = i + 1;
            pc = pc + 1;
            pb = pb + 1;
        } while (i < 4);
    }

    b = data_0209b2ec[0];
    pc = data_0209fc5d;
    pb = data_0209b2ed;
    i = 1;
    do {
        if (*pc != 0) {
            if (*pb > b) {
                b = *pb;
            }
        }
        i = i + 1;
        pc = pc + 1;
        pb = pb + 1;
    } while (i < 4);

    pc = data_0209fc5c;
    pb = data_0209b2ec;
    i = 0;
    do {
        if (*pc != 0 && *pb >= b && b != 0) {
            mResultLeaderFlags[i] = 1;
        } else {
            mResultLeaderFlags[i] = 0;
        }
        i = i + 1;
        pc = pc + 1;
        pb = pb + 1;
    } while (i < 4);
}

s32 dScEntry_c::InitResultResources() {
    int k;
    void *tmp;
    int id0;
    int id1;
    int i;
    int z;
    int found;
    int j;
    int acc;
    int base;
    int step;
    int n;

    G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1000,
                     GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01);
    G2_BG1Mosaic(0);

    LoadCompressedFileAt(0x8413, G2_GetBG2CharPtr());
    tmp = LoadFile(0x8414);
    GX_LoadBGPltt(tmp, 0, 0x1c0);
    Deallocate(tmp);

    n = data_0209fc50;
    if (n == 2) {
        id0 = 0x840c;
        id1 = 0x840d;
    } else if (n == 3) {
        id0 = 0x840e;
        id1 = 0x840f;
    } else {
        id0 = 0x8410;
        id1 = 0x8411;
    }
    LoadCompressedFileAt(id0, G2_GetBG1ScrPtr());
    LoadCompressedFileAt(id1, G2_GetBG2ScrPtr());

    i = 0;
    if (n > 0) {
        z = i;
        do {
            SetSlotBorderPalette(i, z);
            i = i + 1;
        } while (i < n);
    }

    LoadCompressedFileAt(data_ov075_0211c990[GetLanguageIndex()], G2_GetOBJCharPtr());
    tmp = LoadFile(0x8415);
    GX_LoadOBJPltt(tmp, 0, 0x140);
    Deallocate(tmp);

    G2_SetBG1Priority(2);
    SetBg1Offset(0, 0);
    LoadCompressedFileAt(0x8402, G2S_GetOBJCharPtr());
    LoadCompressedFileAt(data_ov075_0211c97c[GetLanguageIndex()],
                         (u8 *)G2S_GetOBJCharPtr() + 0x6000);
    tmp = LoadFile(0x8403);
    GXS_LoadOBJPltt(tmp, 0, 0x200);
    Deallocate(tmp);

    TallyRoundWinners();

    found = -1;
    if (data_0209b2f0[func_0203da9c()] != 0) {
        found = func_0203da9c();
    } else {
        j = 0;
        do {
            if (data_0209b2f0[(u8)j] != 0) {
                found = j;
                break;
            }
            j = j + 1;
        } while (j < 4);
    }
    if (found >= 0) {
        func_02032f9c((s8)(u8)found);
    } else {
        func_02033390(1);
    }

    k = 0;
    if (n > 0) {
        acc = k;
        step = (4 - n) * 0x20 + 0xa0;
        base = (4 - n) * 0x40 + 0xa5;
        do {
            unsigned char ch = func_02030958(k);
            func_02033464((unsigned short)(base + acc), (signed char)ch, 1);
            k = k + 1;
            acc = acc + step;
        } while (k < n);
    }

    SetSteps(&data_ov075_0211d7e0);
    mEntObj = (dEntObj_c *)dBase_c::Spawn(0x15a, gCurrentScene, 1, 0);
    return 1;
}

/* Runtime asset handles resolved through the overlay 0 file table. */
enum EntryAssetId { ASSET_D_2D_BATTLE_FRAMEWORK_GAMESTART_P_NSC_BIN = 0x025e };

s32 dScEntry_c::InitEntryResources() {
    void *tmp;
    int mode;

    if (param1 != 0 || func_0203d9b4() != 0) {
        func_0200f13c();
        if (func_0203d9b4() != 0) {
            LoadCompressedFileAt(ASSET_D_2D_BATTLE_FRAMEWORK_GAMESTART_P_NSC_BIN,
                                 G2S_GetBG2ScrPtr());
        }
        DecompressLZ16(data_0208a0e4[GetLanguageIndex()], G2S_GetOBJCharPtr());
    }
    LoadCompressedFileAt(data_ov075_0211c9a4[GetLanguageIndex()], G2_GetOBJCharPtr());

    tmp = LoadFile(0x8412);
    GX_LoadOBJPltt(tmp, 0, 0xc0);
    Deallocate(tmp);

    tmp = LoadFile(0x980b);
    GX_LoadBGPltt(tmp, 0, 0xc0);
    Deallocate(tmp);

    mStepDelay = 0;
    mIconCount = 0;

    if (param1 == 0) {
        if (func_0203da3c() == 0) {
            func_02030aa4(0);
            SetSteps(&data_ov075_0211d888);
        } else {
            SetSteps(&EntryPlayerSelectIntroSteps);
        }
        ResetMatchRecords();
        mode = 0;
    } else {
        SetSteps(&EntryCourseSelectSteps);
        mode = 2;
    }

    mEntObj = (dEntObj_c *)dBase_c::Spawn(0x15a, gCurrentScene, mode, 0);
    return 1;
}

void dScEntry_c::LeaveSettings() {
    if (data_0209fc68) {
        ChangeStepsAfterDelay(&data_ov075_0211d828, 0x14);
    } else {
        ChangeStepsAfterDelay(&EntryCourseSelectSteps, 0x14);
    }
}

void dScEntry_c::StartMatch() {
    PrepareVsMode();
    u8 course = data_0209b2e4;
    LoadLevelNoReturn(data_ov075_0211c6ec[course], 0, 2, 0);
    Sound::StopLoadedMusic_Layer1(0x3c);
    data_02092778 = 1;
}

/* ArePlayersReady's twin for the rematch handshake (coined name): every
 * joined player must show the rematch ready bit (link settings byte 2,
 * bit 4) that EnterRematchSync raises through func_020201c0. */
int dScEntry_c::ArePlayersReadyForRematch() {
    int i;
    for (i = 0; i < 4; i++) {
        if (data_0209fc5c[i]) {
            int r = func_020201f0(i);
            if (!r)
                return 0;
        }
    }
    return 1;
}

int dScEntry_c::ArePlayersReady() {
    int player = 0;
    u8 *active = data_0209fc5c;
    for (;;) {
        if (*active != 0) {
            int ready = func_02020364(player);
            if (ready == 0)
                return 0;
        }
        ++player;
        ++active;
        if (player >= 4)
            break;
    }
    return 1;
}

int dScEntry_c::CheckMenuConfirm(int player) {
    u16 pressed;
    if (player >= 0)
        pressed = data_020a0e58[player].pressed;
    else
        pressed = data_020a0e58[func_0203da9c()].pressed;
    if (pressed != 0) {
        if (pressed & 0xc)
            return 1;
        func_02012790(0xe);
    }
    return 0;
}

void dScEntry_c::ResetMatchRecords() {
    int i;
    for (i = 0; i < 4; i++) {
        data_0209b2f0[i] = 0;
        data_0209b2ec[i] = 0;
    }
    data_0209b2e4 = 0;
}

void dScEntry_c::UpdateSettingsSync() {
    if (func_02020168()) {
        func_02020304();
        func_020200e0();
        G2_BlendNone();
        ChangeStepsAfterDelay(&EntryCourseSelectSteps, 5);
        return;
    }
    int a = func_0203d9b4();
    if (a == 0)
        return;
    int b = ArePlayersReady();
    if (b == 0)
        return;
    func_02020124();
}

void dScEntry_c::EnterSettingsSync() {
    gMainScreenLayers &= ~DISPLAY_LAYER_BG2;
    gSubScreenLayers &= ~1;
    ENTRY_GRAPH_CALLBACK->SetBg2ScreenBase(6);
    func_020338b0(0x145, 0xb0, 0x1d, -1);
    gSubScreenLayers |= 5;
    mIconCount = 0;
    func_02020334();
    func_02012790(0x120);
}

void dScEntry_c::RenderSoundSettings() {
    unsigned char idx = mSelectedIcon;
    int a1, a2;
    if (idx < 2) {
        a2 = (short)(idx * 0x30 + 0x38);
        a1 = 0xe;
    } else {
        int j = GetLanguageIndex();
        a1 = data_ov075_0211b5d4[j];
        a2 = 0xa8;
    }
    OAM::RenderSub((OamAttr *)data_ov075_0211c790, a1, a2);

    /* mStepWork[1] counts frames; its bit 3 bobs the three arrows. */
    mStepWork[1] += 1;
    int arrowY = (mStepWork[1] & 8) ? 0xac : 0xae;
    OAM::Render(false, &OAM::BOUNCING_ARROW, 0x40, arrowY, -1, -1, 0x1000, 0x1000, 0, -1);
    OAM::Render(false, &OAM::BOUNCING_ARROW, 0x80, arrowY, -1, -1, 0x1000, 0x1000, 0, -1);
    OAM::Render(false, &OAM::BOUNCING_ARROW, 0xc0, arrowY, -1, -1, 0x1000, 0x1000, 0, -1);
}

void dScEntry_c::UpdateSoundSettings() {
    int w = func_0203da9c();
    int dir;
    u16 flags;

    if (TouchArea_Update((char *)&mIcons[5], -1) != 0 ||
        TouchArea_Update((char *)&mIcons[8], -1) != 0 ||
        ((flags = data_020a0e58[w].pressed) & 8) != 0 || (flags & 4) != 0 ||
        (CheckMenuConfirm(-1) != 0 && mSelectedIcon == 2)) {
        mIcons[5].Select();
        mSelectedIcon = 2;
        LeaveSettings();
        func_02012790(0x11b);
    } else {
        dir = 0;
        if (mIcons[2].mReady != 0) {
            if (TouchArea_Update((char *)&mIcons[2], -1) != 0 ||
                TouchArea_Update((char *)&mIcons[1], -1) != 0 ||
                TouchArea_Update((char *)&mIcons[6], -1) != 0) {
                mSelectedIcon = 0;
                dir = 1;
            } else if (TouchArea_Update((char *)&mIcons[0], -1) != 0) {
                mSelectedIcon = dir;
                dir = -1;
            } else if (CheckMenuConfirm(-1) != 0) {
                if (mSelectedIcon == 0)
                    dir = 1;
            }
            if (dir != 0 && mSelectedIcon == 0) {
                int mode = GetSoundMode();
                unsigned newmode;
                if (dir > 0) {
                    if (mode == 2)
                        newmode = 0;
                    else
                        newmode = (mode + 1) & 0xff;
                    mIcons[1].mOn = 0;
                    func_ov001_020ab3f0(&mIcons[1]);
                } else {
                    if (mode == 0)
                        newmode = 2;
                    else
                        newmode = (mode - 1) & 0xff;
                    mIcons[0].mOn = 0;
                    func_ov001_020ab3f0(&mIcons[0]);
                }
                SetSoundMode(newmode);
                Message::DisplayOptionsMenuTextVS((GetSoundMode() + 9));
                mIcons[2].Select();
                func_02012790(100);
            }
        }
        if (mIcons[3].mReady != 0 && mIcons[4].mReady != 0) {
            if ((CheckMenuConfirm(-1) != 0 && mSelectedIcon == 1) ||
                TouchArea_Update((char *)&mIcons[7], -1) != 0) {
                dir = 1;
                mSelectedIcon = 1;
                if (mIcons[3].mOn == 0)
                    dir = -1;
            }
            if (TouchArea_Update((char *)&mIcons[3], -1) != 0 || (dir < 0 && mSelectedIcon == 1)) {
                TurnBacklightOn();
                mIcons[3].Select();
                mIcons[4].mOn = 0;
                mSelectedIcon = 1;
                func_02012790(0x66);
            } else if (TouchArea_Update((char *)&mIcons[4], -1) != 0 ||
                       (dir > 0 && mSelectedIcon == 1)) {
                TurnBacklightOff();
                mIcons[3].mOn = 0;
                mIcons[4].Select();
                mSelectedIcon = 1;
                func_02012790(0x66);
            }
        }
    }

    /* Light the selected option's label: two 11 x 2-tile strips of the
       BG2 screen from tile (2, 6), six rows apart, take palette 4 when
       their index is mSelectedIcon and palette 3 otherwise. */
    {
        int i;
        for (i = 0; i < 2; i++) {
            u16 *p = static_cast<u16 *>(G2S_GetBG2ScrPtr()) + 6 * 32 + 2 + i * 6 * 32;
            int pal = (i == mSelectedIcon) ? 0x4000 : 0x3000;
            int j;
            for (j = 0; j < 11; j++, p++) {
                *p = (*p & 0xfff) | pal;
                p[32] = (p[32] & 0xfff) | pal;
            }
        }
    }
}

#define SH(base, i) (*(short *)((char *)(base) + (i) * 4))

void dScEntry_c::EnterSoundSettings() {
    ENTRY_GRAPH_CALLBACK->QueueBg2Screen(LoadFile(data_ov075_0211ca08[GetLanguageIndex()]));

    Message::DisplayOptionsMenuTextVS((GetSoundMode() + 9));

    gSubScreenLayers |= 5;
    G2_SetBlendBrightness(0xb, ~5);

    mSelectedIcon = 0;

    mIcons[6].Place(5, 0, (SH(data_ov075_0211d72c, GetLanguageIndex()) + 8), 0x38);
    mIcons[7].Place(6, 0, (SH(data_ov075_0211d740, GetLanguageIndex()) + 8), 0x68);
    mIcons[8].Place(0xc, 0, 0x56, 0xa8);
    mIcons[2].Place(1, 0, 0xac, 0x38);
    mIcons[0].Place(3, 0, 0x64, 0x38);
    mIcons[1].Place(4, 0, 0xf4, 0x38);

    mIcons[2].mOn = 1;

    mIcons[3].Place(2, 0, 0x8c, 0x68);
    mIcons[4].Place(2, 0, 0xcc, 0x68);

    if (data_0208ee3c != 0)
        mIcons[3].mOn = 1;
    else
        mIcons[4].mOn = 1;

    mIcons[5].Place(0xb, 0, 0x80, 0xa8);

    mIconCount = 9;

    func_02020304();
}

#undef SH

void dScEntry_c::RenderControlSettings() {
    int *tbl = data_ov075_0211c9cc[GetLanguageIndex()];
    OamAttr *attr = (OamAttr *)tbl[mStepWork[0]];
    OAM::Render(false, attr, 0x80, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);

    int a1, a2;
    unsigned char idx = mSelectedIcon;
    if (idx < 3) {
        a2 = (short)(idx * 0x28 + 0x30);
        a1 = 0x26;
    } else {
        int j = GetLanguageIndex();
        a1 = data_ov075_0211b5d4[j];
        a2 = 0xa8;
    }
    OAM::RenderSub((OamAttr *)data_ov075_0211c790, a1, a2);
}

void dScEntry_c::UpdateControlSettings() {
    int w = func_0203da9c();

    if (TouchArea_Update((char *)&mIcons[3], -1) != 0 ||
        TouchArea_Update((char *)&mIcons[4], -1) != 0 || (data_020a0e58[w].pressed & 8) != 0 ||
        (data_020a0e58[w].pressed & 4) != 0 || (CheckMenuConfirm(-1) != 0 && mSelectedIcon == 3)) {
        int i;
        for (i = 0; i <= 2; i++) {
            if (mIcons[i].mOn != 0) {
                SetControllerMode(i);
                func_02020028(w, i);
            }
        }
        mIcons[3].Select();
        mSelectedIcon = 3;
        LeaveSettings();
        func_02012790(0x11a);
        return;
    }

    if (TouchArea_Update((char *)&mIcons[0], -1) != 0 ||
        (CheckMenuConfirm(-1) != 0 && mSelectedIcon == 0)) {
        mSelectedIcon = 0;
        if (mIcons[0].mOn != 0) {
            SetControllerMode(0);
            func_02020028(w, 0);
            LeaveSettings();
            func_02012790(0x11a);
        } else {
            mIcons[1].mOn = 0;
            mIcons[2].mOn = 0;
            mStepWork[0] = mSelectedIcon;
            SetBg2Offset(0, 0);
            func_02012790(0x5b);
        }
        mIcons[0].Select();
        return;
    }

    if (TouchArea_Update((char *)&mIcons[1], -1) != 0 ||
        (CheckMenuConfirm(-1) != 0 && mSelectedIcon == 1)) {
        mSelectedIcon = 1;
        if (mIcons[1].mOn != 0) {
            SetControllerMode(1);
            func_02020028(w, 1);
            LeaveSettings();
            func_02012790(0x11a);
        } else {
            mIcons[0].mOn = 0;
            mIcons[2].mOn = 0;
            mStepWork[0] = mSelectedIcon;
            SetBg2Offset(0x100, 0);
            func_02012790(0x5b);
        }
        mIcons[1].Select();
        return;
    }

    if (TouchArea_Update((char *)&mIcons[2], -1) != 0 ||
        (CheckMenuConfirm(-1) != 0 && mSelectedIcon == 2)) {
        mSelectedIcon = 2;
        if (mIcons[2].mOn != 0) {
            SetControllerMode(2);
            func_02020028(w, 2);
            LeaveSettings();
            func_02012790(0x11a);
        } else {
            mIcons[0].mOn = 0;
            mIcons[1].mOn = 0;
            mStepWork[0] = mSelectedIcon;
            SetBg2Offset(0, 0x100);
            func_02012790(0x5b);
        }
        mIcons[2].Select();
        return;
    }
}

void dScEntry_c::EnterControlSettings() {
    int i;
    int r;
    int y;
    icon_c *icon;

    i = 0;
    icon = mIcons;
    y = 0x30;
    mSelectedIcon = data_0209f498[gActivePlayerSlot].mode;

    for (; i <= 2; i++) {
        icon->Place(0, 0, 0x80, y);
        y += 0x28;
        icon++;
    }

    mIcons[mSelectedIcon].mOn = 1;

    mIcons[3].Place(0xb, 0, 0x80, 0xa8);

    r = GetLanguageIndex();
    mIcons[4].Place(0xc, 0, data_ov075_0211b5d4[r], 0xa8);

    mIconCount = 5;
    gSubScreenLayers &= ~5;

    r = GetLanguageIndex();
    ENTRY_GRAPH_CALLBACK->QueueBg2Screen(LoadFile(data_ov075_0211c9b8[r]));

    if (mSelectedIcon == 0)
        SetBg2Offset(0, 0);
    else if (mSelectedIcon == 1)
        SetBg2Offset(0x100, 0);
    else
        SetBg2Offset(0, 0x100);

    func_02033bb8(5);
    gMainScreenLayers |= DISPLAY_LAYER_BG2;
    gSubScreenLayers |= 5;
    G2_SetBlendBrightness(0xb, ~5);

    mStepWork[0] = mSelectedIcon;
    func_02020304();
}

void dScEntry_c::EnterQuitMessage() {
    gMainScreenLayers =
        gMainScreenLayers & ~(DISPLAY_LAYER_BG1 | DISPLAY_LAYER_BG2 | DISPLAY_LAYER_BG3);
    gSubScreenLayers = gSubScreenLayers & (~1);
    ENTRY_GRAPH_CALLBACK->SetBg2ScreenBase(6);
    func_020338b0(0x145, 0xb0, 0x17, -1);
    gSubScreenLayers = gSubScreenLayers | 5;
    mIconCount = 0;
    G2_SetBlendBrightness(0x1f, ~5);
    mStepDelay = 2;
}

void dScEntry_c::UpdateQuit() {
    mStepDelay -= data_0208ee44;
    if (mStepDelay > 0)
        return;
    dScene_c::StartSceneFade(1, 0, 0);
    Sound::StopLoadedMusic_Layer1(0x3c);
    data_02092778 = 1;
}

void dScEntry_c::EnterQuit() {
    gMainScreenLayers &= ~(DISPLAY_LAYER_BG1 | DISPLAY_LAYER_BG2 | DISPLAY_LAYER_BG3);
    mStepDelay = 0x14;
    UnloadOverlay((int)&OVERLAY_100_ID);
    UnloadOverlay((int)&OVERLAY_102_ID);
}

static inline unsigned IsMode6(dScEntry_c *scene) {
    /* materialized: returning the comparison directly shrinks the caller (0x15c) */
    unsigned dok = (scene->actorID == 6);
    return dok;
}

void dScEntry_c::UpdateQuitConfirm() {
    int id;

    id = func_0203da9c();
    if (TouchArea_Update((char *)&mIcons[0], id) != 0 ||
        (CheckMenuConfirm(id) != 0 && mSelectedIcon == 0)) {
        mIcons[1].mOn = 0;
        mIcons[0].Select();
        if (func_0203d9b4() != 0 && IsMode6(this)) {
            ChangeStepsAfterDelay(&data_ov075_0211d8e8, 0x14);
        } else {
            func_02030a78();
            ChangeStepsAfterDelay(reinterpret_cast<const StepSet *>(&data_ov075_0211d930), 0x14);
        }
        func_02012790(0x63);
        return;
    }
    if (TouchArea_Update((char *)&mIcons[1], id) == 0) {
        if (CheckMenuConfirm(id) == 0)
            return;
        if (mSelectedIcon != 1)
            return;
    }
    mIcons[0].mOn = 0;
    mIcons[1].Select();
    if (IsMode6(this)) {
        ChangeStepsAfterDelay(&data_ov075_0211d888, 0x14);
        func_02030aa4(3);
    } else {
        ChangeStepsAfterDelay(&data_ov075_0211d810, 0x14);
    }
    func_02012790(0x98);
}

/* The ENTRY scene (actorID 6) only has Message::DisplayDontSaveTextVS draw sub-screen
 * message 0x19; the RESULT scene loads file 0x9802, draws message 0x16
 * and takes the rematch prompt's render step. The mode test is a static
 * inline predicate (the ROM materializes the bool), and the render step
 * is one aggregate copy, which keeps both loads ahead of the flag
 * read-modify-write. */
static inline int IsEntryScene(const dScEntry_c *scene) {
    return scene->actorID == 6;
}

void dScEntry_c::EnterQuitConfirm() {
    void *fh;

    gSubScreenLayers = gSubScreenLayers & ~1;
    if (IsEntryScene(this)) {
        Message::DisplayDontSaveTextVS(0x19);
    } else {
        gSubScreenLayers = gSubScreenLayers & ~4;
        fh = LoadFile(0x9802);
        ENTRY_GRAPH_CALLBACK->QueueBg2Screen(fh);
        Message::DisplayDontSaveTextVS(0x16);
        gSubScreenLayers = gSubScreenLayers | 4;
        mRenderStep = data_ov075_0211d810.render;
    }
    gSubScreenLayers = gSubScreenLayers | 1;
    mIconCount = 2;
    mSelectedIcon = 1;
    mIcons[0].Place(8, 0, 0x50, 0xa8);
    mIcons[1].Place(8, 0, 0xb0, 0xa8);
}

void dScEntry_c::UpdateRematchSync() {
    if (data_0209fc58 != 0) {
        func_02020190();
        HandlePlayerDisconnect(&data_ov075_0211d870);
        return;
    }
    if (func_02020168()) {
        func_02020190();
        func_020200e0();
        func_02030994();
        gMainScreenLayers =
            gMainScreenLayers & ~(DISPLAY_LAYER_BG1 | DISPLAY_LAYER_BG2 | DISPLAY_LAYER_BG3);
        mDisconnectHideMask = 0;
        mRenderStep = 0;
        dScene_c::StartSceneFade(6, 2, 0);
        data_02092778 = 1;
        return;
    }
    if (func_0203d9b4() == 0)
        return;
    if (ArePlayersReadyForRematch() == 0)
        return;
    func_02020124();
}

void dScEntry_c::EnterRematchSync() {
    gSubScreenLayers &= ~1;
    ENTRY_GRAPH_CALLBACK->SetBg2ScreenBase(6);
    func_020338b0(0x145, 0xb0, 0x1d, -1);
    gSubScreenLayers |= 5;
    mIconCount = 0;
    if (data_0209fc58 == 0) {
        func_020201c0();
    }
    func_02020304();
    func_02012790(0x120);
}

/* Renders each joined player's two-digit count (the shared base at
 * data_0209ee90+0x1d4 plus that player's data_0209b2ec, mod 100) as
 * tens/ones OAM digits, then every 3 calls toggles a blink flag and has
 * SetSlotBorderPalette repaint each round winner's slot border bright or
 * dim by it. mStepWork[0] counts frames and mStepWork[1] is the flag. */
void dScEntry_c::RenderWinCounts() {
    int count = data_0209fc50;
    int d = 4 - count;
    int yBase = d * 16 + 0x30;
    int stride = d * 8 + 0x28;
    int n, tens;
    int i = 0;

    if (count > 0) {
        int sz = 0x1000;
        int xTens = 0xce;
        int eight = 8;
        int yOff = i;
        int zT = 0;
        int zO = 0;
        int minus1 = -1;
        int xOnes = 0xd8;

        do {
            int *p;

            n = *(int *)(data_0209ee90 + 0x1d4) + data_0209b2ec[func_02030958(i)];
            n = n % 100;
            tens = n / 10;
            if (tens != 0) {
                p = (int *)OAM::Render(zT, (OamAttr *)data_ov075_0211c8b0, xTens, yBase + yOff,
                                       eight, minus1, sz, zT);
                if (p != 0) {
                    int *q = (p + 1);
                    int val = *q;
                    *q = (val & ~0x3ff) | ((tens + ((u32)(val << 22) >> 22)) & 0x3ff);
                }
            }
            p = (int *)OAM::Render(zO, (OamAttr *)data_ov075_0211c8b0, xOnes, yBase + yOff, eight,
                                   minus1, sz, zO);
            if (p != 0) {
                int *q = (p + 1);
                int val = *q;
                *q = (val & ~0x3ff) | ((((u32)(val << 22) >> 22) + n % 10) & 0x3ff);
            }
        } while (yOff += stride, ++i < count);
    }

    mStepWork[0]++;
    if (mStepWork[0] < 3)
        return;

    mStepWork[0] = 0;
    mStepWork[1] ^= 1;

    if (count <= 0)
        return;

    {
        int j = 0;
        do {
            if (data_0209b2f0[func_02030958(j)] != 0)
                SetSlotBorderPalette(j, mStepWork[1]);
            j++;
        } while (j < count);
    }
}

void dScEntry_c::UpdateRematchPrompt() {
    int id;
    if (data_0209fc58 != 0) {
        HandlePlayerDisconnect(&data_ov075_0211d810);
        return;
    }
    id = func_0203da9c();
    if (TouchArea_Update((char *)&mIcons[0], id) != 0 ||
        (CheckMenuConfirm(id) != 0 && mSelectedIcon == 0)) {
        mIcons[1].mOn = 0;
        mIcons[0].Select();
        ChangeStepsAfterDelay(&data_ov075_0211d870, 0x14);
        func_02012790(0x126);
        return;
    }
    if (TouchArea_Update((char *)&mIcons[1], id) == 0) {
        if (CheckMenuConfirm(id) == 0)
            return;
        if (mSelectedIcon != 1)
            return;
    }
    mIcons[0].mOn = 0;
    mIcons[1].Select();
    ChangeStepsAfterDelay(&data_ov075_0211d8b8, 0x14);
    func_02012790(0x119);
}

/* Sets gMainScreenLayers bits 0xe and records them in mDisconnectHideMask; on
 * a player disconnect HandlePlayerDisconnect clears gMainScreenLayers by
 * ~mDisconnectHideMask, and UpdateRematchSync clears both on its own. */
void dScEntry_c::EnterRematchPrompt() {
    gSubScreenLayers &= ~5;
    ENTRY_GRAPH_CALLBACK->SetBg2ScreenBase(1);
    ENTRY_GRAPH_CALLBACK->QueueBg2Screen(LoadFile(0x9801));
    LoadCompressedFileAt(0x8407, G2S_GetBG3ScrPtr());
    Message::DisplayVsExitText(0x1f);

    gMainScreenLayers |= DISPLAY_LAYER_BG1 | DISPLAY_LAYER_BG2 | DISPLAY_LAYER_BG3;
    gSubScreenLayers |= 5;
    mSelectedIcon = 0;
    mIcons[0].Place(0xa, 0, 0x80, 0x48);
    mIcons[1].Place(0xa, 0, 0x80, 0x78);
    mIconCount = 2;
    G2_SetBlendAlpha(4, 0x11, 0xc, 4);
    mDisconnectHideMask = 0xe;
    if (data_0209fc68 != 0)
        data_0209fc68 = 5;
    func_02020190();
    mStepWork[0] = 0;
    mStepWork[1] = 0;
    func_02012790(0x125);
    Sound::LoadAndSetMusic_Layer1(0x4c);
}

void dScEntry_c::RenderResults() {
    int id = func_0203da9c();
    OAM::Render(false, &data_ov075_0211cb84, 0x80, 0x20, -1, -1, 0x1000, 0x1000, 0, -1);
    RenderPlayerScores();

    u8 *p = data_0209b2f0 + id;
    OamAttr *sb;
    if (*p != 0)
        sb = data_ov075_0211c9f4[GetLanguageIndex()];
    else
        sb = data_ov075_0211c9e0[GetLanguageIndex()];

    {
        int i = 0;
        int x = 0x80;
        int y = 0x60;
        do {
            int fix = (i == mStepWork[1]) ? 0xe00 : 0x1000;
            OAM::Render(true, sb, x, y, -1, -1, fix, 0);
            if (sb->attr3 == 0xffff)
                break;
            sb++;
            i++;
        } while (1);

        /* mStepWork[1] is the sprite drawn at 0xe00 scale; every third
           frame (mStepWork[0]) it moves one along the string and wraps. */
        mStepWork[0]++;
        if (mStepWork[0] >= 3) {
            mStepWork[0] = 0;
            mStepWork[1]++;
            if (mStepWork[1] > i)
                mStepWork[1] = 0;
        }
    }

    if (*p != 0) {
        func_0202048c(&mOamAnimations[2], 0x80, 0x88);
        func_0202048c(&mOamAnimations[0], 0x80, 0x88);
    } else {
        func_0202048c(&mOamAnimations[0], 0x80, 0x88);
        func_0202048c(&mOamAnimations[3], 0x6c, 0x6e);
        func_0202048c(&mOamAnimations[2], 0xa6, 0x6e);
    }
    func_0202048c(&mOamAnimations[1], 0x80, 0x88);
}

void dScEntry_c::UpdateResults() {
    int id;
    int acc;
    int i;
    u8 *p;

    id = func_0203da9c();
    if (func_02020168() != 0) {
        func_02020304();
        func_020200e0();
        ChangeStepsAfterDelay(&data_ov075_0211d810, 5);
        func_02012790(0x124);
    } else {
        if (func_0203d9b4() != 0) {
            /* mStepWork[3]: set once the mStepDelay countdown has run out
               and func_02020334 has been sent; this step reads it back. */
            if (mStepWork[3] == 0) {
                acc = 0;
                for (i = 0; i < 4; i++) {
                    if (data_0209fc5c[i] != 0) {
                        acc |= data_020a0e58[i].pressed & 0xc;
                        acc |=
                            ((TouchInfo *)gTouchHeld)[i].held && ((TouchInfo *)gTouchHeld)[i].edge;
                    }
                }
                mStepDelay -= data_0208ee44;
                if (mStepDelay <= 0 || (mStepDelay < 0x21c && acc != 0)) {
                    func_02020334();
                    mStepWork[3] = 1;
                }
            } else {
                if (ArePlayersReady() != 0)
                    func_02020124();
            }
        } else if (mStepWork[3] == 0) {
            mStepDelay -= data_0208ee44;
            if (mStepDelay < 0x1e0) {
                func_02020334();
                mStepWork[3] = 1;
            }
        }
        if ((data_020a0e58[func_0203da9c()].pressed & ~0xc) != 0)
            func_02012790(0xe);
    }

    func_02020768((char *)&mOamAnimations[0]);
    func_02020768((char *)&mOamAnimations[1]);
    func_02020768((char *)&mOamAnimations[2]);
    p = data_0209b2f0 + id;
    if (*p == 0)
        func_02020768((char *)&mOamAnimations[3]);
    if (mOamAnimations[0].enabled == 0) {
        if (mOamAnimations[1].enabled == 0) {
            mOamAnimations[0].affine = 0;
            mOamAnimations[1].enabled = 1;
            mOamAnimations[1].affine = 1;
            if (*p == 0) {
                mOamAnimations[2].enabled = 1;
                mOamAnimations[2].affine = 1;
                mOamAnimations[3].enabled = 1;
                mOamAnimations[3].affine = 1;
            }
        }
    }
}

void dScEntry_c::EnterResults() {
    int id;

    id = func_0203da9c();
    mStepDelay = 0x258;
    mStepWork[0] = 0;
    mStepWork[1] = 0;

    if (data_0209b2f0[id] != 0) {
        LoadCompressedFileAt(0x8404, G2S_GetBG3ScrPtr());
        func_02020820(&mOamAnimations[0], &data_ov075_0211ca64, 1, 1, 1, 1);
        func_02020820(&mOamAnimations[1], &data_ov075_0211ca6c, 1, 0, 0, 0);
        func_02020820(&mOamAnimations[2], &data_ov075_0211ca74, 1, 1, 1, 1);
    } else {
        LoadCompressedFileAt(0x8401, G2S_GetBG3ScrPtr());
        func_02020820(&mOamAnimations[0], &data_ov075_0211cc44, 1, 1, 1, 1);
        func_02020820(&mOamAnimations[1], &data_ov075_0211cc4c, 1, 0, 0, 0);
        func_02020820(&mOamAnimations[2], &data_ov075_0211cc54, 1, 0, 0, 0);
        func_02020820(&mOamAnimations[3], &data_ov075_0211cc5c, 1, 0, 0, 0);
    }

    mOamAnimations[0].palette = id + 0xa;
    mOamAnimations[1].palette = id + 0xa;

    {
        void *bg0 = G2S_GetBG0ScrPtr();
        volatile int zero = 0;
        MI_CpuFill32(bg0, zero, 0x800);
    }

    func_02033390(0);

    func_020331fc(0x40, 0x20, func_0203da9c() << 24 >> 24);

    gSubScreenLayers |= 1;

    func_02020304();

    mStepWork[3] = 0;
}

/* The behavior of the step set at data_ov075_0211d7c8, which the static
 * initializer fills and no code installs. It only arms the link
 * handshake (func_020308b4). */
void dScEntry_c::func_ov075_02118a78() {
    func_020308b4();
}

// local extern: the Fix12-by-value overload retains its measured scalar ABI bridge.
extern "C" void OamRender10_d_s_entry_methods(int sub, struct OamAttr *data, s32 x, s32 y,
                                              s32 palette, s32 priority, s32 scaleX, s32 scaleY,
                                              s32 rotation, s32 mode);

void dScEntry_c::UpdateMatchLoading() {
    if (mSawLoadSignal == 0) {
        if (func_0203d8fc() != 0)
            mSawLoadSignal = 1;
    }
    if (mStepPhase == MATCH_LOAD_RESOURCES) {
        if (data_0209fc68 != 0) {
            if (EntryCourseLoadStarted == 0) {
                func_0201a244(&dScEntry_c::LoadCourseResources, data_0209b2e4, 0xf, 0, 0x1000);
                EntryCourseLoadStarted = 1;
            }
            if (EntryCourseLoadStarted != 0 && func_0201a1bc() != 0) {
                mStepPhase = MATCH_LOAD_WAIT_FOR_SIGNAL_CLEAR;
                EntryCourseLoadStarted = 0;
                func_0203d930();
            }
        } else {
            mStepPhase = MATCH_LOAD_READY;
        }
    }
    if (mStepPhase == MATCH_LOAD_WAIT_FOR_SIGNAL_CLEAR && mSawLoadSignal != 0) {
        if (func_0203d8fc() == 0)
            mStepPhase = MATCH_LOAD_READY;
    }
    if (mStepDelay > 0) {
        mStepDelay -= data_0208ee44;
    }
    if (mStepPhase < MATCH_LOAD_READY)
        return;
    if (mStepDelay > 0)
        return;
    StartMatch();
    mBehaviorStep = 0;
}

void dScEntry_c::LoadCourseResources(int course) {
    mHeap::Heap_t *previousHeap = data_020a0eac->SetDefault();
    LoadArchive(7);
    int selectedArchive = course + 2;
    unsigned int archive;
    for (archive = 2; archive <= 5; ++archive) {
        if (archive == selectedArchive)
            continue;
        UnloadArchive(archive);
    }
    LoadArchive(selectedArchive);
    previousHeap->SetDefault();
    gGameMode = 1;
    func_0202deac(data_ov075_0211c6ec[course]);
}

void dScEntry_c::EnterMatchLoading() {
    mEntObj->func_ov075_02114fa8();
    mStepDelay = 240;
    mStepPhase = MATCH_LOAD_RESOURCES;
    mSawLoadSignal = 0;
    if (data_0209fc68 != 0)
        data_0208a0e0 = 4;
    func_0203d918();
}

void dScEntry_c::RenderCourseSelectTitle() {
    OAM::Render(0, data_ov075_0211cba4, 0x80, 0x20, -1, -1, 0x1000, 0x1000, 0, -1);
}

void dScEntry_c::UpdateCourseSelect() {
    if (func_0203d974() && func_020202dc() != data_0209b2e4) {
        data_0209b2e4 = func_020202dc();
    }
    if (func_02020168()) {
        u8 action;
        func_02020304();
        func_020200e0();
        action = func_020200b8();
        if (action == 0) {
            int index;
            func_ov001_020ab3f0(&mIcons[data_0209b2e4]);
            mIcons[data_0209b2e4].mReady = 0;
            index = 0;
            for (; index < 4; index++) {
                if (index != data_0209b2e4)
                    mIcons[index].mOn = 0;
            }
            SetSteps(&EntryMatchLoadingSteps);
            func_02012790(0xa0);
            func_02012790(0x128);
            return;
        }
        if (action == 1) {
            SetSteps(&data_ov075_0211d798);
            func_02012790(0x9e);
            return;
        }
        SetSteps(&data_ov075_0211d7f8);
        func_02012790(0x9f);
        return;
    }
    if (func_0203d9b4()) {
        if (ArePlayersReady()) {
            bool touched;
            int index;
            touched = false;
            index = 0;
            icon_c *icon = mIcons;
            for (; index < 4; index++) {
                if (TouchArea_Update((char *)icon, 0)) {
                    touched = true;
                    data_0209b2e4 = index;
                    break;
                }
                ++icon;
            }
            if (touched) {
                func_02020078(0);
                func_02020124();
            } else {
                if (data_020a0e58[gActivePlayerSlot].pressed & 4) {
                    func_02020078(1);
                    func_02020124();
                } else if (data_020a0e58[gActivePlayerSlot].pressed & 8) {
                    func_02020078(2);
                    func_02020124();
                }
            }
            func_0202029c(data_0209b2e4);
        }
    }
    G2_BlendNone();
    if (data_020a0e58[func_0203da9c()].pressed & ~0xc) {
        func_02012790(0xe);
    }
}

void dScEntry_c::EnterCourseSelect() {
    int index;
    int x;
    icon_c *icon;

    gMainScreenLayers &= ~DISPLAY_LAYER_BG2;
    gSubScreenLayers &= ~5;
    ENTRY_GRAPH_CALLBACK->SetBg2ScreenBase(1);
    ENTRY_GRAPH_CALLBACK->QueueBg2Screen(LoadFile(0x8409));
    LoadCompressedFileAt(0x8407, G2S_GetBG3ScrPtr());

    if (func_0203d9b4() != 0)
        func_020338b0(0x205, 0xb0, 0x21, -1);
    else
        func_020338b0(0x225, 0xb0, 0x1d, -1);

    gSubScreenLayers |= 5;

    icon = mIcons;
    index = 0;
    x = 0x20;
    for (; index < 4; index++) {
        icon->Place(7, index, x, 0x48);
        mIcons[index].mOn = 1;
        mIcons[index].mPressTime = 8;
        x += 0x40;
        ++icon;
    }

    data_0209b2e4 = 4;
    mIcons[4].Place(0xd, 4, 0x80, -0x14);
    mIconCount = 5;
    func_02020334();
    func_020200e0();
}

void dScEntry_c::UpdatePlayerSelectIntro() {
    if (func_0203d974()) {
        if (!mPlayerSelectReady) {
            mEntObj->func_ov075_02115134();
            if (mEntObj->func_ov075_02115098(func_0203da9c()))
                mPlayerSelectReady = 1;
        }
        if (mPlayerSelectReady)
            mStepPhase = 1;
    }

    if (func_02020168()) {
        func_02020304();
        func_020200e0();
        ChangeStepsAfterDelay(&EntryCourseSelectSteps, 5);
        return;
    }

    if (mStepDelay > 0) {
        mStepDelay -= data_0208ee44;
        return;
    }
    if (!mStepPhase)
        return;
    func_02020334();
    if (!func_0203d9b4())
        return;
    if (!ArePlayersReady())
        return;
    func_02020124();
}

void dScEntry_c::EnterPlayerSelectIntro() {
    int row;
    int col;
    u16 *tiles;
    u32 palette;
    gSubScreenLayers &= ~5;
    if (GetLanguageIndex() == 0)
        LoadCompressedFileAt(0x840a, G2S_GetBG2ScrPtr());
    else
        LoadCompressedFileAt(0x980c, G2S_GetBG2ScrPtr());
    LoadCompressedFileAt(0x840b, G2S_GetBG3ScrPtr());

    // Color the 9-by-10 tile panel for this player's wireless station.
    tiles = static_cast<u16 *>(G2S_GetBG2ScrPtr()) + 11 * 32 + 11;
    palette = (static_cast<u32>(func_0203da9c() + 10) << 28) >> 16;
    for (row = 0; row < 10; ++row) {
        for (col = 0; col < 9; ++col)
            tiles[col] = palette | (tiles[col] & 0xfff);
        tiles += 32;
    }
    gSubScreenLayers |= 4;
    mIconCount = 0;
    mStepDelay = 180;
    if (func_0203d974()) {
        mStepPhase = 0;
        mPlayerSelectReady = 0;
        mWaiting.syncTicks = 0;
    } else {
        mStepPhase = 1;
    }
    func_02012790(0x122);
}

void dScEntry_c::UpdatePlayerSync() {
    mSoundHandle = Sound_PlayIfNotActive(mSoundHandle, 2, 0x11d, 0);

    switch (mStepPhase) {
    case 0: {
        u8 activePlayers = data_0209fc50;
        int sessionPlayers = func_0203d890();
        if (activePlayers == sessionPlayers) {
            func_020308b4();
            ++mStepPhase;
        } else {
            ++mWaiting.playerWaitTicks;
            if (mWaiting.playerWaitTicks >= 900)
                data_0209fc54 = 1;
        }
        break;
    }
    case 1:
        if (func_020308a8())
            ++mStepPhase;
        break;
    case 2: {
        int hostReady = func_0203d8fc();
        if (hostReady) {
            func_020308b4();
            ++mStepPhase;
        }
        break;
    }
    case 3:
        if (func_020308a8()) {
            ChangeStepsWithFade(&EntryPlayerSelectIntroSteps);
            u8 observedPlayers = data_0208f274;
            int sessionPlayers = func_0203d890();
            if (observedPlayers == sessionPlayers) {
                func_02012790(0x11f);
                return;
            }
        }
        break;
    }

    if (mStepPhase < 1)
        return;
    ++mWaiting.syncTicks;
    if (mWaiting.syncTicks >= 2700)
        data_0209fc54 = 1;
}

void dScEntry_c::EnterPlayerSync() {
    // Download-play clients and non-host stations install the waiting screen.
    if (func_0203d974()) {
        gSubScreenLayers &= ~5;
        void *screen = LoadFile(0x8409);
        ENTRY_GRAPH_CALLBACK->QueueBg2Screen(screen);
        func_020338b0(0x225, 0xb0, 0x1d, -1);
        gSubScreenLayers |= 5;
        mIconCount = 0;
        mWaiting.wavePhase = 0;
        mWaiting.waveDelay = 120;
    }
    mStepPhase = 0;
    mWaiting.playerWaitTicks = 0;
    mWaiting.syncTicks = 0;
}

void dScEntry_c::RenderWaitingText() {
    OamAttr *glyph = data_0208a0f8[GetLanguageIndex()];

    if (mWaiting.waveDelay <= 0) {
        int phase;
        bool waving;

        phase = mWaiting.wavePhase;
        waving = false;
        do {
            int y;
            if (phase >= 0 && phase < 0x10000) {
                waving = true;
                // The intermediate narrowing preserves the retail ARM shifts.
                y = (FX_SinCosTable_[2 * ((u16)(s16)(u16)phase >> 4)] >> 10) + 96;
            } else {
                y = 96;
            }
            OAM::Render(true, glyph, 128, y, -1, -1, 0x1000, 0);
            if (glyph->attr3 == 0xffff)
                break;
            ++glyph;
            phase -= 0x2000;
        } while (true);

        mWaiting.wavePhase += 0x800;
        if (!waving)
            mWaiting.waveDelay = 120;
    } else {
        OAM::RenderSub(glyph, 128, 96);
        mWaiting.waveDelay -= data_0208ee44;
        mWaiting.wavePhase = 0;
    }
}

/* The wireless host's steps, every name coined from the bodies. On the host
 * the scene starts in the recruit steps (data_ov075_0211d888) while link
 * mode 3 gathers players. The host's start button then leads to the solo
 * confirm steps (data_ov075_0211d8a0) when nobody joined, or closes
 * recruiting (func_02030a6c) and takes the session start steps
 * (data_ov075_0211d8d0). Both wait for the link mode to drop back to 0. The
 * solo path goes on to course select through data_ov075_0211d900; the session
 * path opens link mode 1 and goes on to player sync. */

/* Behavior of the session start steps. */
void dScEntry_c::UpdateSessionStart() {
    mSoundHandle = Sound_PlayIfNotActive(mSoundHandle, 2, 0x11d, 0);
    if (mStepPhase == 0) {
        mEntObj->func_ov075_02115134();
        mStepPhase = mEntObj->func_ov075_02115098(func_0203da9c());
    }
    if (func_0203da3c() != 0)
        return;
    LoadTextNarcs();
    LoadArchive(0);
    LoadArchive(1);
    func_02030aa4(1);
    SetSteps(&data_ov075_0211d918);
}

/* Enter step of the session start steps: the waiting screen InitResources
   loaded (file 0x8409) goes to BG2. */
void dScEntry_c::EnterSessionStart() {
    mEntObj->func_ov075_02115134();
    gSubScreenLayers &= ~5;
    ENTRY_GRAPH_CALLBACK->QueueBg2Screen(data_0209b2e8);
    data_0209b2e8 = 0;
    func_020338b0(0x225, 0xb0, 0x1d, -1);
    gSubScreenLayers |= 5;
    mIconCount = 0;
    mWaiting.wavePhase = 0;
    mWaiting.waveDelay = 0x78;
    func_02012790(0x11e);
    mStepPhase = mEntObj->func_ov075_02115098(func_0203da9c());
}

/* Behavior of the solo start steps. */
void dScEntry_c::UpdateSoloStart() {
    int linkMode = func_0203da3c();
    if (linkMode != 0)
        return;
    Deallocate(data_0209b2e8);
    data_0209b2e8 = 0;
    LoadTextNarcs();
    LoadArchive(0);
    LoadArchive(1);
    SetSteps(&EntryCourseSelectSteps);
}

/* Enter step of the solo start steps. */
void dScEntry_c::EnterSoloStart() {
    mIconCount = 0;
}

/* Behavior of the solo confirm steps. Yes plays alone; no asks whether to
   quit (data_ov075_0211d8b8). */
void dScEntry_c::UpdateSoloConfirm() {
    if (mStepPhase == 0)
        mStepPhase = mEntObj->func_ov075_02115098(func_0203da9c()) != 0;
    if (mStepPhase == 0)
        return;
    if (TouchArea_Update((char *)&mIcons[0], -1) != 0 ||
        (CheckMenuConfirm(-1) != 0 && mSelectedIcon == 0)) {
        mIcons[1].mOn = 0;
        mIcons[0].Select();
        SetNumPlayers(1);
        ChangeStepsAfterDelay(&data_ov075_0211d900, 0x14);
        func_02012790(0x118);
        return;
    }
    if (TouchArea_Update((char *)&mIcons[1], -1) == 0) {
        if (CheckMenuConfirm(-1) == 0)
            return;
        if (mSelectedIcon != 1)
            return;
    }
    mIcons[0].mOn = 0;
    mIcons[1].Select();
    ChangeStepsAfterDelay(&data_ov075_0211d8b8, 0x14);
    mEntObj->func_ov075_0211505c();
    func_02012790(0x119);
}

/* Enter step of the solo confirm steps: message 0x11 over a yes/no pair,
   and the recruiting link step is asked to wind down (func_02030a38). */
void dScEntry_c::EnterSoloConfirm() {
    gSubScreenLayers = gSubScreenLayers & ~1;
    ENTRY_GRAPH_CALLBACK->SetBg2ScreenBase(2);
    Message::DisplayDontSaveTextVS(0x11);

    gSubScreenLayers = gSubScreenLayers | 5;
    mSelectedIcon = 1;
    mIcons[0].Place(8, 0, 0x50, 0xa8);
    mIcons[1].Place(8, 0, 0xb0, 0xa8);

    mIconCount = 2;
    func_02030a38();
    {
        int localPlayer = func_0203da9c();
        mStepPhase = mEntObj->func_ov075_02115098(localPlayer) != 0;
    }
}

struct SinePair {
    short a, b;
};

/* Render step of the recruit steps: RenderWaitingText's wave, drawn from
   the language's glyph list data_ov075_0211c968 at y 0x1c. */
void dScEntry_c::RenderRecruitText() {
    OamAttr *attr;

    attr = data_ov075_0211c968[GetLanguageIndex()];

    if (mWaiting.waveDelay <= 0) {
        int ang;
        int flag;
        int fallback = 0x1c;
        int zero = 0;
        int one = 1;
        int minus = -1;
        int fx = 0x1000;
        int x = 0x80;

        ang = mWaiting.wavePhase;
        flag = 0;

        do {
            int y;

            if (ang >= 0 && ang < 0x10000) {
                flag = one;
                y = (((SinePair *)FX_SinCosTable_)[(unsigned short)(short)(unsigned short)ang >> 4]
                         .a >>
                     0xa) +
                    fallback;
            } else {
                y = fallback;
            }

            OAM::Render(one, attr, x, y, minus, minus, fx, zero);

            if (attr->attr3 == 0xffff)
                break;

            attr += 1;
            ang -= 0x2000;
        } while (1);

        mWaiting.wavePhase += 0x800;

        if (flag == zero)
            mWaiting.waveDelay = 0x78;
    } else {
        OAM::RenderSub(attr, 0x80, 0x1c);
        mWaiting.waveDelay -= data_0208ee44;
        mWaiting.wavePhase = 0;
    }
}

/* Behavior of the recruit steps. The host's start button (icon 0) ends
   recruiting; with four players it ends without waiting for it. */
void dScEntry_c::UpdateRecruit() {
    int localPlayer = func_0203da9c();
    int playerCount = data_0209fc50;
    for (int i = 1; i < 4; i++) {
        func_02033464((unsigned char)i, (signed char)i, 0);
    }
    mEntObj->func_ov075_02115134();
    mSoundHandle = Sound_PlayIfNotActive(mSoundHandle, 2, 0x11c, 0);
    if (data_0209fc5c[localPlayer] == 0)
        return;
    if (playerCount < 4) {
        if (TouchArea_Update((char *)&mIcons[0], 0) == 0) {
            if (CheckMenuConfirm(0) == 0)
                return;
        }
    }
    func_ov001_020ab3f0(&mIcons[0]);
    if (playerCount <= 1) {
        ChangeStepsAfterDelay(&data_ov075_0211d8a0, 0x14);
    } else {
        func_02030a6c();
        ChangeStepsAfterDelay(&data_ov075_0211d8d0, 0x14);
    }
    func_02012790(0x117);
}

/* Enter step of the recruit steps. */
void dScEntry_c::EnterRecruit() {
    gSubScreenLayers &= ~1;
    ENTRY_GRAPH_CALLBACK->SetBg2ScreenBase(1);
    func_020337ec();
    gSubScreenLayers |= 5;
    mIcons[0].Place(0xa, 0, 0x80, 0xa8);
    mIconCount = 1;
    mStepPhase = 0;
    mWaiting.wavePhase = 0;
    mWaiting.waveDelay = 0x78;
}

void dScEntry_c::UpdateStepFadeOut() {
    ++mFadeTick;
    if (mFadeTick & 1)
        return;
    ++mFadeBrightness;
    G2S_ChangeBlendBrightness(mFadeBrightness);
    if (mFadeBrightness < 16)
        return;
    mIsFading = 1;
    SetSteps(mPendingSteps);
}

void dScEntry_c::EnterStepFadeOut() {
    G2_BlendNone();
    G2S_SetBlendBrightness(0x1d, 0);
    mFadeBrightness = 0;
    mFadeTick = 0;
}

void dScEntry_c::UpdateStepDelay() {
    mStepDelay -= data_0208ee44;
    if (mStepDelay > 0)
        return;
    SetSteps(mPendingSteps);
}

/* State 6 is written by the wireless teardown path. Keep the materialized
 * predicate: flattening it changes the retail branch sequence. */
static inline int HasConnectionError() {
    return data_0209fc68 == 6;
}

void dScEntry_c::HandlePlayerDisconnect(const StepSet *resumeSteps) {
    u8 activePeerMask;
    u8 *peerActive;
    int player;
    u8 disconnectFlags;

    gSubScreenLayers = gSubScreenLayers & ~0x11;
    activePeerMask = 0;
    peerActive = data_0209fc5d;
    for (player = 1; player < 4; player++) {
        if (peerActive[0])
            activePeerMask = activePeerMask | (1 << player);
        peerActive++;
    }
    disconnectFlags = data_0209fc58;
    if (HasConnectionError()) {
        ENTRY_GRAPH_CALLBACK->SetBg2ScreenBase(5);
        func_02030a78();
        {
            if ((disconnectFlags & 0x30) != 0 || mBehaviorStep == data_ov075_0211d920) {
                if (GetOwnerLanguage() == 3)
                    func_020338b0(0xa5, 0xb0, 0x15, -1);
                else
                    func_020338b0(0xc5, 0xb0, 0x15, -1);
            } else {
                func_020338b0(0xc5, 0xb0, 0x13, -1);
            }
        }
        mBehaviorStep = 0;
        gMainScreenLayers = gMainScreenLayers & ~mDisconnectHideMask;
        if (mDisconnectHideMask)
            mRenderStep = 0;
        G2_SetBlendBrightness(0x1f, -6);
        G2S_BlendNone();
        Sound::StopLoadedMusic_Layer1(4);
        func_02012790(0x123);
        mEntObj->mSuspended = 1;
    } else {
        ENTRY_GRAPH_CALLBACK->SetBg2ScreenBase(6);
        if ((func_0203d974() && data_0209fc5c[0] == 0) ||
            (func_0203d9b4() && activePeerMask == 0)) {
            if (func_0203d974())
                func_020338b0(0x125, 0xb0, 0x1b, 0);
            else
                func_020338b0(0x125, 0xb0, 0x1a, -1);
            func_02030a78();
            ChangeStepsAfterDelay(reinterpret_cast<const StepSet *>(&data_ov075_0211d930), 0xb4);
            gMainScreenLayers = gMainScreenLayers & ~mDisconnectHideMask;
            if (mDisconnectHideMask)
                mRenderStep = 0;
            G2_SetBlendBrightness(0x1f, -6);
            func_02012790(0x12a);
        } else {
            for (player = 0; player < 4; player++) {
                int bit = 1 << player;
                if (disconnectFlags & bit) {
                    func_020338b0(0x125, 0xb0, 0x1c, (s8)(u8)player);
                    data_0209fc58 = data_0209fc58 & ~bit;
                    ChangeStepsAfterDelay(resumeSteps, 0x78);
                    func_02012790(0x12a);
                    break;
                }
            }
        }
    }
    gSubScreenLayers = gSubScreenLayers | 5;
    mIconCount = 0;
}

void dScEntry_c::ChangeStepsWithFade(const StepSet *steps) {
    mPendingSteps = steps;
    Step render = mRenderStep;
    SetSteps(&EntryFadeOutSteps);
    mRenderStep = render;
}

void dScEntry_c::ChangeStepsAfterDelay(const StepSet *steps, s32 delay) {
    mPendingSteps = steps;
    mStepDelay = delay;
    Step render = mRenderStep;
    SetSteps(&EntryDelaySteps);
    mRenderStep = render;
}

void dScEntry_c::SetSteps(const StepSet *steps) {
    mEnterStep = steps->enter;
    mBehaviorStep = steps->behavior;
    mRenderStep = steps->render;
    if (mEnterStep != 0)
        (this->*mEnterStep)();
}

/* Slot 3. Releases the level-entry scratch buffer, unregisters the
 * graph callback, drops the voice group and clears the shared
 * model-data array. */
s32 dScEntry_c::CleanupResources() {
    func_020308b4();
    if (data_0209b2e8 != 0) {
        Deallocate(data_0209b2e8);
        data_0209b2e8 = 0;
    }
    gGraphCallback = 0;
    Sound::UnsetPlayerVoiceGroup();
    CleanCommonModelDataArr();
    return 1;
}

/* Slot 12. Empty override. */
void dScEntry_c::OnPendingDestroy() {}

/* Slot 9. */
s32 dScEntry_c::Render() {
    if (mRenderStep != 0)
        (this->*mRenderStep)();
    func_0203083c();
    return 1;
}

/* Slot 6. Restores the sub-screen brightness after a step change, then the
 * per-frame step, then every live icon. */
s32 dScEntry_c::Behavior() {
    if (mIsFading != 0) {
        mFadeTick++;
        if ((mFadeTick & 1) == 0) {
            mFadeBrightness--;
            G2S_ChangeBlendBrightness(mFadeBrightness);
            if (mFadeBrightness == 0) {
                G2S_BlendNone();
                mFadeBrightness = 0;
                mIsFading = 0;
            }
        }
    }

    if (gSceneFader->IsAtStart() == 0 || mIsFading != 0)
        return 1;

    if (data_0209fc68 != 0) {
        int t = (data_0209fc68 == 6);
        if (t != 0)
            HandlePlayerDisconnect(reinterpret_cast<const StepSet *>(&data_ov075_0211d930));
    }

    if (mBehaviorStep != 0)
        (this->*mBehaviorStep)();

    {
        int i = 0;
        int n = mIconCount;
        if (n > 0) {
            icon_c *o = mIcons;
            do {
                o->Behavior();
                i++;
                o++;
            } while (i < mIconCount);
        }
    }
    return 1;
}

/* Slot 0. */
s32 dScEntry_c::InitResources() {
    Enable3dEngines();
    func_0200f2cc();

    {
        int isResult = (actorID == 7);
        if (isResult || param1 != 0 || func_0203d9b4() != 0) {
            LoadTextNarcs();
            LoadArchive(0);
            LoadArchive(1);
            func_0200f220();
        }
    }

    {
        void *paletteData = LoadFile(0x9807);
        GXS_LoadBGPltt(paletteData, 0, 0xe0);
        Deallocate(paletteData);
    }
    {
        void *paletteData = LoadFile(0x8406);
        GXS_LoadBGPltt(paletteData, 0xe0, 0x120);
        Deallocate(paletteData);
    }

    G2_SetBG0Priority(3);
    G2_SetBG2Priority(2);
    G2_SetBG3Priority(1);
    SetBg0Offset(0, 0);
    SetBg2Offset(0, 0);
    SetBg3Offset(0, 0);
    gMainScreenLayers = DISPLAY_LAYER_BG0 | DISPLAY_LAYER_OBJ;

    G2S_SetBG0Priority(1);
    G2S_SetBG2Priority(3);
    G2S_SetBG3Priority(3);
    SetSubBg0Offset(0, 0);
    SetSubBg2Offset(0, 0);
    SetSubBg3Offset(0, 0);
    gSubScreenLayers = 0x18;

    mFadeBrightness = 0;
    mFadeTick = 0;
    mIsFading = 0;

    if (data_0209b2e8 != 0) {
        Deallocate(data_0209b2e8);
        data_0209b2e8 = 0;
    }
    mDisconnectHideMask = 0;
    LoadFont3D();

    {
        int isEntry = (actorID == 6);
        if (isEntry) {
            InitEntryResources();
            if (param1 == 0) {
                if (func_0203da3c() == 0) {
                    data_0209b2e8 = LoadFile(0x8409);
                    UnloadArchive(0);
                    func_02030aa4(3);
                }
                {
                    int localPlayer = func_0203da9c();
                    func_02020028(localPlayer, data_0209f498[gActivePlayerSlot].mode);
                }
            }
        } else {
            InitResultResources();
        }
    }

    ENTRY_GRAPH_CALLBACK->Init(this, 1);
    gGraphCallback = ENTRY_GRAPH_CALLBACK;
    Sound::LoadInitialGroup(0x2b);

    {
        int isEntry = (actorID == 6);
        if (isEntry) {
            if (param1 < 2) {
                Sound::LoadAndSetMusic_Layer1(0x4c);
            }
        } else {
            if (data_0209b2f0[func_0203da9c()] != 0) {
                Sound::LoadAndSetMusic_Layer1(0x51);
            } else {
                Sound::LoadAndSetMusic_Layer1(0x52);
            }
        }
    }

    data_0208ee44 = 2;
    data_02092778 = 0;
    return 1;
}

/* Slot 1 forwards this and the boolean result through an interworking
 * tail call. ResetFadersAndSound returns only false or true on its two
 * exits. */
bool dScEntry_c::BeforeInitResources() {
    return ResetFadersAndSound();
}

void *dScEntry_c_classInit_RESULT(void) {
    return new dScEntry_c;
}

OamAnimation::OamAnimation() {}

dScEntry_c::icon_c::icon_c() {}
