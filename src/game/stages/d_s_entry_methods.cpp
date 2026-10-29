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
 * - The step helpers func_ov075_02119640..02119c38 are still extern
 *   "C" functions that take dScEntry_c *self. Turning them into
 *   members is not done yet.
 * - The transition helpers use native Step member-pointer copies.
 * - The destructor, the key function, is in d_s_entry.cpp. This file
 *   defines no virtual the class declares first, so it emits no
 *   dScEntry_c vtable or typeinfo.
 * - OamAnimation() and icon_c() are written out of line. The
 *   cartridge's copies close the run after the RESULT factory, the
 *   place the inline copies a factory forces would land. Inline
 *   copies are not emitted here: the factories name the two
 *   constructors by their linker names, so nothing in C++ uses them.
 * - The factory keeps its hand-built construction. A `new dScEntry_c'
 *   would need the class constructor, which is not reconstructed.
 *   Its __cxa_vec_ctor calls take the icon_c and OamAnimation
 *   structors by address, which C++ cannot spell.
 * - The header names the entry object and shared step workspace. The
 *   extern "C" step callbacks take the scene as a dScEntry_c pointer and
 *   reach it through those members. The transition helpers use the
 *   typed pending StepSet pointer at 0x6c.
 * - InitResources keeps its `int isResult = (actorID == 7)' flag locals.
 *   Folding the first one into its condition rewrites the function
 *   (999 words, measured).
 * - The helpers it calls stay func_ov075_* and the arm9 globals keep
 *   their linker names. Naming belongs with recovered members.
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
#include "PlayerInput.h"
#include "Sound.h"
#include "decl_common.h"
#include "dScEntry_c.h"
#include "dFdBrightness_c.h"
#include "AsyncJob.h"
#include "Heap.h"

/* The Nitro SDK's sub-engine palette loader. No header in the tree
 * declares the namespace yet; d_s_star_sel_methods.cpp spells it the
 * same way. */
namespace GXS {
void LoadBGPltt(const void *, unsigned int, unsigned int);
}

extern "C" {
void func_020308b4(void *self);
void func_020551f0(void *s, int v);
extern int data_0209fc68;

void Enable3dEngines(void);
void func_0200f2cc(void);
int func_0203d9b4(void);
void LoadTextNarcs(void);
int LoadArchive(int idx);
void func_0200f220(void);
int LoadFile(int handle);
void SetBg0Offset(int a, int b);
void SetBg2Offset(int a, int b);
void SetBg3Offset(int a, int b);
void SetSubBg0Offset(int a, int b);
void SetSubBg2Offset(int a, int b);
void SetSubBg3Offset(int a, int b);
void LoadFont3D(void);
int func_0203da3c(void);
void UnloadArchive(int i);
void func_02030aa4(int arg);
int func_0203da9c(void);
void func_02020028(int a, unsigned char b);
void func_ov075_0211601c(char *c, int a, int b);
}

extern void *data_0209d4a8;
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern u8 data_0209f4ae[];
extern s32 data_0208ee44;

/* The scene manager Behavior polls through its slot 5. */
extern "C" dFdBrightness_c *data_0209f5bc;

#include "dEntObj_c.h"
#include "OAM.h"
#include "OamAttr.h"
#include "G2x.h"
#include "dBase_c.h"
#include "G2.h"
#include "Message.h"

/* The RESULT factory retains its two-word aggregate-copy schedule. */
struct P2Copy {
    int words[2];
};

extern "C" {
int func_0203d974(void);
u8 func_02020168(void);
void func_02020304(void);
void func_020200e0(void);
void func_02020334(void);
void func_02020124(void);
int func_0203d890(void);
u32 LoadCompressedFileAt(int, void *);
int Sound_PlayIfNotActive(int, int, int, int);
void func_ov075_02116030(void *, int);
void func_ov075_02116028(int *, int);
void func_020338b0(int, int, short, int);
void func_02033464(int, int, int);
void func_ov075_02115e8c(char *, int, int, short, short);
void func_02030a38(void);
void func_02030a78(void);
void func_02030a6c(void);
void func_020337ec(void);
int TouchArea_Update(char *, int);
void func_ov001_020ab3f0(void *);

extern u8 data_0208f274;
extern OamAttr *data_0208a0f8[];
extern s16 data_02082214[];
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
}

/* Course-select and match-loading steps, directly below the player-select
 * run in ROM order. The declarations below are the union of the six shards'
 * extern blocks, minus names this file already declares. */
extern "C" {
int func_0203d8fc(void);
int func_0201a1bc(void);
void func_0203d930(void);
extern unsigned char EntryCourseLoadStarted;
void func_0202deac(int level);
void func_0203d918();
extern u8 data_0208a0e0;
u8 func_020202dc(void);
u8 func_020200b8(void);
void func_02020078(u32 flags);
void func_0202029c(u32 idx);
extern u16 data_020a0e5a[];
extern s8 data_ov075_0211c6ec[];
extern u8 data_0209f2d8;
extern u8 data_0209b2e4;
}

extern dScEntry_c::StepSet data_ov075_0211d798;
extern dScEntry_c::StepSet data_ov075_0211d7f8;
extern struct OamAttr data_ov075_0211cba4[];

/* Result tally, the two InitResources halves and the settings, quit,
 * rematch and results steps, in ROM order ahead of the course-select
 * steps. */
extern "C" {
extern s8 data_0209f310[];
extern s16 data_0209f358[];
extern u8 data_0209b2f0[];
extern u8 data_0209b2ec[];
extern u8 data_0209b2ed[];
extern u8 data_0209fc50;
extern u8 data_0209fc58;
extern char data_0209ee90[];
extern u8 data_0208ee3c;
extern u8 data_020a0e58[];
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
extern void *data_0209f5c0;
extern int OVERLAY_100_ID;
extern int OVERLAY_102_ID;

int func_0200f0bc(void);
void SetBg1Offset(int a, int b);
void func_ov075_02116128(char *c, int i, int z);
void func_ov075_0211621c(char *c);
void func_02033390(int a);
void func_02032f9c(int a);
int func_02030958(int val);
void func_0200f13c(void);
void DecompressLZ16(void *src, int dst);
int GetSoundMode(void);
void SetSoundMode(int mode);
void TurnBacklightOn(void);
void TurnBacklightOff(void);
void SetControllerMode(unsigned char mode);
void func_02033bb8(int a);
void UnloadOverlay(int id);
void func_02020190(void);
void func_02030994(void);
void func_020201c0(void);
void func_0202048c(void *dst, int a, int b);
void func_02020768(char *thiz);
void func_02020820(void *dst, void *src, int a, int b, int c, int d);
void MultiStore_Int(int val, void *dst, int len);
void func_020331fc(int a, int b, int c);
void PrepareVsMode(void);
void LoadLevelNoReturn(int levelID, u32 entranceID, u32 mode, u32 arg3);
extern u8 data_02092778;
u8 func_02020364(int player);
int func_020201f0(int);
}

