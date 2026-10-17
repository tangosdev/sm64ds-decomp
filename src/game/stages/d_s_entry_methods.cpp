//cpp
/* leftovers:
 * - defer_codegen off stays, and the file stays in ROM order, the
 *   same form as d_s_entry.cpp below the hole. Reordering moves the
 *   run.
 * - This is the upper part of dScEntry_c's translation unit. The
 *   lower part is d_s_entry.cpp (0x02115ab8..0x02116128). Between
 *   them sit 69 func_ov075_* helpers (0x02116128..0x0211a210), each
 *   still its own file. No other class's code reaches them. They are
 *   called from InitResources, Behavior or one another, or reached
 *   through the step tables that __sinit_ov075_0211bb00 copies into
 *   .bss. That initializer also builds the graphCallback_c, so it is
 *   this unit's. A delinks entry is one contiguous .text claim, so
 *   the two parts join only once the hole is folded.
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
 * - The header holds 0x50, 0x6c..0x70 and 0x264..0x280 as opaque
 *   storage.
 * - InitResources keeps its `int b = (actorID == 7)' flag locals.
 *   Folding the first one into its condition rewrites the function
 *   (999 words, measured).
 * - The helpers it calls stay func_ov075_* and the arm9 globals keep
 *   their linker names. Naming belongs with recovered members.
 *
 * dScEntry_c, the course-entry / results scene. ov075 .text
 * 0x0211a210..0x0211a854: the six scene methods, the RESULT factory,
 * then the OamAnimation and icon_c constructors. The ENTRY factory at
 * 0x0211a854 is not in this file. It sits past the constructor tail
 * that closes this run. The code after it is reached from dEntObj_c
 * (d_ent_obj.cpp) and through tables __sinit_ov075_0211c51c reads,
 * not from this class.
 */
#pragma defer_codegen off

#include "types.h"
#include "PlayerInput.h"
#include "Sound.h"
#include "decl_common.h"
#include "dScEntry_c.h"

/* The Nitro SDK's sub-engine palette loader. No header in the tree
 * declares the namespace yet; d_s_star_sel_methods.cpp spells it the
 * same way. */
namespace GXS {
void LoadBGPltt(const void *, unsigned int, unsigned int);
}

extern "C" {
void func_020308b4(void *self);
void func_020551f0(void *s, int v);
int func_ov075_02119dc4(void *c, void *arg);
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
int func_ov075_02116ad4(void *arg);
int func_0203da3c(void);
void UnloadArchive(int i);
void func_02030aa4(int arg);
int func_0203da9c(void);
void func_02020028(int a, int b);
void func_ov075_02116818(char *c);
void func_ov075_0211601c(char *c, int a, int b);
}

extern void *data_0209d4a8;
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern u8 data_0209f4ae[];
extern s32 data_0208ee44;

/* The scene manager Behavior polls through its slot 5. */
struct VObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual int v5();
};
extern "C" VObj *data_0209f5bc;

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

/* Slot 6. Runs the sub-screen fade-out if one is in flight, then the
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

    if (data_0209f5bc->v5() == 0 || mIsFading != 0)
        return 1;

    if (data_0209fc68 != 0) {
        int t = (data_0209fc68 == 6);
        if (t != 0)
            func_ov075_02119dc4(this, &data_ov075_0211d930);
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
    char *c = (char *)this;
    Enable3dEngines();
    func_0200f2cc();

    {
        int b = (actorID == 7);
        if (b || param1 != 0 || func_0203d9b4() != 0) {
            LoadTextNarcs();
            LoadArchive(0);
            LoadArchive(1);
            func_0200f220();
        }
    }

    {
        void *f = (void *)LoadFile(0x9807);
        GXS::LoadBGPltt(f, 0, 0xe0);
        Deallocate(f);
    }
    {
        void *f = (void *)LoadFile(0x8406);
        GXS::LoadBGPltt(f, 0xe0, 0x120);
        Deallocate(f);
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
    unk_287 = 0;
    LoadFont3D();

    {
        int b6 = (actorID == 6);
        if (b6) {
            func_ov075_02116ad4(c);
            if (param1 == 0) {
                if (func_0203da3c() == 0) {
                    data_0209b2e8 = (void *)LoadFile(0x8409);
                    UnloadArchive(0);
                    func_02030aa4(3);
                }
                {
                    int r = func_0203da9c();
                    func_02020028(r, data_0209f4ae[gActivePlayerSlot * 0x18]);
                }
            }
        } else {
            func_ov075_02116818(c);
        }
    }

    func_ov075_0211601c((char *)&data_ov075_0211d71c, (int)c, 1);
    data_0209d4a8 = &data_ov075_0211d71c;
    Sound::LoadInitialGroup(0x2b);

    {
        int b6b = (actorID == 6);
        if (b6b) {
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
extern void *_ZN7fBase_cnwEj(unsigned int);
extern void *_ZN7fBase_cC2Ev(void *);
extern void __cxa_vec_ctor(void *base, unsigned int count, unsigned int stride,
                           void (*ctor)(void *), void (*dtor)(void *));

struct P2 {
    int a, b;
};
struct P2Copy {
    int words[2];
};
extern char data_0208e4b8[];
extern char _ZTV10dScEntry_c[];
extern struct P2 data_02086b58;
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
    char *p = (char *)_ZN7fBase_cnwEj(sizeof(dScEntry_c));
    if (p) {
        struct P2 v;
        _ZN7fBase_cC2Ev(p);
        *(char **)p = data_0208e4b8;
        *(char **)p = (char *)_ZTV8dScene_c;
        {
            unsigned char *flags = &((dScEntry_c *)p)->pauseFlags;
            *flags |= 1;
            *flags |= 4;
        }
        *(char **)p = _ZTV10dScEntry_c;
        /* Array-member copies preserve the original C aggregate-copy schedule
           in C++ without changing the two-word value's type or layout. */
        *(struct P2Copy *)&v = *(struct P2Copy *)&data_02086b58;
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