namespace G2 {
void *GetBG1ScrPtr(void);
void *GetBG2CharPtr(void);
}

namespace GX {
void LoadBGPltt(const void *p, u32 a, u32 b);
void LoadOBJPltt(const void *p, u32 a, u32 b);
}

namespace GXS {
void LoadOBJPltt(const void *p, u32 a, u32 b);
}

extern dScEntry_c::StepSet data_ov075_0211d7e0;
extern dScEntry_c::StepSet data_ov075_0211d810;
extern dScEntry_c::StepSet data_ov075_0211d828;
extern dScEntry_c::StepSet data_ov075_0211d870;
extern dScEntry_c::StepSet data_ov075_0211d888;
extern dScEntry_c::StepSet data_ov075_0211d8e8;
extern short data_ov075_0211b5d4[];

// @symbol _ZN10dScEntry_c17TallyRoundWinnersEv
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

    if (data_0209f310[0] != 0 || data_0209f310[1] != 0 || data_0209f310[2] != 0 ||
        data_0209f310[3] != 0 || data_0209f358[0] != 0 || data_0209f358[1] != 0 ||
        data_0209f358[2] != 0 || data_0209f358[3] != 0) {
        max = 0;
        i = max;
        pc = (u8 *)data_0209f310;
        do {
            t = (s8)*pc;
            i = i + 1;
            pc = pc + 1;
            if (max < t) {
                max = t & 0xff;
            }
        } while (i < 4);

        n = 0;
        pb = (u8 *)data_0209f310;
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
                    if (*pc != 0 && w < data_0209f358[i]) {
                        w = data_0209f358[i] & 0xff;
                    }
                    i = i + 1;
                    pc = pc + 1;
                } while (i < 4);

                pc = data_0209b2f0;
                i = 0;
                do {
                    if (*pc != 0 && data_0209f358[i] < w) {
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

// @symbol _ZN10dScEntry_c19InitResultResourcesEv
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
    char *sub;
    int n;

    *(volatile u16 *)0x400000a = (u16)((*(volatile u16 *)0x400000a & 0x43) | 0x208);
    *(volatile u16 *)0x400000a = (u16)(*(volatile u16 *)0x400000a & ~0x40);

    LoadCompressedFileAt(0x8413, G2::GetBG2CharPtr());
    tmp = (void *)LoadFile(0x8414);
    GX::LoadBGPltt(tmp, 0, 0x1c0);
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
    LoadCompressedFileAt(id0, G2::GetBG1ScrPtr());
    LoadCompressedFileAt(id1, (void *)G2::GetBG2ScrPtr());

    i = 0;
    if (n > 0) {
        z = i;
        do {
            func_ov075_02116128((char *)this, i, z);
            i = i + 1;
        } while (i < n);
    }

    LoadCompressedFileAt(data_ov075_0211c990[func_0200f0bc()], (void *)0x6400000);
    tmp = (void *)LoadFile(0x8415);
    GX::LoadOBJPltt(tmp, 0, 0x140);
    Deallocate(tmp);

    *(volatile u16 *)0x400000a = (u16)((*(volatile u16 *)0x400000a & ~3) | 2);
    SetBg1Offset(0, 0);
    sub = (char *)0x6600000;
    LoadCompressedFileAt(0x8402, sub);
    LoadCompressedFileAt(data_ov075_0211c97c[func_0200f0bc()], sub + 0x6000);
    tmp = (void *)LoadFile(0x8403);
    GXS::LoadOBJPltt(tmp, 0, 0x200);
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
            unsigned char ch = (unsigned char)func_02030958(k);
            func_02033464((unsigned short)(base + acc), (signed char)ch, 1);
            k = k + 1;
            acc = acc + step;
        } while (k < n);
    }

    SetSteps(&data_ov075_0211d7e0);
    mEntObj = (dEntObj_c *)dBase_c::Spawn(0x15a, (fBase_c *)data_0209f5c0, 1, 0);
    return 1;
}

/* Runtime asset handles resolved through the overlay 0 file table. */
enum EntryAssetId { ASSET_D_2D_BATTLE_FRAMEWORK_GAMESTART_P_NSC_BIN = 0x025e };

// @symbol _ZN10dScEntry_c18InitEntryResourcesEv
s32 dScEntry_c::InitEntryResources() {
    void *tmp;
    int mode;

    if (param1 != 0 || func_0203d9b4() != 0) {
        func_0200f13c();
        if (func_0203d9b4() != 0) {
            LoadCompressedFileAt(ASSET_D_2D_BATTLE_FRAMEWORK_GAMESTART_P_NSC_BIN,
                                 G2S::GetBG2ScrPtr());
        }
        DecompressLZ16(data_0208a0e4[func_0200f0bc()], 0x6600000);
    }
    LoadCompressedFileAt(data_ov075_0211c9a4[func_0200f0bc()], (void *)0x6400000);

    tmp = (void *)LoadFile(0x8412);
    GX::LoadOBJPltt(tmp, 0, 0xc0);
    Deallocate(tmp);

    tmp = (void *)LoadFile(0x980b);
    GX::LoadBGPltt(tmp, 0, 0xc0);
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

    mEntObj = (dEntObj_c *)dBase_c::Spawn(0x15a, (fBase_c *)data_0209f5c0, mode, 0);
    return 1;
}

// @symbol _ZN10dScEntry_c13LeaveSettingsEv
void dScEntry_c::LeaveSettings() {
    if (data_0209fc68) {
        ChangeStepsAfterDelay(&data_ov075_0211d828, 0x14);
    } else {
        ChangeStepsAfterDelay(&EntryCourseSelectSteps, 0x14);
    }
}

// @symbol _ZN10dScEntry_c10StartMatchEv
void dScEntry_c::StartMatch() {
    PrepareVsMode();
    u8 course = data_0209b2e4;
    LoadLevelNoReturn(data_ov075_0211c6ec[course], 0, 2, 0);
    Sound::StopLoadedMusic_Layer1(0x3c);
    data_02092778 = 1;
}

// @symbol _ZN10dScEntry_c19func_ov075_02116ce4Ev
int dScEntry_c::func_ov075_02116ce4() {
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

// @symbol _ZN10dScEntry_c15ArePlayersReadyEv
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

// @symbol _ZN10dScEntry_c16CheckMenuConfirmEi
int dScEntry_c::CheckMenuConfirm(int player) {
    u16 pressed;
    if (player >= 0)
        pressed = data_020a0e5a[player * 2];
    else
        pressed = data_020a0e5a[func_0203da9c() * 2];
    if (pressed != 0) {
        if (pressed & 0xc)
            return 1;
        func_02012790(0xe);
    }
    return 0;
}

// @symbol _ZN10dScEntry_c17ResetMatchRecordsEv
void dScEntry_c::ResetMatchRecords() {
    int i;
    for (i = 0; i < 4; i++) {
        data_0209b2f0[i] = 0;
        data_0209b2ec[i] = 0;
    }
    data_0209b2e4 = 0;
}

// @symbol _ZN10dScEntry_c18UpdateSettingsSyncEv
void dScEntry_c::UpdateSettingsSync() {
    if (func_02020168()) {
        func_02020304();
        func_020200e0();
        *(short *)0x4000050 = 0;
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

// @symbol _ZN10dScEntry_c17EnterSettingsSyncEv
void dScEntry_c::EnterSettingsSync() {
    data_0209d45c &= ~4;
    data_0209d454 &= ~1;
    func_ov075_02116028((int *)&data_ov075_0211d71c, 6);
    func_020338b0(0x145, 0xb0, 0x1d, -1);
    data_0209d454 |= 5;
    mIconCount = 0;
    func_02020334();
    func_02012790(0x120);
}

// @symbol _ZN10dScEntry_c19RenderSoundSettingsEv
void dScEntry_c::RenderSoundSettings() {
    unsigned char idx = mSelectedIcon;
    int a1, a2;
    if (idx < 2) {
        a2 = (short)(idx * 0x30 + 0x38);
        a1 = 0xe;
    } else {
        int j = func_0200f0bc();
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

#pragma push
#pragma opt_strength_reduction off
// @symbol _ZN10dScEntry_c19UpdateSoundSettingsEv
void dScEntry_c::UpdateSoundSettings() {
    int w = func_0203da9c();
    int dir;
    u16 flags;

    if (TouchArea_Update((char *)&mIcons[5], -1) != 0 ||
        TouchArea_Update((char *)&mIcons[8], -1) != 0 ||
        ((flags = data_020a0e5a[w << 1]) & 8) != 0 || (flags & 4) != 0 ||
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
                Message::DisplayOptionsMenuTextVS((s16)(GetSoundMode() + 9));
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
       their index is mSelectedIcon and palette 3 otherwise. dir is reused
       as the strip's tile offset. */
    {
        int i;
        int zero;
        u16 palOff;
        u16 palOn;
        u16 mask;
        i = 0;
        dir = i;
        zero = i;
        palOff = 0x3000;
        palOn = 0x4000;
        mask = 0xfff;
        do {
            u16 *p = static_cast<u16 *>(G2S::GetBG2ScrPtr()) + 6 * 32 + 2;
            p += dir;
            u16 pal = (i == mSelectedIcon) ? palOn : palOff;
            int j = zero;
            do {
                unsigned v = *p;
                j++;
                v = v & mask;
                *p = v | pal;
                v = p[32];
                v = v & mask;
                p[32] = v | pal;
                p++;
            } while (j < 11);
            i++;
            dir += 6 * 32;
        } while (i < 2);
    }
}
#pragma pop

#define SH(base, i) (*(short *)((char *)(base) + (i) * 4))

// @symbol _ZN10dScEntry_c18EnterSoundSettingsEv
void dScEntry_c::EnterSoundSettings() {
    func_ov075_02116030(&data_ov075_0211d71c, LoadFile(data_ov075_0211ca08[func_0200f0bc()]));

    Message::DisplayOptionsMenuTextVS((s16)(GetSoundMode() + 9));

    data_0209d454 |= 5;
    G2x::SetBlendBrightness((volatile unsigned short *)0x4000050, 0xb, ~5);

    mSelectedIcon = 0;

    func_ov075_02115e8c((char *)&mIcons[6], 5, 0,
                        (s16)(SH(data_ov075_0211d72c, func_0200f0bc()) + 8), 0x38);
    func_ov075_02115e8c((char *)&mIcons[7], 6, 0,
                        (s16)(SH(data_ov075_0211d740, func_0200f0bc()) + 8), 0x68);
    func_ov075_02115e8c((char *)&mIcons[8], 0xc, 0, 0x56, 0xa8);
    func_ov075_02115e8c((char *)&mIcons[2], 1, 0, 0xac, 0x38);
    func_ov075_02115e8c((char *)&mIcons[0], 3, 0, 0x64, 0x38);
    func_ov075_02115e8c((char *)&mIcons[1], 4, 0, 0xf4, 0x38);

    mIcons[2].mOn = 1;

    func_ov075_02115e8c((char *)&mIcons[3], 2, 0, 0x8c, 0x68);
    func_ov075_02115e8c((char *)&mIcons[4], 2, 0, 0xcc, 0x68);

    if (data_0208ee3c != 0)
        mIcons[3].mOn = 1;
    else
        mIcons[4].mOn = 1;

    func_ov075_02115e8c((char *)&mIcons[5], 0xb, 0, 0x80, 0xa8);

    mIconCount = 9;

    func_02020304();
}

#undef SH

// @symbol _ZN10dScEntry_c21RenderControlSettingsEv
void dScEntry_c::RenderControlSettings() {
    int *tbl = data_ov075_0211c9cc[func_0200f0bc()];
    OamAttr *attr = (OamAttr *)tbl[mStepWork[0]];
    OAM::Render(false, attr, 0x80, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);

    int a1, a2;
    unsigned char idx = mSelectedIcon;
    if (idx < 3) {
        a2 = (short)(idx * 0x28 + 0x30);
        a1 = 0x26;
    } else {
        int j = func_0200f0bc();
        a1 = data_ov075_0211b5d4[j];
        a2 = 0xa8;
    }
    OAM::RenderSub((OamAttr *)data_ov075_0211c790, a1, a2);
}

// @symbol _ZN10dScEntry_c21UpdateControlSettingsEv
void dScEntry_c::UpdateControlSettings() {
    int w = func_0203da9c();

    if (TouchArea_Update((char *)&mIcons[3], -1) != 0 ||
        TouchArea_Update((char *)&mIcons[4], -1) != 0 || (data_020a0e5a[w << 1] & 8) != 0 ||
        (data_020a0e5a[w << 1] & 4) != 0 || (CheckMenuConfirm(-1) != 0 && mSelectedIcon == 3)) {
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

// @symbol _ZN10dScEntry_c20EnterControlSettingsEv
void dScEntry_c::EnterControlSettings() {
    int i;
    int r;
    int v;
    char *p;

    i = 0;
    p = (char *)mIcons;
    v = 0x30;
    mSelectedIcon = data_0209f4ae[gActivePlayerSlot * 0x18];

    for (; i <= 2; i++) {
        func_ov075_02115e8c(p, 0, 0, 0x80, (s16)v);
        v += 0x28;
        p += 0x24;
    }

    mIcons[mSelectedIcon].mOn = 1;

    func_ov075_02115e8c((char *)&mIcons[3], 0xb, 0, 0x80, 0xa8);

    r = func_0200f0bc();
    func_ov075_02115e8c((char *)&mIcons[4], 0xc, 0, data_ov075_0211b5d4[r], 0xa8);

    mIconCount = 5;
    data_0209d454 &= ~5;

    r = func_0200f0bc();
    func_ov075_02116030(&data_ov075_0211d71c, LoadFile(data_ov075_0211c9b8[r]));

    if (mSelectedIcon == 0)
        SetBg2Offset(0, 0);
    else if (mSelectedIcon == 1)
        SetBg2Offset(0x100, 0);
    else
        SetBg2Offset(0, 0x100);

    func_02033bb8(5);
    data_0209d45c |= 4;
    data_0209d454 |= 5;
    G2x::SetBlendBrightness((volatile unsigned short *)0x4000050, 0xb, ~5);

    mStepWork[0] = mSelectedIcon;
    func_02020304();
}

// @symbol _ZN10dScEntry_c16EnterQuitMessageEv
void dScEntry_c::EnterQuitMessage() {
    data_0209d45c = data_0209d45c & (~0xe);
    data_0209d454 = data_0209d454 & (~1);
    func_ov075_02116028((int *)&data_ov075_0211d71c, 6);
    func_020338b0(0x145, 0xb0, 0x17, -1);
    data_0209d454 = data_0209d454 | 5;
    mIconCount = 0;
    G2x::SetBlendBrightness((volatile unsigned short *)0x4000050, 0x1f, ~5);
    mStepDelay = 2;
}

// @symbol _ZN10dScEntry_c10UpdateQuitEv
void dScEntry_c::UpdateQuit() {
    mStepDelay -= data_0208ee44;
    if (mStepDelay > 0)
        return;
    dScene_c::StartSceneFade(1, 0, 0);
    Sound::StopLoadedMusic_Layer1(0x3c);
    data_02092778 = 1;
}

// @symbol _ZN10dScEntry_c9EnterQuitEv
void dScEntry_c::EnterQuit() {
    data_0209d45c &= ~0xe;
    mStepDelay = 0x14;
    UnloadOverlay((int)&OVERLAY_100_ID);
    UnloadOverlay((int)&OVERLAY_102_ID);
}

static inline unsigned IsMode6(dScEntry_c *scene) {
    /* materialized: returning the comparison directly shrinks the caller (0x15c) */
    unsigned dok = (scene->actorID == 6);
    return dok;
}

// @symbol _ZN10dScEntry_c17UpdateQuitConfirmEv
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

// @symbol _ZN10dScEntry_c16EnterQuitConfirmEv
void dScEntry_c::EnterQuitConfirm() {
    int fh;

    data_0209d454 = data_0209d454 & ~1;
    if (IsEntryScene(this)) {
        Message::DisplayDontSaveTextVS(0x19);
    } else {
        data_0209d454 = data_0209d454 & ~4;
        fh = LoadFile(0x9802);
        func_ov075_02116030(&data_ov075_0211d71c, fh);
        Message::DisplayDontSaveTextVS(0x16);
        data_0209d454 = data_0209d454 | 4;
        mRenderStep = data_ov075_0211d810.render;
    }
    data_0209d454 = data_0209d454 | 1;
    mIconCount = 2;
    mSelectedIcon = 1;
    func_ov075_02115e8c((char *)&mIcons[0], 8, 0, 0x50, 0xa8);
    func_ov075_02115e8c((char *)&mIcons[1], 8, 0, 0xb0, 0xa8);
}

// @symbol _ZN10dScEntry_c17UpdateRematchSyncEv
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
        data_0209d45c = data_0209d45c & ~0xe;
        mDisconnectHideMask = 0;
        mRenderStep = 0;
        dScene_c::StartSceneFade(6, 2, 0);
        data_02092778 = 1;
        return;
    }
    if (func_0203d9b4() == 0)
        return;
    if (func_ov075_02116ce4() == 0)
        return;
    func_02020124();
}

// @symbol _ZN10dScEntry_c16EnterRematchSyncEv
void dScEntry_c::EnterRematchSync() {
    data_0209d454 &= ~1;
    func_ov075_02116028((int *)&data_ov075_0211d71c, 6);
    func_020338b0(0x145, 0xb0, 0x1d, -1);
    data_0209d454 |= 5;
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
 * func_ov075_02116128 repaint each round winner's slot border bright or
 * dim by it. mStepWork[0] counts frames and mStepWork[1] is the flag. */
// @symbol _ZN10dScEntry_c15RenderWinCountsEv
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
                    int *q = (int *)(int)(p + 1);
                    int val = *q;
                    *q = (val & ~0x3ff) | ((tens + ((u32)(val << 22) >> 22)) & 0x3ff);
                }
            }
            p = (int *)OAM::Render(zO, (OamAttr *)data_ov075_0211c8b0, xOnes, yBase + yOff, eight,
                                   minus1, sz, zO);
            if (p != 0) {
                int *q = (int *)(int)(p + 1);
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
                func_ov075_02116128((char *)this, j, mStepWork[1]);
            j++;
        } while (j < count);
    }
}

// @symbol _ZN10dScEntry_c19UpdateRematchPromptEv
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

/* Sets data_0209d45c bits 0xe and records them in mDisconnectHideMask; on
 * a player disconnect HandlePlayerDisconnect clears data_0209d45c by
 * ~mDisconnectHideMask, and UpdateRematchSync clears both on its own. */
// @symbol _ZN10dScEntry_c18EnterRematchPromptEv
void dScEntry_c::EnterRematchPrompt() {
    data_0209d454 &= ~5;
    func_ov075_02116028((int *)&data_ov075_0211d71c, 1);
    func_ov075_02116030(&data_ov075_0211d71c, LoadFile(0x9801));
    LoadCompressedFileAt(0x8407, G2S::GetBG3ScrPtr());
    Message::DisplayVsExitText(0x1f);

    data_0209d45c |= 0xe;
    data_0209d454 |= 5;
    mSelectedIcon = 0;
    func_ov075_02115e8c((char *)&mIcons[0], 0xa, 0, 0x80, 0x48);
    func_ov075_02115e8c((char *)&mIcons[1], 0xa, 0, 0x80, 0x78);
    mIconCount = 2;
    G2x::SetBlendAlpha((volatile unsigned short *)0x4000050, 4, 0x11, 0xc, 4);
    mDisconnectHideMask = 0xe;
    if (data_0209fc68 != 0)
        data_0209fc68 = 5;
    func_02020190();
    mStepWork[0] = 0;
    mStepWork[1] = 0;
    func_02012790(0x125);
    Sound::LoadAndSetMusic_Layer1(0x4c);
}

// @symbol _ZN10dScEntry_c13RenderResultsEv
void dScEntry_c::RenderResults() {
    int id = func_0203da9c();
    OAM::Render(false, &data_ov075_0211cb84, 0x80, 0x20, -1, -1, 0x1000, 0x1000, 0, -1);
    func_ov075_0211621c((char *)this);

    u8 *p = data_0209b2f0 + id;
    OamAttr *sb;
    if (*p != 0)
        sb = data_ov075_0211c9f4[func_0200f0bc()];
    else
        sb = data_ov075_0211c9e0[func_0200f0bc()];

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

#pragma push
// 6e: keep pointer-induction un-reduced
#pragma opt_strength_reduction off
// 6e: EBB-local CSE
#pragma opt_common_subs off
// @symbol _ZN10dScEntry_c13UpdateResultsEv
void dScEntry_c::UpdateResults() {
    int id;
    int acc;
    int i;
    int zero;
    int one;
    int bit;
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
                p = data_0209fc5c;
                i = acc;
                zero = acc;
                one = 1;
                do {
                    bit = *p;
                    if (bit != 0) {
                        acc |= *(u16 *)(data_020a0e58 + (i << 2) + 2) & 0xc;
                        if (gTouchHeld[i << 2] != 0 && (gTouchHeld + (i << 2))[1] != 0)
                            bit = one;
                        else
                            bit = zero;
                        acc |= bit;
                    }
                    i = i + 1;
                    p = p + 1;
                } while (i < 4);
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
        if ((data_020a0e5a[func_0203da9c() * 2] & ~0xc) != 0)
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
#pragma pop

// @symbol _ZN10dScEntry_c12EnterResultsEv
void dScEntry_c::EnterResults() {
    int id;

    id = func_0203da9c();
    mStepDelay = 0x258;
    mStepWork[0] = 0;
    mStepWork[1] = 0;

    if (data_0209b2f0[id] != 0) {
        LoadCompressedFileAt(0x8404, G2S::GetBG3ScrPtr());
        func_02020820((char *)&mOamAnimations[0], &data_ov075_0211ca64, 1, 1, 1, 1);
        func_02020820((char *)&mOamAnimations[1], &data_ov075_0211ca6c, 1, 0, 0, 0);
        func_02020820((char *)&mOamAnimations[2], &data_ov075_0211ca74, 1, 1, 1, 1);
    } else {
        LoadCompressedFileAt(0x8401, G2S::GetBG3ScrPtr());
        func_02020820((char *)&mOamAnimations[0], &data_ov075_0211cc44, 1, 1, 1, 1);
        func_02020820((char *)&mOamAnimations[1], &data_ov075_0211cc4c, 1, 0, 0, 0);
        func_02020820((char *)&mOamAnimations[2], &data_ov075_0211cc54, 1, 0, 0, 0);
        func_02020820((char *)&mOamAnimations[3], &data_ov075_0211cc5c, 1, 0, 0, 0);
    }

    mOamAnimations[0].palette = id + 0xa;
    mOamAnimations[1].palette = id + 0xa;

    {
        void *bg0 = G2S::GetBG0ScrPtr();
        volatile int zero = 0;
        MultiStore_Int(zero, bg0, 0x800);
    }

    func_02033390(0);

    func_020331fc(0x40, 0x20, func_0203da9c() << 24 >> 24);

    data_0209d454 |= 1;

    func_02020304();

    mStepWork[3] = 0;
}

/* The behavior of the step set at data_ov075_0211d7c8, which the static
 * initializer fills and no code installs. A tail call that hands the
 * scene pointer through in r0. */
// @symbol _ZN10dScEntry_c19func_ov075_02118a78Ev
void dScEntry_c::func_ov075_02118a78() {
    func_020308b4(this);
}

// local extern: the Fix12-by-value overload retains its measured scalar ABI bridge.
extern "C" void OamRender10_d_s_entry_methods(int sub, struct OamAttr *data, s32 x, s32 y,
                                              s32 palette, s32 priority, s32 scaleX, s32 scaleY,
                                              s32 rotation, s32 mode);

// @symbol _ZN10dScEntry_c18UpdateMatchLoadingEv
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

// @symbol _ZN10dScEntry_c19LoadCourseResourcesEi
void dScEntry_c::LoadCourseResources(int course) {
    mHeap::Heap_t *previousHeap = static_cast<mHeap::Heap_t *>(data_020a0eac)->SetDefault();
    LoadArchive(7);
    int selectedArchive = course + 2;
    unsigned int archive;
    for (archive = 2; archive <= 5; ++archive) {
        if (archive == (unsigned int)selectedArchive)
            continue;
        UnloadArchive(archive);
    }
    LoadArchive(selectedArchive);
    previousHeap->SetDefault();
    data_0209f2d8 = 1;
    func_0202deac(data_ov075_0211c6ec[course]);
}

// @symbol _ZN10dScEntry_c17EnterMatchLoadingEv
void dScEntry_c::EnterMatchLoading() {
    mEntObj->func_ov075_02114fa8();
    mStepDelay = 240;
    mStepPhase = MATCH_LOAD_RESOURCES;
    mSawLoadSignal = 0;
    if (data_0209fc68 != 0)
        data_0208a0e0 = 4;
    func_0203d918();
}

// @symbol _ZN10dScEntry_c23RenderCourseSelectTitleEv
void dScEntry_c::RenderCourseSelectTitle() {
    OAM::Render(0, data_ov075_0211cba4, 0x80, 0x20, -1, -1, 0x1000, 0x1000, 0, -1);
}

// @symbol _ZN10dScEntry_c18UpdateCourseSelectEv
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
                if (data_020a0e5a[gActivePlayerSlot * 2] & 4) {
                    func_02020078(1);
                    func_02020124();
                } else if (data_020a0e5a[gActivePlayerSlot * 2] & 8) {
                    func_02020078(2);
                    func_02020124();
                }
            }
            func_0202029c(data_0209b2e4);
        }
    }
    *(u16 *)0x4000050 = 0;
    if (data_020a0e5a[func_0203da9c() * 2] & ~0xc) {
        func_02012790(0xe);
    }
}

// @symbol _ZN10dScEntry_c17EnterCourseSelectEv
void dScEntry_c::EnterCourseSelect() {
    int index;
    int x;
    icon_c *icon;

    data_0209d45c &= ~4;
    data_0209d454 &= ~5;
    func_ov075_02116028((int *)&data_ov075_0211d71c, 1);
    func_ov075_02116030((void *)&data_ov075_0211d71c, LoadFile(0x8409));
    LoadCompressedFileAt(0x8407, G2S::GetBG3ScrPtr());

    if (func_0203d9b4() != 0)
        func_020338b0(0x205, 0xb0, 0x21, -1);
    else
        func_020338b0(0x225, 0xb0, 0x1d, -1);

    data_0209d454 |= 5;

    icon = mIcons;
    index = 0;
    x = 0x20;
    for (; index < 4; index++) {
        func_ov075_02115e8c((char *)icon, 7, index, (short)x, 0x48);
        mIcons[index].mOn = 1;
        mIcons[index].mPressTime = 8;
        x += 0x40;
        ++icon;
    }

    data_0209b2e4 = 4;
    func_ov075_02115e8c((char *)&mIcons[4], 0xd, 4, 0x80, -0x14);
    mIconCount = 5;
    func_02020334();
    func_020200e0();
}

// @symbol _ZN10dScEntry_c23UpdatePlayerSelectIntroEv
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

// @symbol _ZN10dScEntry_c22EnterPlayerSelectIntroEv
void dScEntry_c::EnterPlayerSelectIntro() {
    int row;
    int col;
    u16 *tiles;
    u32 palette;
    data_0209d454 &= ~5;
    if (func_0200f0bc() == 0)
        LoadCompressedFileAt(0x840a, G2S::GetBG2ScrPtr());
    else
        LoadCompressedFileAt(0x980c, G2S::GetBG2ScrPtr());
    LoadCompressedFileAt(0x840b, G2S::GetBG3ScrPtr());

    // Color the 9-by-10 tile panel for this player's wireless station.
    tiles = static_cast<u16 *>(G2S::GetBG2ScrPtr()) + 11 * 32 + 11;
    palette = (static_cast<u32>(func_0203da9c() + 10) << 28) >> 16;
    for (row = 0; row < 10; ++row) {
        for (col = 0; col < 9; ++col)
            tiles[col] = palette | (tiles[col] & 0xfff);
        tiles += 32;
    }
    data_0209d454 |= 4;
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

// @symbol _ZN10dScEntry_c16UpdatePlayerSyncEv
void dScEntry_c::UpdatePlayerSync() {
    mSoundHandle = Sound_PlayIfNotActive(mSoundHandle, 2, 0x11d, 0);

    switch (mStepPhase) {
    case 0: {
        u8 activePlayers = data_0209fc50;
        int sessionPlayers = func_0203d890();
        if (activePlayers == sessionPlayers) {
            func_020308b4((void *)sessionPlayers);
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
            func_020308b4((void *)hostReady);
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

// @symbol _ZN10dScEntry_c15EnterPlayerSyncEv
void dScEntry_c::EnterPlayerSync() {
    // Download-play clients and non-host stations install the waiting screen.
    if (func_0203d974()) {
        data_0209d454 &= ~5;
        int screen = LoadFile(0x8409);
        func_ov075_02116030((void *)&data_ov075_0211d71c, screen);
        func_020338b0(0x225, 0xb0, 0x1d, -1);
        data_0209d454 |= 5;
        mIconCount = 0;
        mWaiting.wavePhase = 0;
        mWaiting.waveDelay = 120;
    }
    mStepPhase = 0;
    mWaiting.playerWaitTicks = 0;
    mWaiting.syncTicks = 0;
}

// @symbol _ZN10dScEntry_c17RenderWaitingTextEv
void dScEntry_c::RenderWaitingText() {
    OamAttr *glyph = data_0208a0f8[func_0200f0bc()];

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
                y = (data_02082214[2 * ((u16)(s16)(u16)phase >> 4)] >> 10) + 96;
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

// @symbol func_ov075_02119640
extern "C" void func_ov075_02119640(dScEntry_c *self) {
    self->mSoundHandle = Sound_PlayIfNotActive(self->mSoundHandle, 2, 0x11d, 0);
    if (self->mStepPhase == 0) {
        self->mEntObj->func_ov075_02115134();
        self->mStepPhase = (unsigned char)self->mEntObj->func_ov075_02115098(func_0203da9c());
    }
    if (func_0203da3c() != 0)
        return;
    LoadTextNarcs();
    LoadArchive(0);
    LoadArchive(1);
    func_02030aa4(1);
    self->SetSteps(&data_ov075_0211d918);
}

// @symbol func_ov075_021196d0
extern "C" void func_ov075_021196d0(dScEntry_c *self) {
    self->mEntObj->func_ov075_02115134();
    data_0209d454 &= ~5;
    func_ov075_02116030((void *)&data_ov075_0211d71c, (int)data_0209b2e8);
    data_0209b2e8 = 0;
    func_020338b0(0x225, 0xb0, 0x1d, -1);
    data_0209d454 |= 5;
    self->mIconCount = 0;
    self->mWaiting.wavePhase = 0;
    self->mWaiting.waveDelay = 0x78;
    func_02012790(0x11e);
    self->mStepPhase = self->mEntObj->func_ov075_02115098(func_0203da9c());
}

// @symbol func_ov075_0211977c
extern "C" void func_ov075_0211977c(dScEntry_c *self) {
    int r = func_0203da3c();
    if (r != 0)
        return;
    Deallocate(data_0209b2e8);
    data_0209b2e8 = 0;
    LoadTextNarcs();
    LoadArchive(0);
    LoadArchive(1);
    self->SetSteps(&EntryCourseSelectSteps);
}

// @symbol func_ov075_021197dc
extern "C" void func_ov075_021197dc(dScEntry_c *self) {
    self->mIconCount = 0;
}

// @symbol func_ov075_021197e8
extern "C" void func_ov075_021197e8(dScEntry_c *self) {
    if (self->mStepPhase == 0)
        self->mStepPhase = self->mEntObj->func_ov075_02115098(func_0203da9c()) != 0;
    if (self->mStepPhase == 0)
        return;
    if (TouchArea_Update((char *)&self->mIcons[0], -1) != 0 ||
        (self->CheckMenuConfirm(-1) != 0 && self->mSelectedIcon == 0)) {
        self->mIcons[1].mOn = 0;
        self->mIcons[0].Select();
        SetNumPlayers(1);
        self->ChangeStepsAfterDelay(&data_ov075_0211d900, 0x14);
        func_02012790(0x118);
        return;
    }
    if (TouchArea_Update((char *)&self->mIcons[1], -1) == 0) {
        if (self->CheckMenuConfirm(-1) == 0)
            return;
        if (self->mSelectedIcon != 1)
            return;
    }
    self->mIcons[0].mOn = 0;
    self->mIcons[1].Select();
    self->ChangeStepsAfterDelay(&data_ov075_0211d8b8, 0x14);
    self->mEntObj->func_ov075_0211505c();
    func_02012790(0x119);
}

// @symbol func_ov075_02119918
extern "C" void func_ov075_02119918(dScEntry_c *self) {
    data_0209d454 = data_0209d454 & ~1;
    func_ov075_02116028((int *)&data_ov075_0211d71c, 2);
    Message::DisplayDontSaveTextVS(0x11);

    data_0209d454 = data_0209d454 | 5;
    self->mSelectedIcon = 1;
    func_ov075_02115e8c((char *)&self->mIcons[0], 8, 0, 0x50, 0xa8);
    func_ov075_02115e8c((char *)&self->mIcons[1], 8, 0, 0xb0, 0xa8);

    self->mIconCount = 2;
    func_02030a38();
    {
        int r = func_0203da9c();
        self->mStepPhase = self->mEntObj->func_ov075_02115098(r) != 0;
    }
}

struct SinePair {
    short a, b;
};

// @symbol func_ov075_021199d8
extern "C" void func_ov075_021199d8(dScEntry_c *self) {
    OamAttr *attr;

    attr = data_ov075_0211c968[func_0200f0bc()];

    if (self->mWaiting.waveDelay <= 0) {
        int ang;
        int flag;
        int fallback = 0x1c;
        int zero = 0;
        int one = 1;
        int minus = -1;
        int fx = 0x1000;
        int x = 0x80;

        ang = self->mWaiting.wavePhase;
        flag = 0;

        do {
            int y;

            if (ang >= 0 && ang < 0x10000) {
                flag = one;
                y = (((SinePair *)data_02082214)[(unsigned short)(short)(unsigned short)ang >> 4]
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

        self->mWaiting.wavePhase += 0x800;

        if (flag == zero)
            self->mWaiting.waveDelay = 0x78;
    } else {
        OAM::RenderSub(attr, 0x80, 0x1c);
        self->mWaiting.waveDelay -= data_0208ee44;
        self->mWaiting.wavePhase = 0;
    }
}

// @symbol func_ov075_02119b34
extern "C" void func_ov075_02119b34(dScEntry_c *self) {
    int r5 = func_0203da9c();
    int r4 = data_0209fc50;
    for (int i = 1; i < 4; i++) {
        func_02033464((unsigned char)i, (signed char)i, 0);
    }
    self->mEntObj->func_ov075_02115134();
    self->mSoundHandle = Sound_PlayIfNotActive(self->mSoundHandle, 2, 0x11c, 0);
    if (data_0209fc5c[r5] == 0)
        return;
    if (r4 < 4) {
        if (TouchArea_Update((char *)&self->mIcons[0], 0) == 0) {
            if (self->CheckMenuConfirm(0) == 0)
                return;
        }
    }
    func_ov001_020ab3f0(&self->mIcons[0]);
    if (r4 <= 1) {
        self->ChangeStepsAfterDelay(&data_ov075_0211d8a0, 0x14);
    } else {
        func_02030a6c();
        self->ChangeStepsAfterDelay(&data_ov075_0211d8d0, 0x14);
    }
    func_02012790(0x117);
}

// @symbol func_ov075_02119c38
extern "C" void func_ov075_02119c38(dScEntry_c *self) {
    data_0209d454 &= ~1;
    func_ov075_02116028((int *)&data_ov075_0211d71c, 1);
    func_020337ec();
    data_0209d454 |= 5;
    func_ov075_02115e8c((char *)&self->mIcons[0], 0xa, 0, 0x80, 0xa8);
    self->mIconCount = 1;
    self->mStepPhase = 0;
    self->mWaiting.wavePhase = 0;
    self->mWaiting.waveDelay = 0x78;
}

// @symbol _ZN10dScEntry_c17UpdateStepFadeOutEv
void dScEntry_c::UpdateStepFadeOut() {
    ++mFadeTick;
    if (mFadeTick & 1)
        return;
    ++mFadeBrightness;
    func_020551f0((void *)0x4001050, mFadeBrightness);
    if (mFadeBrightness < 16)
        return;
    mIsFading = 1;
    SetSteps(mPendingSteps);
}

// @symbol _ZN10dScEntry_c16EnterStepFadeOutEv
void dScEntry_c::EnterStepFadeOut() {
    *(volatile u16 *)0x4000050 = 0;
    G2x::SetBlendBrightness((volatile u16 *)0x4001050, 0x1d, 0);
    mFadeBrightness = 0;
    mFadeTick = 0;
}

// @symbol _ZN10dScEntry_c15UpdateStepDelayEv
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

// @symbol _ZN10dScEntry_c22HandlePlayerDisconnectEPKNS_7StepSetE
void dScEntry_c::HandlePlayerDisconnect(const StepSet *resumeSteps) {
    u8 activePeerMask;
    u8 *peerActive;
    int player;
    u8 disconnectFlags;

    data_0209d454 = data_0209d454 & ~0x11;
    activePeerMask = 0;
    peerActive = data_0209fc5d;
    for (player = 1; player < 4; player++) {
        if (peerActive[0])
            activePeerMask = activePeerMask | (1 << player);
        peerActive++;
    }
    disconnectFlags = data_0209fc58;
    if (HasConnectionError()) {
        func_ov075_02116028((int *)&data_ov075_0211d71c, 5);
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
        data_0209d45c = data_0209d45c & ~mDisconnectHideMask;
        if (mDisconnectHideMask)
            mRenderStep = 0;
        G2x::SetBlendBrightness((volatile u16 *)0x4000050, 0x1f, -6);
        *(volatile u16 *)0x4001050 = 0;
        Sound::StopLoadedMusic_Layer1(4);
        func_02012790(0x123);
        mEntObj->mSuspended = 1;
    } else {
        func_ov075_02116028((int *)&data_ov075_0211d71c, 6);
        if ((func_0203d974() && data_0209fc5c[0] == 0) ||
            (func_0203d9b4() && activePeerMask == 0)) {
            if (func_0203d974())
                func_020338b0(0x125, 0xb0, 0x1b, 0);
            else
                func_020338b0(0x125, 0xb0, 0x1a, -1);
            func_02030a78();
            ChangeStepsAfterDelay(reinterpret_cast<const StepSet *>(&data_ov075_0211d930), 0xb4);
            data_0209d45c = data_0209d45c & ~mDisconnectHideMask;
            if (mDisconnectHideMask)
                mRenderStep = 0;
            G2x::SetBlendBrightness((volatile u16 *)0x4000050, 0x1f, -6);
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
    data_0209d454 = data_0209d454 | 5;
    mIconCount = 0;
}

// @symbol _ZN10dScEntry_c19ChangeStepsWithFadeEPKNS_7StepSetE
void dScEntry_c::ChangeStepsWithFade(const StepSet *steps) {
    mPendingSteps = steps;
    Step render = mRenderStep;
    SetSteps(&EntryFadeOutSteps);
    mRenderStep = render;
}

// @symbol _ZN10dScEntry_c21ChangeStepsAfterDelayEPKNS_7StepSetEi
void dScEntry_c::ChangeStepsAfterDelay(const StepSet *steps, s32 delay) {
    mPendingSteps = steps;
    mStepDelay = delay;
    Step render = mRenderStep;
    SetSteps(&EntryDelaySteps);
    mRenderStep = render;
}

// @symbol _ZN10dScEntry_c8SetStepsEPKNS_7StepSetE
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
// @symbol _ZN10dScEntry_c16CleanupResourcesEv
s32 dScEntry_c::CleanupResources() {
    func_020308b4(this);
    if (data_0209b2e8 != (void *)0) {
        Deallocate(data_0209b2e8);
        data_0209b2e8 = (void *)0;
    }
    data_0209d4a8 = (void *)0;
    Sound::UnsetPlayerVoiceGroup();
    CleanCommonModelDataArr();
    return 1;
}

/* Slot 12. Empty override. */
// @symbol _ZN10dScEntry_c16OnPendingDestroyEv
void dScEntry_c::OnPendingDestroy() {}

/* Slot 9. */
// @symbol _ZN10dScEntry_c6RenderEv
s32 dScEntry_c::Render() {
    if (mRenderStep != 0)
        (this->*mRenderStep)();
    func_0203083c();
    return 1;
}

/* Slot 6. Restores the sub-screen brightness after a step change, then the
 * per-frame step, then every live icon. */
// @symbol _ZN10dScEntry_c8BehaviorEv
s32 dScEntry_c::Behavior() {
    if (mIsFading != 0) {
        mFadeTick++;
        if ((mFadeTick & 1) == 0) {
            mFadeBrightness--;
            func_020551f0((void *)0x4001050, mFadeBrightness);
            if (mFadeBrightness == 0) {
                *(u16 *)0x4001050 = 0;
                mFadeBrightness = 0;
                mIsFading = 0;
            }
        }
    }

    if (data_0209f5bc->IsAtStart() == 0 || mIsFading != 0)
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
// @symbol _ZN10dScEntry_c13InitResourcesEv
s32 dScEntry_c::InitResources() {
    char *rawScene = (char *)this;
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
        void *paletteData = (void *)LoadFile(0x9807);
        GXS::LoadBGPltt(paletteData, 0, 0xe0);
        Deallocate(paletteData);
    }
    {
        void *paletteData = (void *)LoadFile(0x8406);
        GXS::LoadBGPltt(paletteData, 0xe0, 0x120);
        Deallocate(paletteData);
    }

    *(u16 *)0x4000008 = (*(u16 *)0x4000008 & ~3) | 3;
    *(u16 *)0x400000c = (*(u16 *)0x400000c & ~3) | 2;
    *(u16 *)0x400000e = (*(u16 *)0x400000e & ~3) | 1;
    SetBg0Offset(0, 0);
    SetBg2Offset(0, 0);
    SetBg3Offset(0, 0);
    data_0209d45c = 0x11;

    *(u16 *)0x4001008 = (*(u16 *)0x4001008 & ~3) | 1;
    *(u16 *)0x400100c = (*(u16 *)0x400100c & ~3) | 3;
    *(u16 *)0x400100e = (*(u16 *)0x400100e & ~3) | 3;
    SetSubBg0Offset(0, 0);
    SetSubBg2Offset(0, 0);
    SetSubBg3Offset(0, 0);
    data_0209d454 = 0x18;

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
                    data_0209b2e8 = (void *)LoadFile(0x8409);
                    UnloadArchive(0);
                    func_02030aa4(3);
                }
                {
                    int localPlayer = func_0203da9c();
                    func_02020028(localPlayer, data_0209f4ae[gActivePlayerSlot * 0x18]);
                }
            }
        } else {
            InitResultResources();
        }
    }

    func_ov075_0211601c((char *)&data_ov075_0211d71c, (int)rawScene, 1);
    data_0209d4a8 = &data_ov075_0211d71c;
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
// @symbol _ZN10dScEntry_c19BeforeInitResourcesEv
bool dScEntry_c::BeforeInitResources() {
    return ResetFadersAndSound();
}

// @symbol dScEntry_c_classInit_RESULT
/* The array runtime passes each element address and ignores lifecycle
 * results. Explicit function-pointer casts mark that runtime ABI
 * boundary. */
extern "C" {
/* local extern: the factory runs the base constructor over fresh operator new
   storage and stores the vptrs itself; C++ has no call spelling for a C2. */
extern void *_ZN7fBase_cC2Ev(void *);
extern void __cxa_vec_ctor(void *base, unsigned int count, unsigned int stride,
                           void (*ctor)(void *), void (*dtor)(void *));

extern char data_0208e4b8[];
extern char _ZTV10dScEntry_c[];
/* local extern: __cxa_vec_ctor takes the element constructor and destructor
   by address, and C++ cannot take a constructor's or destructor's address. */
extern dScEntry_c::icon_c *_ZN10dScEntry_c6icon_cD1Ev(dScEntry_c::icon_c *object);
extern dScEntry_c::icon_c *_ZN10dScEntry_c6icon_cC1Ev(dScEntry_c::icon_c *object);
extern OamAnimation *_ZN12OamAnimationD1Ev(OamAnimation *object);
extern OamAnimation *_ZN12OamAnimationC1Ev(OamAnimation *object);

/* Reconstructed source-style name: SM64DS proves dScEntry_c through RTTI,
 * allocation size, vtable identity, and the RESULT registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: func_ov075_0211a740. */
void *dScEntry_c_classInit_RESULT(void) {
    /* The raw byte pointer preserves the ROM factory's address-expression
       tree; typed member expressions fold five instructions in mwccarm.
       Members cast through it (pauseFlags, the three steps, mIcons,
       mOamAnimations) are byte-neutral. */
    char *p = (char *)fBase_c::operator new(sizeof(dScEntry_c));
    if (p) {
        dScEntry_c::Step v;
        _ZN7fBase_cC2Ev(p);
        *(char **)p = data_0208e4b8;
        *(char **)p = (char *)_ZTV8dScene_c;
        {
            unsigned char *flags = &((dScEntry_c *)p)->pauseFlags;
            *flags |= 1;
            *flags |= 4;
        }
        *(char **)p = _ZTV10dScEntry_c;
        v = 0;
        /* Keep the factory's aggregate-copy schedule for these three stores. */
        *(struct P2Copy *)&((dScEntry_c *)p)->mEnterStep = *(struct P2Copy *)&v;
        *(struct P2Copy *)&((dScEntry_c *)p)->mBehaviorStep = *(struct P2Copy *)&v;
        *(struct P2Copy *)&((dScEntry_c *)p)->mRenderStep = *(struct P2Copy *)&v;
        __cxa_vec_ctor(((dScEntry_c *)p)->mIcons, 9, sizeof(dScEntry_c::icon_c),
                       (void (*)(void *))_ZN10dScEntry_c6icon_cC1Ev,
                       (void (*)(void *))_ZN10dScEntry_c6icon_cD1Ev);
        __cxa_vec_ctor(
            ((dScEntry_c *)p)->mOamAnimations, 4, sizeof(((dScEntry_c *)p)->mOamAnimations[0]),
            (void (*)(void *))_ZN12OamAnimationC1Ev, (void (*)(void *))_ZN12OamAnimationD1Ev);
    }
    return p;
}
}

// @symbol _ZN12OamAnimationC1Ev
OamAnimation::OamAnimation() {}

// @symbol _ZN10dScEntry_c6icon_cC1Ev
dScEntry_c::icon_c::icon_c() {}
