//cpp
/* dScMgSound_c -- the MG_SOUND minigame scene, ov006, 82 functions
 * (.text 0x02119904..0x0211cbd0), the registry factory among them.
 * Each round draws a pattern (mPattern) and lays out note slots on the touch
 * screen; touching a slot plays its note back on the pattern's instrument.
 *
 * The class is a dScMgSingle3DBase_c (its RTTI is at 0x0213f6e4). The
 * compiler emits .text in reverse source order, so the functions run here
 * from the highest address down. Do not reorder.
 *
 * Still raw: the func_ and data_ helpers are unnamed in symbols.txt, and the
 * header leaves the note slots at 0x50e8 (stride 0x14), the rows at 0x51b0
 * (stride 0x24) and the playback queue at 0x5610 as raw bytes, so they are
 * reached by offset.
 */

#include "dScMgSound_c.h"
#include "types.h"
#include "decl_common.h"

/* Local views of the scene's arrays; each helper keeps the view it was
   matched with. The dispatch helpers see `C` as an incomplete type and two
   others see it complete as `CFull`. The compiler picks the pointer-to-member
   layout from that, so the two must stay distinct types. */

/* --- the state-machine dispatch family (incomplete view) ------------------ */
struct C;                                   /* opaque: dispatch only */
typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };                  /* PMF dispatch entry */

/* --- the same dispatch, seen by the two members that index the array ------ */
struct Elem {                               /* 0x24 stride at +0x51d1 */
  unsigned char idx;
  char pad[0x23];
};
struct CFull {
  char pad[0x51d1];
  Elem arr[1];
};
typedef void (CFull::*PMFFull)(int);
struct EntryFull { PMFFull pmf; };

/* --- the 0x24-stride array at +0x51b4, two disjoint field views ----------- */
struct BallSlot {                           /* func_ov006_0211a500's view */
    int a;            /* +0x00 (this+0x51b4) */
    int pad4;         /* +0x04 */
    int b;            /* +0x08 (this+0x51bc) */
    int c;            /* +0x0c (this+0x51c0) */
    char pad10[8];    /* +0x10 */
    unsigned char f18;/* +0x18 (this+0x51cc) */
    unsigned char f19;/* +0x19 (this+0x51cd) */
    char pad1a[10];   /* pad to 0x24 */
};
struct BallObj {
    char pad[0x51b4];
    struct BallSlot arr[4];
};

struct BigSlot {                            /* func_ov006_0211aa94's view */
    int b4;
    int unk4;
    int bc;
    int c0;
    int unk10;
    int unk14;
    unsigned char cc;
    unsigned char cd;
    unsigned char pad[10];
};
struct BigObj {
    char pad[0x51b4];
    BigSlot arr[16];
};

/* --- the 0x24-stride array at +0x51b8 ------------------------------------- */
struct NoteSlot {                           /* func_ov006_0211a9fc's view */
    u32 value;     /* +0x00 (this+0x51b8) */
    u32 counter;   /* +0x04 */
    char _pad8[8];
    u16 halfword;  /* +0x10 (this+0x51c8) */
    char _pad12[3];
    u8 flagA;      /* +0x15 (this+0x51cd) */
    char _pad16[3];
    u8 flagB;      /* +0x19 (this+0x51d1) */
    u8 type;       /* +0x1a (this+0x51d2) */
    char _pad1b[9];
};
struct NoteObj {
    char _pad[0x51b8];
    struct NoteSlot entries[1];  /* 0x51b8 */
};

/* --- the 0x14-stride array at +0x50e8, two field views -------------------- */
struct E { char _[0x14]; };                 /* stride only; no field is read */

struct Slot {                               /* func_ov006_0211b654's view */
    int f00; int f04; int f08; int f0c; int f10; int f14;
    u16 f18; u16 f1a;
    u8 f1c; u8 f1d; u8 f1e; u8 f1f; u8 f20; u8 f21; u8 f22; u8 f23;
};
struct Mgr { u8 pad[0x51b0]; struct Slot slot[30]; };

struct SndEntry {
    s32 x;
    u8 unk04[0x0c - 0x04];
    u8 b0c;
    u8 b0d;
    u8 b0e;
    u8 b0f;
    u8 b10;
    u8 b11;
    u8 b12;
    u8 b13;
};
struct SndCtx {
    u8 unk00[0x50e8];
    SndEntry ent[67];
    u8 unk5624[3];
    u8 mode;
};

/* --- func_ov006_0211b9c8 reaches the scene through its own vtable --------- */
struct SceneVt {
    virtual void p00(); virtual void p01(); virtual void p02(); virtual void p03();
    virtual void p04(); virtual void p05(); virtual void p06(); virtual void p07();
    virtual void p08(); virtual void p09(); virtual void p10(); virtual void p11();
    virtual void p12(); virtual void p13(); virtual void p14(); virtual void p15();
    virtual void p16(); virtual void p17(); virtual void p18(); virtual void p19();
    virtual void p20(); virtual void p21(); virtual void p22(); virtual void p23();
    virtual void p24(); virtual void p25(); virtual void p26(); virtual void p27();
    virtual void p28(); virtual void p29(); virtual void p30(); virtual void p31();
    virtual void p32(); virtual void p33(); virtual void p34();
    virtual int check();
};

/* --- the two RAW members' shared jump-table row --------------------------- */
struct Ent { int a; int b; };

namespace Sound {
    void PlayBank2_2D(unsigned int id);
    void LoadAndSetMusic_Layer1(int music);
}

namespace G2S {
    unsigned GetBG2CharPtr();
}

namespace GX {
    void LoadOBJPltt(const void *data, u32 offset, u32 size);
}

namespace GXS {
    void LoadBGPltt(const void *data, u32 offset, u32 size);
    void LoadOBJPltt(const void *data, u32 offset, u32 size);
}

#define A(p) ((int)(p))
#define F1E(b,i) (*(unsigned char*)((char*)(b) + 0x51d1 + (i)*0x24))
#define M(p) ((void*)(int)(((long long)(int)(p)) & ~0LL))
#define RND (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)
#define LAUNDER(p) ((int)(p))

extern "C" {
extern int Hud_RenderSprite(int a,int b,int c,int d,int e);
extern int data_ov006_02137ae8[];
extern unsigned short data_ov006_0212ee38[];
extern int data_ov006_02138d28[];
extern u16 data_ov006_0212ee28[];
extern void func_ov004_020b2574(int a, int b);
extern const short data_02082214[];
extern void* data_ov006_0213f6f0[];
extern int data_ov006_0212ef30[];
extern int data_ov006_0212ef0c[];
extern "C" EntryFull data_ov006_02142db0[];
extern int data_ov006_0212eeac[];
extern int data_ov006_0212ee88[];
extern "C" EntryFull data_ov006_02142d08[];
extern int data_ov006_0212ee58[];
extern int data_ov006_0212ee4c[];
extern PMF data_ov006_02142cc0[];
extern int data_ov006_0212ee64[];
extern int data_ov006_0212ef00[];
extern "C" PMF data_ov006_02142d98[];
extern int data_ov006_0212eedc[];
extern "C" Entry data_ov006_02142d68[];
extern int data_ov006_0212ee94[];
extern "C" Entry data_ov006_02142d20[];
extern u32 data_ov006_0212ee70[];
extern "C" Entry data_ov006_02142cf0[];
extern int data_ov006_0212ee7c[];
extern "C" Entry data_ov006_02142cd8[];
extern int data_ov006_0212ee40[];
extern "C" Entry data_ov006_02142de0[];
extern int data_ov006_0212ef18[];
extern int data_ov006_0212ef24[];
extern "C" Entry data_ov006_02142dc8[];
extern int data_ov006_0212eef4[];
extern int data_ov006_0212eee8[];
extern "C" Entry data_ov006_02142d80[];
extern int data_ov006_0212eed0[];
extern int data_ov006_0212eec4[];
extern "C" Entry data_ov006_02142d50[];
extern int data_ov006_0212eeb8[];
extern int data_ov006_0212eea0[];
extern unsigned char data_ov006_0212ee0c[];
extern "C" PMF data_ov006_02142d38[];
extern u8 data_ov006_0212ee30[];
extern void func_02012790(int);
extern void func_ov006_020c271c(void *c);
extern void func_ov006_020c2664(char *c);
extern int data_ov006_0212ef7c[];
extern int data_ov006_0212ef8c[];
extern int data_ov006_0212ef5c[];
extern int data_ov006_0212ef6c[];
void func_ov006_0211ba88(char *base, int idx);
extern unsigned short data_ov006_0212ef4c[];
void func_ov006_0211b654(struct Mgr *m, int n);
void func_ov006_020c2300(char *p);
void func_02012174(u32 bank, u32 id);
extern u16 data_ov006_0212ef3c[];
extern u16 data_ov006_0213f794[];
extern u16 data_ov006_0213f7e8[];
extern u8 data_020a0e40[];
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern u8 data_ov006_0212ee10[];
extern u8 data_ov006_0212ee18[];
extern u8 data_ov006_0212ee20[];
extern int data_ov006_0212ef9c[];
extern int data_ov006_0212efb0[];
extern u16 *data_ov006_0213f6fc[];
extern void func_ov006_020c2924(char *c);
extern void func_ov006_020c2594(void *c);
extern void func_ov006_0211c478(char *base);
extern void func_ov006_0211c080(char *o);
extern "C" void func_ov004_020b1e34(void *c, int a, int b, int d);
void FreeGfxSlotsById(int id);
int LoadFile(int handle);
void DecompressLZ16(int src, void *dst);
void func_ov006_020c225c(void *p);
int func_ov006_020c3050(void *p);
extern u8 data_0209d45c;
extern u8 data_0209d454;
}

// @symbol dScMgSound_c_classInit
/* The MG_SOUND registry factory. It sits directly after InitResources and
   installs this class's vtable, so it was compiled with the class.
   Still a hand-written constructor: the header declares none, so the base
   and Particle::SysTracker constructors are called by their mangled names.
   The two vtable stores differ on purpose. dScMgSingle3DBase_c's table is
   external and its symbol is the address point; this class's table is
   emitted here, where the symbol sits two words lower, hence the `+ 2`. */
extern "C" void *_ZN7fBase_cnwEj(unsigned int);
extern "C" void _ZN11dScMgBase_cC2Ev(void *);
extern "C" void _ZN8Particle10SysTrackerC1Ev(void *);
extern "C" void func_ov006_020c33dc(void *);
extern void *_ZTV19dScMgSingle3DBase_c[];
extern void *_ZTV12dScMgSound_c[];

extern "C" void *dScMgSound_c_classInit(void)
{
    char *p = (char *)_ZN7fBase_cnwEj(0x562c);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        *(void **)p = _ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(p + 0x471c);
        *(void **)p = _ZTV12dScMgSound_c + 2;
        func_ov006_020c33dc(p + 0x4f38);
    }
    return p;
}

// @symbol _ZN12dScMgSound_c13InitResourcesEv
/* Loads the minigame's graphics for both screens, sets up the camera and
   the table, then deals the first round. */
s32 dScMgSound_c::InitResources()
{
    u8 *raw = (u8 *)this;
    int objChars, objPltt, file;

    data_0209d45c |= 0x11;
    objChars = LoadFile(0xff);
    objPltt = LoadFile(0x100);
    DecompressLZ16(objChars, (void *)0x6400000);
    GX::LoadOBJPltt((const void *)objPltt, 0, 0x100);

    data_0209d454 |= 4;
    file = LoadFile(0x98);
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x214;
    DecompressLZ16(file, (void *)G2S::GetBG2CharPtr());
    Deallocate((void *)file);

    file = LoadFile(0x99);
    GXS::LoadBGPltt((const void *)file, 0x60, 0x1a0);
    Deallocate((void *)file);

    file = LoadFile(0x9a);
    func_02056374((const void *)file, 0, 0x800);
    Deallocate((void *)file);

    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile u32 *)0x4001018 = 0;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 1;
    DecompressLZ16(objChars, (void *)0x6600000);
    GXS::LoadOBJPltt((const void *)objPltt, 0, 0x100);
    Deallocate((void *)objChars);
    Deallocate((void *)objPltt);

    func_ov006_020c225c((void *)(raw + 0x4660));
    if (func_ov006_020c3050((void *)&mTable) == 0)
        return 0;

    mTable.mSuppressSound = 1;
    func_ov006_0211c478((char *)this);
    func_ov006_0211c080((char *)this);
    unk_5626 = 3;
    mState = 1;
    *(u16 *)(raw + 0x5618) = 0x20;
    func_ov004_020b6808();
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
    mHudScore = 0;
    return 1;
}

// @symbol _ZN12dScMgSound_c8BehaviorEv
/* One frame of the round, by mState: 0 starts, 1 counts down the intro at
   0x5618, 2 plays the round and, when the result countdown at 0x5616 runs
   out, bumps the win counter (capped at 9999), 3 counts down a retry. The
   countdowns sit in the raw queue storage at 0x5610.
   The `(int)` casts on the three countdown decrements are needed: without
   them the address folds into the decrement and the function comes out
   short. */
s32 dScMgSound_c::Behavior()
{
    char *raw = (char *)this;

    switch (mState) {
    case 0:
        mState = 1;
        break;
    case 1:
        if (*(u16 *)(raw + 0x5618) != 0) {
            (*(u16 *)(int)(raw + 0x5618))--;
            if (*(u16 *)(raw + 0x5618) == 0) {
                FreeGfxSlotsById(0x1d);
                if (mPromptBlinkCount == 0) {
                    mPromptEnabled = 1;
                    mPromptBlinkCount = 1;
                    mPromptBlinkTimer = 0;
                }
            }
        }
        func_ov006_0211b954(raw);
        func_ov006_0211b80c(raw);
        func_ov006_0211b5e0(raw);
        func_ov006_0211b790(raw);
        break;
    case 2:
        func_ov006_0211b954(raw);
        func_ov006_0211b5e0(raw);
        if (*(u16 *)(raw + 0x5616) != 0) {
            (*(u16 *)(int)(raw + 0x5616))--;
            if (*(u16 *)(raw + 0x5616) == 0) {
                if (unk_5626 != 0) {
                    mTable.mSuppressSound = 0;
                    func_ov006_020c2594((char *)&mTable);
                    if (unk_5626 == 3)
                        func_ov004_020b67f8();
                    func_ov004_020b0a54(0);
                    {
                        char *active = (char *)data_ov004_020beb68;
                        if (active != 0) {
                            if (*(int *)(active + 0xb4) < 9999)
                                (*(int *)(int)(active + 0xb4))++;
                            if (*(int *)(active + 0xb4) > *(int *)(active + 0xb8))
                                *(int *)(active + 0xb8) = *(int *)(active + 0xb4);
                        }
                    }
                    func_ov004_020adb1c(data_ov004_020beb68 != 0
                                            ? *(int *)((char *)data_ov004_020beb68 + 0xb4) : 0);
                    func_ov006_02119ba4(raw);
                    func_ov006_02119a88(raw);
                    mPromptEnabled = 0;
                } else {
                    mState = 3;
                    *(u16 *)(raw + 0x5616) = 0x20;
                    func_ov006_0211b9c8(raw);
                }
            }
        } else {
            func_ov006_02119b00(raw);
            func_ov006_02119a18(raw);
        }
        break;
    case 3:
        func_ov006_0211b954(raw);
        func_ov006_0211b5e0(raw);
        if (*(u16 *)(raw + 0x5616) != 0) {
            (*(u16 *)(int)(raw + 0x5616))--;
            if (*(s16 *)(raw + 0x5616) <= 0) {
                mTable.mSuppressSound = 0;
                func_ov006_020c2440((char *)&mTable);
                func_ov004_020b0a54(0x12);
                mPromptEnabled = 0;
                *(u16 *)(raw + 0x5616) = 0;
            }
        }
        break;
    }

    func_ov006_020c2b8c((char *)&mTable);
    return 1;
}

// @symbol _ZN12dScMgSound_c6RenderEv
/* Draws the HUD and each of the scene's layers, then the table. */
s32 dScMgSound_c::Render()
{
    char *raw = (char *)this;

    func_ov004_020b1e34(raw, 0xe0, 0x14, 1);
    func_ov006_02119c74(raw);
    func_ov006_02119bdc(raw);
    func_ov006_02119bc4(raw);
    func_ov006_021199c0(raw);
    func_ov006_02119aa8(raw);
    func_ov006_020c29dc(&mTable);
    return 1;
}

// @symbol _ZN12dScMgSound_c13OnYoshiTryEatEi
/* Starts a round. With mode 0 the streak at 0xbc goes up (capped at 9998);
   otherwise it and the scene's total are cleared.
   `raw` must stay a `char *`: laundering `this` through an int puts the large
   offsets in the literal pool and the function grows by 0x10. */
void dScMgSound_c::OnYoshiTryEat(int mode)
{
    char *raw = (char *)this;

    int *active;
    int total;

    mState = 0;

    if (mode == 0) {
        unk_0bc = unk_0bc + 1;
        if (unk_0bc > 0x270e)
            unk_0bc = 0x270e;
    } else {
        unk_0bc = 0;
        if (unk_0bc > 0x270e)
            unk_0bc = 0x270e;

        active = (int *)data_ov004_020beb68;
        if (active)
            *(int *)((char *)active + 0xb4) = 0;

        active = (int *)data_ov004_020beb68;
        if (active)
            total = *(int *)((char *)active + 0xb4);
        else
            total = 0;
        func_ov004_020adb1c(total);
    }

    mTable.mSuppressSound = 1;
    func_ov006_0211c478(raw);

    unk_5626 = 3;
    func_ov006_0211c080(raw);

    *(short*)(raw + 0x5618) = 0x20;
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);

    Sound::LoadAndSetMusic_Layer1(6);
}

// @symbol _ZN12dScMgSound_c9Virtual50Ev
/* Virtual50 is dScMgBase_c.h's placeholder name; what the slot does is not
   known yet. */
void dScMgSound_c::Virtual50()
{
    char *raw = (char *)this;

    func_ov006_020c2594(raw + 0x4f38);
}

// @symbol func_ov006_0211c478
extern "C" {
void func_ov006_0211c478(char *scene)
{
    dScMgSound_c *self = (dScMgSound_c *)scene;
    int i;
    int cnt1;
    int cnt2;
    char *b = scene;

    for (cnt1 = 0; cnt1 < 10; cnt1++) {
        *(int *)(b + 0x50e8) = 0;
        *(int *)(b + 0x50ec) = 0;
        *(short *)(b + 0x50f0) = 0;
        *(unsigned char *)(b + 0x50f4) = 0;
        *(unsigned char *)(b + 0x50f5) = 0;
        *(unsigned char *)(b + 0x50f6) = 0;
        *(unsigned char *)(b + 0x50f7) = 0;
        *(unsigned char *)(b + 0x50f8) = 0;
        b += 0x14;
    }

    for (cnt2 = 0; cnt2 < 5; cnt2++) {
        *(unsigned char *)(scene + 0x561a + cnt2) = 0;
        *(unsigned char *)(scene + 0x561f + cnt2) = 0;
    }

    b = scene;
    for (i = 0; i < 0x1e; i++) {
        *(int *)(b + 0x51b0) = 0;
        *(int *)(b + 0x51b4) = 0;
        *(int *)(b + 0x51b8) = 0;
        *(int *)(b + 0x51bc) = 0;
        *(int *)(b + 0x51c4) = 0;
        *(short *)(b + 0x51c8) = 0;
        *(unsigned char *)(b + 0x51cc) = 0;
        *(unsigned char *)(b + 0x51cd) = 0;
        *(unsigned char *)(b + 0x51ce) = 0;
        *(unsigned char *)(b + 0x51cf) = 0;
        *(unsigned char *)(b + 0x51cc) = 0;
        *(unsigned char *)(b + 0x51d0) = 0;
        *(unsigned char *)(b + 0x51d1) = 0;
        *(unsigned char *)(b + 0x51d2) = 0;
        *(unsigned char *)(b + 0x51d3) = 0;
        b += 0x24;
    }

    self->mSpriteB.active = 0;
    self->mSpriteB.visible = 1;
    self->mSpriteB.x = 0xbd000;
    self->mSpriteB.y = 0x97000;
    self->mSpriteB.frame = 0;
    self->mSpriteA.active = 0;
    self->mSpriteA.visible = 1;
    self->mSpriteA.x = 0xa0000;
    self->mSpriteA.y = 0x9d000;
    self->mSpriteA.frame = 0;

    *(short *)(scene + 0x5612) = 0;
    *(short *)(scene + 0x5610) = *(unsigned short *)(scene + 0x5612);
    *(short *)(scene + 0x5614) = 0;
    self->unk_5625 = 0;
    self->unk_5624 = 0;
    *(short *)(scene + 0x5616) = 0;

    func_ov006_020c2924(scene + 0x4f38);
}
}

// @symbol func_ov006_0211c080
extern "C" {
void func_ov006_0211c080(char *scene)
{
    dScMgSound_c *self = (dScMgSound_c *)scene;
    int sel;
    int k;
    int count;
    int n;
    int half;
    int speed;
    int i;

    sel = *(int *)(scene + 0xbc);
    self->mPrevPattern = self->mPattern;
    if (sel >= 5) {
        sel = (u32)(RND * 5) >> 15;
        if (sel == self->mPrevPattern) {
            sel += ((u32)(RND << 2) >> 15) + 1;
            if (sel >= 5)
                sel -= 5;
        }
    }
    self->mPattern = sel;
    k = self->mPattern;
    count = data_ov006_0212ee18[k];
    speed = data_ov006_0212ee10[k];
    if (k == 3) {
        int a = (u32)(RND << 2) >> 15;
        int b = (u32)(RND * 3) >> 15;
        self->mLanes[0] = b;
        self->mLanes[1] = self->mLanes[0] + 1;
        if (self->mLanes[1] >= 3)
            *(u8 *)LAUNDER(scene + 0x5620) -= 3;
        self->mLanes[2] = self->mLanes[1] + 1;
        if (self->mLanes[2] >= 3)
            *(u8 *)LAUNDER(scene + 0x5621) -= 3;
        *(u8 *)LAUNDER(scene + 0x561f) += a * 3;
        *(u8 *)LAUNDER(scene + 0x5620) += a * 3;
        *(u8 *)LAUNDER(scene + 0x5621) += a * 3;
    } else {
        half = count >> 1;
        for (i = 0; i < half; i++) {
            int v = (u32)(RND * speed) >> 15;
            *(u8 *)(scene + i + 0x561f) = v;
            if (i != 0) {
                int dup;
                int j;
                for (;;) {
                    dup = 0;
                    j = 0;
                    for (; j < i; j++) {
                        if (*(u8 *)(scene + i + 0x561f) == *(u8 *)(scene + j + 0x561f)) {
                            dup = 1;
                            break;
                        }
                    }
                    if (dup == 0)
                        break;
                    *(u8 *)LAUNDER(scene + i + 0x561f) = (u32)(RND * speed) >> 15;
                }
            }
        }
    }
    k = self->mPattern;
    if (k == 1 || k == 4) {
        self->unk_560c = data_ov006_0212efb0[((u32)*(int *)(scene + 0xbc) >> 2) & 3];
    }
    half = count >> 1;
    for (i = 0; i < half; i++) {
        *(u8 *)LAUNDER(scene + i + 0x561f) += data_ov006_0212ef9c[self->mPattern];
    }
    {
        int xi;
        char *p;
        int zi;
        char *q;
        u8 *slot;
        int t;

        n = 0;
        if (count <= 0)
            return;
        zi = 1;
        xi = 0;
        p = scene;
        do {
            t = data_ov006_0212ee20[self->mPattern];
            *(int *)(p + 0x50e8) = data_ov006_0213f6fc[t][xi] << 12;
            *(int *)(p + 0x50ec) = (*(u16 * volatile *)&data_ov006_0213f6fc[t])[zi] << 12;
            *(u16 *)(p + 0x50f0) = 0;
            *(u8 *)(p + 0x50f4) = 0;
            *(u8 *)(p + 0x50f5) = 1;
            *(u8 *)(p + 0x50f6) = 1;
            *(u8 *)(p + 0x50f7) = 0;
            *(u8 *)(p + 0x50f9) = 0;
            do {
                q = scene + ((u32)(RND * half) >> 15);
                slot = (u8 *)LAUNDER(q + 0x561a);
            } while (*slot >= 2);
            *(u8 *)(p + 0x50f8) = *(u8 *)(q + 0x561f);
            (*slot)++;
            xi += 2;
            p += 0x14;
            zi += 2;
            n++;
        } while (n < count);
    }
}
}

// @symbol func_ov006_0211bf44
extern "C" {
void func_ov006_0211bf44(char* scene, int slot)
{
    u8 player;
    int offset;
    int active;
    char* entry;
    int dx;
    int dy;

    if (*(int*)(scene + 0x5608) != 1) {
        return;
    }
    if (*(u8*)(scene + 0x5624) >= 2) {
        return;
    }

    player = data_020a0e40[0];
    offset = player * 4;
    active = 0;
    if (data_020a0de8[offset] != 0) {
        if (data_020a0de9[offset] != 0) {
            active = 1;
        }
    }
    if (active == 0) {
        return;
    }

    entry = scene + slot * 0x14;
    entry += 0x5000;
    dx = data_020a0dea[player * 4] - (*(int*)(entry + 0xe8) >> 12);
    dy = data_020a0deb[player * 4] - (*(int*)(entry + 0xec) >> 12);

    if (dx > 0x18) {
        return;
    }
    if (dx < -0x18) {
        return;
    }
    if (dy > 0x18) {
        return;
    }
    if (dy < -0x18) {
        return;
    }

    *(u8*)(entry + 0xf4) = 1;
    *(u16*)(entry + 0xf0) = 0;
    *(u8*)(entry + 0xf7) = 0;
    {
        u8* counter = (u8*)(int)(scene + 0x5624);
        *counter = *counter + 1;
    }
    Sound::PlayBank2_2D(0x201);
}
}

// @symbol func_ov006_0211bc8c
/* Advances one note slot's animation. When the slot's last frame is
   reached its note goes on the playback queue at 0x5610 and is played; the
   instrument at 0x5627 and Virtual8C choose between func_02012790, the bank
   handle at 0x560c and Sound::PlayBank2_2D.
   Two spellings are load-bearing. func_ov006_0211b654 is passed `idx` even
   though it is already in r1: dropping it changes the register allocation
   of the queue block. And the two queue stores keep their different forms:
   the compiler picks a different address shape for each, and forcing
   either one breaks the other. */
extern "C" void func_ov006_0211bc8c(dScMgSound_c *self, int idx)
{
    char *raw = (char *)self;
    int off = idx * 0x14;
    u8 note;
    u8 instrument;

    *(u16 *)(raw + 0x50f0 + off) = *(u16 *)(raw + 0x50f0 + off) + 1;
    if (*(u16 *)(raw + 0x50f0 + off) < data_ov006_0212ef3c[((u8 *)(raw + 0x50f7))[off]]) return;
    *(u16 *)(raw + 0x50f0 + off) = 0;
    ((u8 *)(raw + 0x50f7))[off] = ((u8 *)(raw + 0x50f7))[off] + 1;
    if (((u8 *)(raw + 0x50f7))[off] <= 6) return;
    ((u8 *)(raw + 0x50f7))[off] = 6;
    ((u8 *)(raw + off))[0x50f4] = 2;

    *(u16 *)(raw + *(u8 *)(raw + 0x5625) * 2 + 0x5610) = ((u8 *)(raw + 0x50f8))[off] + 1;
    ((u16 *)(raw + 0x5610))[*(u8 *)(raw + 0x5625)] |= idx << 8;
    (*(u8 *)(raw + 0x5625))++;
    *(u16 *)(raw + 0x5614) = 0x20;

    func_ov006_0211b654((struct Mgr *)self, idx);
    note = ((u8 *)(raw + 0x50f8))[off];

    if (self->Virtual8C()) {
        instrument = *(u8 *)(raw + 0x5627);
        if (instrument == 0 || instrument == 2) {
            if (data_ov006_0213f794[note] == 2) {
                func_02012790(data_ov006_0213f7e8[note]);
            } else {
                func_02012174(*(u32 *)(raw + 0x560c), data_ov006_0213f7e8[note]);
            }
        } else if (instrument == 1) {
            int n = note - 0x1a;
            int k = 0;
            while (n >= 4) { n -= 4; k++; }
            func_02012174(n, data_ov006_0213f7e8[0x1a + k * 4]);
        } else {
            Sound::PlayBank2_2D(data_ov006_0213f7e8[note]);
        }
    } else {
        instrument = *(u8 *)(raw + 0x5627);
        if (instrument == 0 || instrument == 2) {
            func_02012790(data_ov006_0213f7e8[note]);
        } else if (instrument == 1) {
            func_02012174(*(u32 *)(raw + 0x560c), data_ov006_0213f7e8[note]);
        } else if (instrument == 4) {
            if (data_ov006_0213f794[note] == 2) {
                Sound::PlayBank2_2D(data_ov006_0213f7e8[note]);
            } else {
                func_02012174(*(u32 *)(raw + 0x560c), data_ov006_0213f7e8[note]);
            }
        } else {
            Sound::PlayBank2_2D(data_ov006_0213f7e8[note]);
        }
    }

    if (*(u8 *)(raw + 0x5624) == 1) func_ov006_020c2300((char *)&self->mTable);
}

// @symbol func_ov006_0211bc68
extern "C" {
void func_ov006_0211bc68(char* scene, int slot) {
    if (*(unsigned char*)(scene+0x5624) == 0) {
        struct E* a = (struct E*)(scene+0x5000);
        *(unsigned char*)((char*)&a[slot]+0xf4) = 3;
    }
}
}

// @symbol func_ov006_0211bbe0
extern "C" {
void func_ov006_0211bbe0(char *c, int idx);
void func_ov006_0211bbe0(char *scene, int slot){
    int o = slot * 0x14;
    char *cb = scene + 0x50f0;
    char *p = scene + o;
    unsigned char b;
    *(unsigned short*)(cb + o) = (unsigned short)(*(unsigned short*)(cb + o) + 1);
    b = ((unsigned char *)p)[0x50f7];
    if (*(unsigned short*)(cb + o) < data_ov006_0212ef4c[b]) return;
    *(unsigned short*)(cb + o) = 0;
    ((unsigned char *)(scene + 0x50f7))[o] = (unsigned char)(((unsigned char *)(scene + 0x50f7))[o] - 1);
    if (((unsigned char *)(scene + 0x50f7))[o] == 0) ((unsigned char *)p)[0x50f4] = 0;
}
}

// @symbol func_ov006_0211ba88
/* Scrolls one note slot left by 0x10 per call and clears it once it has
   gone off screen. Its column comes from the per-mode tables
   data_ov006_0212ef5c and data_ov006_0212ef6c.
   Three spellings are load-bearing: Virtual8C is asked through `this` in
   C++, the store at +0x0c is a byte, and the last two clears index the table
   as an array while every earlier access multiplies by 0x14. */
void func_ov006_0211ba88(char *scene, int slot)
{
    SndCtx *ctx = (SndCtx *)scene;
    int n;
    int i;
    int limit;
    s32 *cur;
    u8 *flag;

    cur = (s32 *)(scene + 0x50e8 + slot * 0x14);
    *cur = *cur - 0x10000;

    if (((dScMgSound_c *)scene)->Virtual8C() != 0) {
        limit = data_ov006_0212ef5c[*(u8 *)(scene + 0x5000 + 0x627)];
    } else {
        limit = data_ov006_0212ef6c[*(u8 *)(scene + 0x5000 + 0x627)];
    }

    flag = (u8 *)(scene + 0x50f9 + slot * 0x14);
    if (*flag == 0) {
        n = slot;
        if (slot >= limit) {
            do {
                n = n - limit;
            } while (n >= limit);
        }
        if (n == 0) {
            *flag = 1;
            return;
        }
        for (i = 0; i < n; i++) {
            char *e = scene + (slot - i - 1) * 0x14;
            if (*(u8 *)(e + 0x5000 + 0xf5) != 0) {
                if ((*(int *)(scene + slot * 0x14 + 0x50e8) - *(int *)(e + 0x5000 + 0xe8)) >> 0xc <= 4) {
                    *(u8 *)(scene + slot * 0x14 + 0x50f9) = 1;
                    *(u8 *)(e + 0x5000 + 0xf4) = 4;
                }
            }
        }
    }

    if (*cur >> 0xc > -0x18) return;

    ctx->ent[slot].b0d = 0;
    ctx->ent[slot].b0e = 0;
}

// @symbol func_ov006_0211b9c8
extern "C" void func_ov006_0211b9c8(char *scene) {
    SceneVt *obj = (SceneVt *)scene;
    int lr;
    int ip;
    int found;
    int off;
    if (obj->check()) {
        lr = data_ov006_0212ef7c[*(unsigned char*)((char*)obj + 0x5627)];
    } else {
        lr = data_ov006_0212ef8c[*(unsigned char*)((char*)obj + 0x5627)];
    }
    for (ip = 0, off = 0; ip < 2; ip++) {
        int sb;
        found = -1;
        for (sb = 0; sb < lr; sb++) {
            int e = sb + off;
            if (*(unsigned char*)((char*)obj + e * 20 + 0x50f5) != 0)
                found = e;
        }
        if (found != -1) {
            *(unsigned char*)((char*)obj + found * 20 + 0x50f4) = 4;
        }
        off += lr;
    }
}

// @symbol func_ov006_0211b954
extern "C" {
extern Ent data_ov006_02142df8[];
void func_ov006_0211b954(char* scene){
  int i=0;
  char* r5=scene;
  do{
    if(*(unsigned char*)(r5+0x50f5)!=0){
      int idx=*(unsigned char*)(r5+0x50f4);
      Ent* e=&data_ov006_02142df8[idx];
      int adj=e->b;
      char* obj=scene+(adj>>1);
      int fn;
      if(adj&1){
        fn=*(int*)(*(int*)obj + e->a);
      } else {
        fn=e->a;
      }
      ((void(*)(void*,int))fn)(obj,i);
    }
    i++;
    r5+=0x14;
  }while(i<10);
}
}

// @symbol func_ov006_0211b80c
extern "C" {
void func_ov006_0211b80c(char *scene){
  if((unsigned char)scene[0x5625] < 2) return;
  if(*(unsigned short*)(scene+0x5614) != 0){
    *(unsigned short*)M(scene + 0x5614) -= 1;
    if(*(short*)(scene+0x5614) <= 0) *(unsigned short*)(scene+0x5614) = 0;
    return;
  }
  if((*(unsigned short*)(scene+0x5610) & 0xff) == (*(unsigned short*)(scene+0x5612) & 0xff)){
    *((char*)&((struct E*)scene)[*(unsigned short*)(scene+0x5610) >> 8] + 0x50f5) = 0;
    *((char*)&((struct E*)scene)[*(unsigned short*)(scene+0x5612) >> 8] + 0x50f5) = 0;
    func_02012790(0x26);
    { volatile void* p = (void*)(scene+0x4f38); func_ov006_020c271c((void*)p); }
  } else {
    *((char*)&((struct E*)scene)[*(unsigned short*)(scene+0x5610) >> 8] + 0x50f4) = 3;
    *((char*)&((struct E*)scene)[*(unsigned short*)(scene+0x5612) >> 8] + 0x50f4) = 3;
    func_02012790(0xe);
    (*(unsigned char*)M(scene + 0x5626))--;
    func_02012790(0x12f);
    func_ov006_020c2664(scene+0x4f38);
  }
  *(unsigned short*)(scene+0x5612) = 0;
  *(unsigned short*)(scene+0x5610) = *(unsigned short*)(scene+0x5612);
  scene[0x5625] = 0;
  scene[0x5624] = 0;
}
}

// @symbol func_ov006_0211b790
extern "C" {
void func_ov006_0211b790(char* scene)
{
    dScMgSound_c *self = (dScMgSound_c *)scene;
    if (self->unk_5626 == 0) {
        self->mState = 2;
        *(s16*)(scene + 0x5616) = 0x50;
        return;
    }
    {
        int count = 0;
        int i = 0;
        char* p = scene;
        do {
            if (*(u8*)(p + 0x50f5) != 0) {
                count++;
                break;
            }
            i++;
            p += 0x14;
        } while (i < 0xa);
        if (count != 0) return;
        *(s16*)(scene + 0x5616) = 0x50;
        self->mState = 2;
    }
}
}

// @symbol func_ov006_0211b654
extern "C" {
void func_ov006_0211b654(struct Mgr* scene, int n)
{
    int k;
    int i;
    u8* g = data_ov006_0212ee30;
    int* s00 = (int*)((char*)scene + n * 0x14 + 0x50e8);
    int* s04 = (int*)((char*)scene + n * 0x14 + 0x50ec);

    for (k = 0; k < 3; g++, k++) {
        for (i = 0; i < 30; i++) {
            if (scene->slot[i].f1c == 0) {
                scene->slot[i].f1c = 1;
                scene->slot[i].f00 = *s00;
                int* d04 = (int*)(((int)scene + i * 0x24 + 0x51b4));
                *d04 = *s04 - 0x8000;
                u16* d18 = (u16*)(((int)scene + i * 0x24 + 0x51c8));
                scene->slot[i].f10 = *d04;
                scene->slot[i].f1e = *g;
                scene->slot[i].f22 = *g;
                *d18 = 0;
                if (*(u8*)((char*)scene + n * 0x14 + 0x50f8) == 7) {
                    if (k >= 3) { *d18 = 0x10; } else { *d18 = 8; }
                }
                scene->slot[i].f14 = 0x1000;
                scene->slot[i].f08 = 0;
                scene->slot[i].f0c = 0;
                scene->slot[i].f20 = 0;
                scene->slot[i].f21 = 0;
                break;
            }
        }
    }
}
}

// @symbol func_ov006_0211b5e0
extern "C" {
extern Ent data_ov006_02142e20[];
void func_ov006_0211b5e0(char* scene){
  int i=0;
  char* r5=scene;
  do{
    if(*(unsigned char*)(r5+0x51cc)!=0){
      int idx=*(unsigned char*)(r5+0x51d0);
      Ent* e=&data_ov006_02142e20[idx];
      int adj=e->b;
      char* obj=scene+(adj>>1);
      int fn;
      if(adj&1){
        fn=*(int*)(*(int*)obj + e->a);
      } else {
        fn=e->a;
      }
      ((void(*)(void*,int))fn)(obj,i);
    }
    i++;
    r5+=0x24;
  }while(i<30);
}
}

// @symbol func_ov006_0211b590
extern "C" void func_ov006_0211b590(char *scene, int slot)
{
    unsigned char idx = *(unsigned char *)(scene + slot * 0x24 + 0x51d1);
    (((C *)scene)->*data_ov006_02142d38[idx])(slot);
}

// @symbol func_ov006_0211b4fc
extern "C" {
void func_ov006_0211b4fc(char *scene, int slot)
{
    unsigned char typeIndex;

    *(unsigned char*)(scene + slot * 0x24 + 0x5000 + 0x1cd) = 1;

    typeIndex = *(unsigned char*)(scene + 0x51d2 + slot * 0x24);
    *(int*)(scene + 0x51b0 + slot * 0x24) += data_ov006_0212eeb8[typeIndex];
    *(int*)(scene + slot * 0x24 + 0x5000 + 0x1bc) = 0;

    typeIndex = *(unsigned char*)(scene + 0x51d2 + slot * 0x24);
    *(int*)(scene + slot * 0x24 + 0x5000 + 0x1b8) = data_ov006_0212eea0[typeIndex];

    typeIndex = *(unsigned char*)(scene + 0x51d2 + slot * 0x24);
    *(unsigned char*)(scene + slot * 0x24 + 0x5000 + 0x1d3) = data_ov006_0212ee0c[typeIndex];
    *(unsigned char*)(scene + slot * 0x24 + 0x5000 + 0x1d1) = 1;
}
}

// @symbol func_ov006_0211b3ec
extern "C" {
void func_ov006_0211b3ec(char *scene, int slot)
{
    int dir;

    *(int *)(scene + 0x51b0 + slot * 0x24) += *(int *)(scene + 0x51b8 + slot * 0x24);

    dir = *(unsigned char *)(scene + 0x51d3 + slot * 0x24);
    if (dir == 0) {
        *(int *)(scene + 0x51b8 + slot * 0x24) -= 0x100;
        if (*(int *)(scene + 0x51b8 + slot * 0x24) < -0xc00) {
            *(int *)(scene + 0x51b8 + slot * 0x24) = -0xc00;
            *(unsigned char *)(scene + 0x51d3 + slot * 0x24) = 1;
        }
    } else if (dir != 0) {
        *(int *)(scene + 0x51b8 + slot * 0x24) += 0x100;
        if (*(int *)(scene + 0x51b8 + slot * 0x24) > 0xc00) {
            *(int *)(scene + 0x51b8 + slot * 0x24) = 0xc00;
            *(unsigned char *)(scene + 0x51d3 + slot * 0x24) = 0;
        }
    }

    *(int *)(scene + 0x51b4 + slot * 0x24) += *(int *)(scene + 0x51bc + slot * 0x24);
    *(int *)(scene + 0x51bc + slot * 0x24) -= 0xc0;

    if ((*(int *)(scene + 0x51c0 + slot * 0x24) - *(int *)(scene + 0x51b4 + slot * 0x24)) >> 12 >= 0x40) {
        *(unsigned char *)(scene + 0x51cc + slot * 0x24) = 0;
        *(unsigned char *)(scene + 0x51cd + slot * 0x24) = 0;
    }
}
}

// @symbol func_ov006_0211b3e8
extern "C" {
void func_ov006_0211b3e8(void)
{
}
}

// @symbol func_ov006_0211b398
extern "C" void func_ov006_0211b398(char *scene, int slot)
{
    unsigned char state = F1E(scene, slot);
    (((C*)scene)->*data_ov006_02142d50[state].pmf)(slot);
}

// @symbol func_ov006_0211b308
extern "C" {
void func_ov006_0211b308(char *scene, int slot)
{
  int off = slot * 0x24;
  *((unsigned char *) (((scene + off) + 0x5000) + 0x1cd)) = 1;
  *((int *) ((scene + 0x51b0) + off)) = (*((int *) ((scene + 0x51b0) + off))) + data_ov006_0212eed0[*((unsigned char *) ((scene + 0x51d2) + off))];
  if (off && off)
  {
  }
  *((int *) (((scene + off) + 0x5000) + 0x1bc)) = -0x2800;
  *((int *) (((scene + off) + 0x5000) + 0x1b8)) = data_ov006_0212eec4[*((unsigned char *) ((scene + 0x51d2) + off))];
  *((short *) (((scene + off) + 0x5100) + 0xc8)) = 0;
  *((unsigned char *) (((scene + off) + 0x5000) + 0x1d1)) = 1;
}
}

// @symbol func_ov006_0211b278
extern "C" {
void func_ov006_0211b278(char *scene, int slot){
    int o = slot * 0x24;
    *(int*)(scene + 0x51b0 + o) += *(int*)(scene + 0x51b8 + o);
    *(int*)(scene + 0x51b4 + o) += *(int*)(scene + 0x51bc + o);
    *(int*)(scene + 0x51bc + o) += 0x200;
    if(*(int*)(scene + 0x51bc + o) > 0){
        *(int*)(scene + 0x51bc + o) = 0;
        *(unsigned char*)(scene + 0x51d1 + o) = 2;
    }
}
}

// @symbol func_ov006_0211b1cc
extern "C" {
void func_ov006_0211b1cc(char* scene, int slot){
  int n = slot * 0x24;
  *(int*)(scene + 0x51b0 + n) = *(int*)(scene + 0x51b0 + n) + *(int*)(scene + 0x5000 + n + 0x1b8);
  *(int*)(scene + 0x51b4 + n) = *(int*)(scene + 0x51b4 + n) + *(int*)(scene + 0x51bc + n);
  *(int*)(scene + 0x51bc + n) = *(int*)(scene + 0x51bc + n) + 0x100;
  if (((*(int*)(scene + 0x5000 + n + 0x1c0) - *(int*)(scene + 0x51b4 + n)) >> 0xc) > 0x18)
    return;
  if (*(int*)(scene + 0x51bc + n) > 0) {
    *(unsigned char*)(scene + 0x5000 + n + 0x1cc) = 0;
    *(unsigned char*)(scene + 0x5000 + n + 0x1cd) = 0;
  }
}
}

// @symbol func_ov006_0211b17c
extern "C" void func_ov006_0211b17c(char *scene, int slot) {
    unsigned char state = *(unsigned char*)((char*)scene + slot * 0x24 + 0x51d1);
    (((C*)scene)->*data_ov006_02142d80[state].pmf)(slot);
}

// @symbol func_ov006_0211b0ec
extern "C" {
void func_ov006_0211b0ec(char *scene, int slot)
{
    int o = slot * 0x24;
    char *bIdx = scene + 0x51d2;
    char *bVal = scene + 0x51b0;
    char *b50 = (scene + o) + 0x5000;
    unsigned char idx;
    *((unsigned char *)(((scene + (o & 0xFFFFFFFFFFFFFFFFu)) + 0x5000) + 0x1cd)) = 1;
    idx = *((unsigned char *)(bIdx + o));
    *((int *)(bVal + o)) =
        (*((int *)(bVal + o))) + data_ov006_0212eef4[idx];
    *((int *)(((scene + o) + 0x5000) + 0x1bc)) = -0x2000;
    *((int *)(b50 + 0x1b8)) =
        data_ov006_0212eee8[*((unsigned char *)(bIdx + o))];
    *((short *)((scene + o) + 0x51c8)) = 0;
    *((unsigned char *)(((scene + o) + 0x5000) + 0x1d1)) = 1;
}
}

// @symbol func_ov006_0211b05c
extern "C" {
void func_ov006_0211b05c(char *scene, int slot){
    int o = slot * 0x24;
    *(int*)(scene + 0x51b0 + o) += *(int*)(scene + 0x51b8 + o);
    *(int*)(scene + 0x51b4 + o) += *(int*)(scene + 0x51bc + o);
    *(int*)(scene + 0x51bc + o) += 0x100;
    if(*(int*)(scene + 0x51bc + o) > 0){
        *(int*)(scene + 0x51bc + o) = 0;
        *(unsigned char*)(scene + 0x51d1 + o) = 2;
    }
}
}

// @symbol func_ov006_0211afb0
extern "C" {
void func_ov006_0211afb0(char* scene, int slot){
  int n = slot * 0x24;
  *(int*)(scene + 0x51b0 + n) = *(int*)(scene + 0x51b0 + n) + *(int*)(scene + 0x5000 + n + 0x1b8);
  *(int*)(scene + 0x51b4 + n) = *(int*)(scene + 0x51b4 + n) + *(int*)(scene + 0x51bc + n);
  *(int*)(scene + 0x51bc + n) = *(int*)(scene + 0x51bc + n) + 0x100;
  if (((*(int*)(scene + 0x5000 + n + 0x1c0) - *(int*)(scene + 0x51b4 + n)) >> 0xc) > 0x10)
    return;
  if (*(int*)(scene + 0x51bc + n) > 0) {
    *(unsigned char*)(scene + 0x5000 + n + 0x1cc) = 0;
    *(unsigned char*)(scene + 0x5000 + n + 0x1cd) = 0;
  }
}
}

// @symbol func_ov006_0211af60
extern "C" void func_ov006_0211af60(char *scene, int slot) {
    unsigned char state = *(unsigned char*)((char*)scene + slot * 0x24 + 0x51d1);
    (((C*)scene)->*data_ov006_02142dc8[state].pmf)(slot);
}

// @symbol func_ov006_0211aed0
extern "C" {
void func_ov006_0211aed0(char *scene, int slot)
{
  int new_var;
  int o = slot * 0x24;
  char *bIdx = scene + 0x51d2;
  char *bVal = scene + 0x51b0;
  char *b50 = (scene + o) + 0x5000;
  unsigned char idx;
  *((unsigned char *) (b50 + 0x1cd)) = 1;
  idx = *((unsigned char *) (bIdx + o));
  new_var = (*((int *) (bVal + o))) + data_ov006_0212ef18[idx];
  *((int *) (bVal + o)) = *((int *) (bVal + o));
  if (((!o) && (!o)) && (!o))
  {
  }
  *((int *) (bVal + o)) = new_var;
  *((int *) (((scene + o) + 0x5000) + 0x1bc)) = -0x4800;
  *((int *) (((scene + o) + 0x5000) + 0x1b8)) = data_ov006_0212ef24[*((unsigned char *) (bIdx + o))];
  *((short *) ((scene + o) + 0x51c8)) = 0;
  *((unsigned char *) (((scene + o) + 0x5000) + 0x1d1)) = 1;
}
}

// @symbol func_ov006_0211ae40
extern "C" {
void func_ov006_0211ae40(char *scene, int slot){
    int o = slot * 0x24;
    *(int*)(scene + 0x51b0 + o) += *(int*)(scene + 0x51b8 + o);
    *(int*)(scene + 0x51b4 + o) += *(int*)(scene + 0x51bc + o);
    *(int*)(scene + 0x51bc + o) += 0x400;
    if(*(int*)(scene + 0x51bc + o) > 0){
        *(int*)(scene + 0x51bc + o) = 0;
        *(unsigned char*)(scene + 0x51d1 + o) = 2;
    }
}
}

// @symbol func_ov006_0211ad94
extern "C" {
void func_ov006_0211ad94(char* scene, int slot){
  int n = slot * 0x24;
  *(int*)(scene + 0x51b0 + n) = *(int*)(scene + 0x51b0 + n) + *(int*)(scene + 0x5000 + n + 0x1b8);
  *(int*)(scene + 0x51b4 + n) = *(int*)(scene + 0x51b4 + n) + *(int*)(scene + 0x51bc + n);
  *(int*)(scene + 0x51bc + n) = *(int*)(scene + 0x51bc + n) + 0x200;
  if (((*(int*)(scene + 0x5000 + n + 0x1c0) - *(int*)(scene + 0x51b4 + n)) >> 0xc) > 0x20)
    return;
  if (*(int*)(scene + 0x51bc + n) > 0) {
    *(unsigned char*)(scene + 0x5000 + n + 0x1cc) = 0;
    *(unsigned char*)(scene + 0x5000 + n + 0x1cd) = 0;
  }
}
}

// @symbol func_ov006_0211ad44
extern "C" void func_ov006_0211ad44(char *scene, int slot) {
    unsigned char state = *(unsigned char*)((char*)scene + slot * 0x24 + 0x51d1);
    (((C*)scene)->*data_ov006_02142de0[state].pmf)(slot);
}

// @symbol func_ov006_0211ad00
extern "C" {
void func_ov006_0211ad00(char *scene, int slot) {
    scene += slot * 0x24;
    *(int*)(scene + 0x51c4) = 0x3000;
    *(unsigned char*)(scene + 0x51cd) = 1;
    *(int*)(scene + 0x51bc) = -0x1000;
    *(int*)(scene + 0x51b8) = data_ov006_0212ee40[*(unsigned char*)(scene + 0x51d2)];
    *(unsigned char*)(scene + 0x51d1) = 1;
}
}

// @symbol func_ov006_0211ac30
extern "C" {
void func_ov006_0211ac30(char *self, int slot)
{
    int i = slot * 0x24;
    *(int*)(self + 0x51b0 + i) = *(int*)(self + 0x51b0 + i) + *(int*)(self + 0x51b8 + i);
    *(int*)(self + 0x51b4 + i) = *(int*)(self + 0x51b4 + i) + *(int*)(self + 0x51bc + i);
    if (*(int*)(self + 0x51c4 + i) > 0x800) {
        *(int*)(self + 0x51c4 + i) = *(int*)(self + 0x51c4 + i) - 0x100;
        if (*(int*)(self + 0x51c4 + i) < 0x800)
            *(int*)(self + 0x51c4 + i) = 0x800;
    }
    *(int*)(self + 0x51bc + i) = *(int*)(self + 0x51bc + i) - 0x20;
    {
        int d = (*(int*)(self + 0x51c0 + i) - *(int*)(self + 0x51b4 + i)) >> 12;
        if (d >= 0x38) {
            *(unsigned char*)(self + 0x51cc + i) = 0;
            *(unsigned char*)(self + 0x51cd + i) = 0;
        }
    }
}
}

// @symbol func_ov006_0211ac2c
extern "C" {
void func_ov006_0211ac2c(void)
{
}
}

// @symbol func_ov006_0211abdc
extern "C" void func_ov006_0211abdc(char *scene, int slot) {
    unsigned char state = *(unsigned char*)((char*)scene + slot * 0x24 + 0x51d1);
    (((C*)scene)->*data_ov006_02142cd8[state].pmf)(slot);
}

// @symbol func_ov006_0211ab80
extern "C" {
void func_ov006_0211ab80(char *scene, int slot) {
    int n = slot * 0x24;
    char *base = scene + 0x51b0;
    char *p = scene + n;
    *(unsigned char *)(p + 0x51cd) = 1;
    unsigned char k = *(unsigned char *)(p + 0x51d2);
    *(int *)(base + n) = *(int *)(base + n) + data_ov006_0212ee7c[k];
    *(int *)(p + 0x51bc) = -0x4000;
    *(unsigned char *)(p + 0x51d1) = 1;
}
}

// @symbol func_ov006_0211ab0c
extern "C" {
void func_ov006_0211ab0c(char* scene, int slot)
{
    int off = slot * 0x24;
    *(int*)(scene + 0x51b4 + off) = *(int*)(scene + 0x51b4 + off) + *(int*)(scene + 0x51bc + off);
    *(int*)(scene + 0x51bc + off) = *(int*)(scene + 0x51bc + off) + 0x400;
    if (*(int*)(scene + 0x51bc + off) > 0) {
        *(int*)(scene + 0x51bc + off) = 0;
        *(unsigned char*)(scene + off + 0x5000 + 0x1d1) = 2;
    }
}
}

// @symbol func_ov006_0211aa94
extern "C" {
void func_ov006_0211aa94(BigObj *self, int slot) {
    self->arr[slot].b4 += self->arr[slot].bc;
    self->arr[slot].bc += 0x100;
    if (((self->arr[slot].c0 - self->arr[slot].b4) >> 12) <= 0x18) {
        self->arr[slot].cc = 0;
        self->arr[slot].cd = 0;
    }
}
}

// @symbol func_ov006_0211aa44
extern "C" void func_ov006_0211aa44(char *scene, int slot) {
    unsigned char state = *(unsigned char*)((char*)scene + slot * 0x24 + 0x51d1);
    (((C*)scene)->*data_ov006_02142cf0[state].pmf)(slot);
}

// @symbol func_ov006_0211a9fc
/* Activates one row of the 0x24-stride array at 0x51b8: sets its flags,
   clears its counters and loads its duration from data_ov006_0212ee70. */
extern "C" {
void func_ov006_0211a9fc(struct NoteObj *self, int slot)
{
    self->entries[slot].flagA = 1;
    self->entries[slot].counter = 0;
    self->entries[slot].value = data_ov006_0212ee70[self->entries[slot].type];
    self->entries[slot].flagB = 1;
    self->entries[slot].halfword = 0;
}
}

// @symbol func_ov006_0211a910
extern "C" {
void func_ov006_0211a910(char* scene, int slot)
{
    int n = slot * 0x24;
    int v;
    *(int*)(scene + 0x51b0 + n) = *(int*)(scene + 0x51b0 + n) + *(int*)(scene + 0x51b8 + n);
    *(int*)(scene + 0x51b4 + n) = *(int*)(scene + 0x51b4 + n) + *(int*)(scene + 0x51bc + n);
    v = *(int*)(scene + 0x51b8 + n);
    if (v > 0x20) {
        *(int*)(scene + 0x51b8 + n) = v - 0x20;
    } else if (v < -0x20) {
        *(int*)(scene + 0x51b8 + n) = v + 0x20;
    }
    *(int*)(scene + 0x51bc + n) = *(int*)(scene + 0x51bc + n) - 0x40;
    *(unsigned short*)(scene + 0x51c8 + n) = *(unsigned short*)(scene + 0x51c8 + n) + 1;
    if (*(unsigned short*)(scene + 0x51c8 + n) >= 0x28) {
        *(unsigned short*)(scene + 0x51c8 + n) = 0;
        *(unsigned char*)(scene + n + 0x5000 + 0x1d1) = 2;
        *(unsigned char*)(scene + n + 0x5000 + 0x1d3) = 0;
        *(int*)(scene + 0x51b8 + n) = 0xc00;
    }
}
}

// @symbol func_ov006_0211a7fc
extern "C" {
void func_ov006_0211a7fc(char *scene, int slot)
{
    int n = slot * 0x24;
    int f;
    *(int *)(scene + 0x51b0 + n) = *(int *)(scene + 0x51b0 + n) + *(int *)(scene + 0x51b8 + n);
    *(int *)(scene + 0x51b4 + n) = *(int *)(scene + 0x51b4 + n) + *(int *)(scene + 0x51bc + n);
    *(int *)(scene + 0x51bc + n) = *(int *)(scene + 0x51bc + n) - 0x40;
    f = *(unsigned char *)(scene + 0x51d3 + n);
    if (f == 0) {
        *(int *)(scene + 0x51b8 + n) -= 0x100;
        if (*(int *)(scene + 0x51b8 + n) < -0xc00) {
            *(int *)(scene + 0x51b8 + n) = -0xc00;
            *(unsigned char *)(scene + 0x51d3 + n) = 1;
        }
    } else if (f != 0) {
        *(int *)(scene + 0x51b8 + n) += 0x100;
        if (*(int *)(scene + 0x51b8 + n) > 0xc00) {
            *(int *)(scene + 0x51b8 + n) = 0xc00;
            *(unsigned char *)(scene + 0x51d3 + n) = 0;
        }
    }
    if ((*(int *)(scene + 0x51c0 + n) - *(int *)(scene + 0x51b4 + n)) >> 12 >= 0x30) {
        *(unsigned char *)(scene + 0x51cc + n) = 0;
        *(unsigned char *)(scene + 0x51cd + n) = 0;
    }
}
}

// @symbol func_ov006_0211a7ac
extern "C" void func_ov006_0211a7ac(char *scene, int slot) {
    unsigned char state = *(unsigned char*)((char*)scene + slot * 0x24 + 0x51d1);
    (((C*)scene)->*data_ov006_02142d20[state].pmf)(slot);
}

// @symbol func_ov006_0211a714
extern "C" {
void func_ov006_0211a714(char* scene, int slot) {
  int idx = slot * 0x24;
  unsigned short v = *(unsigned short*)(scene + 0x51c8 + idx);
  if (v != 0) {
    *(short*)(scene + 0x51c8 + idx) = v - 1;
    if (*(short*)(scene + 0x51c8 + idx) < 0)
      *(short*)(scene + 0x51c8 + idx) = 0;
    return;
  }
  *(unsigned char*)(scene + 0x51cd + idx) = 1;
  *(int*)(scene + 0x51b0 + idx) += data_ov006_0212ee94[*(unsigned char*)(scene + 0x51d2 + idx)];
  *(int*)(scene + 0x51bc + idx) = -0x4000;
  *(unsigned char*)(scene + 0x51d1 + idx) = 1;
}
}

// @symbol func_ov006_0211a69c
extern "C" {
void func_ov006_0211a69c(char *scene, int slot)
{
    int off = slot * 0x24;
    char *pA = scene + 0x51bc;
    char *pB = scene + 0x51b4;
    int a = *(int *)(pA + off);
    int b = *(int *)(pB + off);
    *(int *)(pB + off) = b + a;
    a = *(int *)(pA + off);
    *(int *)(pA + off) = a + 0x400;
    {
        int d = *(int *)(scene + off + 0x51c0) - *(int *)(pB + off);
        d >>= 12;
        if (d >= 0x20) {
            *(unsigned char *)(scene + off + 0x51cc) = 0;
            *(unsigned char *)(scene + off + 0x51cd) = 0;
        }
    }
}
}

// @symbol func_ov006_0211a698
extern "C" {
void func_ov006_0211a698(void)
{
}
}

// @symbol func_ov006_0211a648
extern "C" void func_ov006_0211a648(char *scene, int slot) {
    unsigned char state = *(unsigned char*)((char*)scene + slot * 0x24 + 0x51d1);
    (((C*)scene)->*data_ov006_02142d68[state].pmf)(slot);
}

// @symbol func_ov006_0211a5ec
extern "C" {
void func_ov006_0211a5ec(char *scene, int slot) {
    int n = slot * 0x24;
    char *base = scene + 0x51b0;
    char *p = scene + n;
    *(unsigned char *)(p + 0x51cd) = 1;
    unsigned char k = *(unsigned char *)(p + 0x51d2);
    *(int *)(base + n) = *(int *)(base + n) + data_ov006_0212eedc[k];
    *(int *)(p + 0x51bc) = -0x4000;
    *(unsigned char *)(p + 0x51d1) = 1;
}
}

// @symbol func_ov006_0211a578
extern "C" {
void func_ov006_0211a578(char* scene, int slot)
{
    int off = slot * 0x24;
    *(int*)(scene + 0x51b4 + off) = *(int*)(scene + 0x51b4 + off) + *(int*)(scene + 0x51bc + off);
    *(int*)(scene + 0x51bc + off) = *(int*)(scene + 0x51bc + off) + 0x400;
    if (*(int*)(scene + 0x51bc + off) > 0) {
        *(int*)(scene + 0x51bc + off) = 0;
        *(unsigned char*)(scene + off + 0x5000 + 0x1d1) = 2;
    }
}
}

// @symbol func_ov006_0211a500
extern "C" {
void func_ov006_0211a500(BallObj *self, int slot) {
    self->arr[slot].a += self->arr[slot].b;
    self->arr[slot].b -= 0x60;
    if ((self->arr[slot].c - self->arr[slot].a) >> 0xc >= 0x3c) {
        self->arr[slot].f18 = 0;
        self->arr[slot].f19 = 0;
    }
}
}

// @symbol func_ov006_0211a4b0
extern "C" void func_ov006_0211a4b0(char *scene, int slot)
{
    unsigned char idx = *(unsigned char *)(scene + slot * 0x24 + 0x51d1);
    (((C *)scene)->*data_ov006_02142d98[idx])(slot);
}

// @symbol func_ov006_0211a420
extern "C" {
void func_ov006_0211a420(char *scene, int slot)
{
  int o = slot * 0x24;
  unsigned char idx;
  char *bIdx = scene + 0x51d2;
  char *bVal;
  char *b50 = (scene + o) + 0x5000;
  *((unsigned char *) (b50 + 0x1cd)) = 1;
  bVal = scene + 0x51b0;
  idx = *((unsigned char *) (bIdx + o));
  *((int *) (bVal + o)) = (*((int *) (bVal + o))) + data_ov006_0212ee64[idx];
  if ((o && o) && o)
  {
  }
  if (1)
  {
    *((int *) (((scene + o) + 0x5000) + 0x1bc)) = -0x3800;
    *((int *) (((scene + o) + 0x5000) + 0x1b8)) = data_ov006_0212ef00[*((unsigned char *) (bIdx + o))];
    *((short *) (0x51c8 + (scene + o))) = 0;
    *((unsigned char *) (((scene + o) + 0x5000) + 0x1d1)) = 1;
  }
}
}

// @symbol func_ov006_0211a388
extern "C" {
void func_ov006_0211a388(char* scene, int slot) {
  int idx = slot * 0x24;
  *(int*)(scene + 0x51b0 + idx) += *(int*)(scene + 0x51b8 + idx);
  *(int*)(scene + 0x51b4 + idx) += *(int*)(scene + 0x51bc + idx);
  *(int*)(scene + 0x51bc + idx) += 0x200;
  if (*(int*)(scene + 0x51bc + idx) <= 0) return;
  *(int*)(scene + 0x51bc + idx) = 0;
  *(unsigned char*)(scene + 0x51d1 + idx) = 2;
  *(short*)(scene + 0x51c8 + idx) = 0;
}
}

// @symbol func_ov006_0211a314
extern "C" {
void func_ov006_0211a314(char* scene, int slot)
{
    int off = slot * 0x24;
    int v = *(int*)(scene + 0x51c4 + off);
    if (v > 0x800) {
        *(int*)(scene + 0x51c4 + off) = v - 0x60;
        if (*(int*)(scene + 0x51c4 + off) < 0x800)
            *(int*)(scene + 0x51c4 + off) = 0x800;
    }
    *(unsigned short*)(scene + 0x51c8 + off) = *(unsigned short*)(scene + 0x51c8 + off) + 1;
    if (*(unsigned short*)(scene + 0x51c8 + off) >= 0x20) {
        *(unsigned char*)(scene + off + 0x5000 + 0x1cc) = 0;
        *(unsigned char*)(scene + off + 0x5000 + 0x1cd) = 0;
    }
}
}

// @symbol func_ov006_0211a2c4
extern "C" void func_ov006_0211a2c4(C* scene, int slot){
  unsigned char sel = *((unsigned char*)((char*)scene + slot*0x24 + 0x51d1));
  (scene->*data_ov006_02142cc0[sel])(slot);
}

// @symbol func_ov006_0211a234
extern "C" {
void func_ov006_0211a234(char *scene, int slot)
{
  int o = slot * 0x24;
  char *bIdx = scene + 0x51d2;
  char *bVal = scene + 0x51b0;
  char *b50;
  unsigned char idx;
  *((unsigned char *) (((scene + o) + 0x5000) + 0x1cd)) = 1;
  idx = *((unsigned char *) (bIdx + o));
  *((int *) (bVal + o)) = (*((int *) (bVal + o))) + data_ov006_0212ee58[idx];
  if ((slot && slot) != 0)
  {
  }
  *((int *) (((scene + o) + 0x5000) + 0x1bc)) = -0x3200;
  *((int *) (((scene + o) + 0x5000) + 0x1b8)) = data_ov006_0212ee4c[*((unsigned char *) (bIdx + o))];
  *((short *) ((scene + o) + 0x51c8)) = 0;
  *((unsigned char *) (((scene + o) + 0x5000) + 0x1d1)) = 1;
}
}

// @symbol func_ov006_0211a19c
extern "C" {
void func_ov006_0211a19c(char* scene, int slot) {
  int idx = slot * 0x24;
  *(int*)(scene + 0x51b0 + idx) += *(int*)(scene + 0x51b8 + idx);
  *(int*)(scene + 0x51b4 + idx) += *(int*)(scene + 0x51bc + idx);
  *(int*)(scene + 0x51bc + idx) += 0x200;
  if (*(int*)(scene + 0x51bc + idx) <= 0) return;
  *(int*)(scene + 0x51bc + idx) = 0;
  *(unsigned char*)(scene + 0x51d1 + idx) = 2;
  *(short*)(scene + 0x51c8 + idx) = 0;
}
}

// @symbol func_ov006_0211a128
extern "C" {
void func_ov006_0211a128(char* scene, int slot)
{
    int off = slot * 0x24;
    int v = *(int*)(scene + 0x51c4 + off);
    if (v > 0x800) {
        *(int*)(scene + 0x51c4 + off) = v - 0x70;
        if (*(int*)(scene + 0x51c4 + off) < 0x800)
            *(int*)(scene + 0x51c4 + off) = 0x800;
    }
    *(unsigned short*)(scene + 0x51c8 + off) = *(unsigned short*)(scene + 0x51c8 + off) + 1;
    if (*(unsigned short*)(scene + 0x51c8 + off) >= 0x20) {
        *(unsigned char*)(scene + off + 0x5000 + 0x1cc) = 0;
        *(unsigned char*)(scene + off + 0x5000 + 0x1cd) = 0;
    }
}
}

// @symbol func_ov006_0211a0d8
extern "C" void func_ov006_0211a0d8(CFull* scene, int slot){
  (scene->*(data_ov006_02142d08[scene->arr[slot].idx].pmf))(slot);
}

// @symbol func_ov006_0211a048
extern "C" {
void func_ov006_0211a048(char *scene, int slot)
{
  int off = slot * 0x24;
  *((unsigned char *) (((scene + off) + 0x5000) + 0x1cd)) = 1;
  *((int *) ((scene + 0x51b0) + off)) = (*((int *) ((scene + 0x51b0) + off))) + data_ov006_0212eeac[*((unsigned char *) ((scene + 0x51d2) + off))];
  if (off && off)
  {
  }
  *((int *) (((scene + off) + 0x5000) + 0x1bc)) = -0x2800;
  *((int *) (((scene + off) + 0x5000) + 0x1b8)) = data_ov006_0212ee88[*((unsigned char *) ((scene + 0x51d2) + off))];
  *((short *) (((scene + off) + 0x5100) + 0xc8)) = 0;
  *((unsigned char *) (((scene + off) + 0x5000) + 0x1d1)) = 1;
}
}

// @symbol func_ov006_02119fb0
extern "C" {
void func_ov006_02119fb0(char* scene, int slot) {
  int idx = slot * 0x24;
  *(int*)(scene + 0x51b0 + idx) += *(int*)(scene + 0x51b8 + idx);
  *(int*)(scene + 0x51b4 + idx) += *(int*)(scene + 0x51bc + idx);
  *(int*)(scene + 0x51bc + idx) += 0x200;
  if (*(int*)(scene + 0x51bc + idx) <= 0) return;
  *(int*)(scene + 0x51bc + idx) = 0;
  *(unsigned char*)(scene + 0x51d1 + idx) = 2;
  *(short*)(scene + 0x51c8 + idx) = 0;
}
}

// @symbol func_ov006_02119f3c
extern "C" {
void func_ov006_02119f3c(char* scene, int slot)
{
    int off = slot * 0x24;
    int v = *(int*)(scene + 0x51c4 + off);
    if (v > 0xa80) {
        *(int*)(scene + 0x51c4 + off) = v - 0x60;
        if (*(int*)(scene + 0x51c4 + off) < 0xa80)
            *(int*)(scene + 0x51c4 + off) = 0xa80;
    }
    *(unsigned short*)(scene + 0x51c8 + off) = *(unsigned short*)(scene + 0x51c8 + off) + 1;
    if (*(unsigned short*)(scene + 0x51c8 + off) >= 0x18) {
        *(unsigned char*)(scene + off + 0x5000 + 0x1cc) = 0;
        *(unsigned char*)(scene + off + 0x5000 + 0x1cd) = 0;
    }
}
}

// @symbol func_ov006_02119eec
extern "C" void func_ov006_02119eec(CFull* scene, int slot){
  (scene->*(data_ov006_02142db0[scene->arr[slot].idx].pmf))(slot);
}

// @symbol func_ov006_02119e5c
extern "C" {
void func_ov006_02119e5c(char *scene, int slot)
{
  int o = slot * 0x24;
  char *bIdx = scene + 0x51d2;
  char *bVal = scene + 0x51b0;
  char *b50 = (scene + o) + 0x5000;
  unsigned char idx;
  *((unsigned char *) (((scene + (o & 0xFFFFFFFFFFFFFFFFu)) + 0x5000) + 0x1cd)) = 1;
  idx = *((unsigned char *) (bIdx + o));
  *((int *) (bVal + o)) = (*((int *) (bVal + o))) + data_ov006_0212ef30[idx];
  *((int *) (((scene + o) + 0x5000) + 0x1bc)) = -0x2400;
  *((int *) (b50 + 0x1b8)) = data_ov006_0212ef0c[*((unsigned char *) (bIdx + o))];
  *((short *) ((scene + o) + 0x51c8)) = 0;
  *((unsigned char *) (((scene + o) + 0x5000) + 0x1d1)) = 1;
}
}

// @symbol func_ov006_02119dc4
extern "C" {
void func_ov006_02119dc4(char* scene, int slot) {
  int idx = slot * 0x24;
  *(int*)(scene + 0x51b0 + idx) += *(int*)(scene + 0x51b8 + idx);
  *(int*)(scene + 0x51b4 + idx) += *(int*)(scene + 0x51bc + idx);
  *(int*)(scene + 0x51bc + idx) += 0x200;
  if (*(int*)(scene + 0x51bc + idx) <= 0) return;
  *(int*)(scene + 0x51bc + idx) = 0;
  *(unsigned char*)(scene + 0x51d1 + idx) = 2;
  *(short*)(scene + 0x51c8 + idx) = 0;
}
}

// @symbol func_ov006_02119d50
extern "C" {
void func_ov006_02119d50(char* scene, int slot)
{
    int off = slot * 0x24;
    int v = *(int*)(scene + 0x51c4 + off);
    if (v > 0xd80) {
        *(int*)(scene + 0x51c4 + off) = v - 0x60;
        if (*(int*)(scene + 0x51c4 + off) < 0xd80)
            *(int*)(scene + 0x51c4 + off) = 0xd80;
    }
    *(unsigned short*)(scene + 0x51c8 + off) = *(unsigned short*)(scene + 0x51c8 + off) + 1;
    if (*(unsigned short*)(scene + 0x51c8 + off) >= 0x18) {
        *(unsigned char*)(scene + off + 0x5000 + 0x1cc) = 0;
        *(unsigned char*)(scene + off + 0x5000 + 0x1cd) = 0;
    }
}
}

// @symbol func_ov006_02119c74
extern "C" {
void func_ov004_020b023c(void* a0, int a1, int a2, int a3, void* a4);

void func_ov006_02119c74(void* scene)
{
    char* p=(char*)scene;
    int i;
    int tb = data_02082214[0];
    long long ta = data_02082214[1];

    for (i = 0; i < 30; i++) {
        if (*(unsigned char*)(p + 0x51cd)) {
            int m[4];
            long long vv = *(int*)(p + 0x51c4);
            int x = *(int*)(p + 0x51b0);
            int y = *(int*)(p + 0x51b4);
            int a = (int)((ta * vv + 0x800) >> 12);
            int b = (int)(((long long)tb * (int)vv + 0x800) >> 12);
            m[0] = a;
            m[3] = a;
            m[1] = b;
            m[2] = -b;
            func_ov004_020b023c(data_ov006_0213f6f0[*(unsigned char*)(p + 0x51ce)],
                                (x >> 12) - 8, (y >> 12) - 0x10, -1, m);
        }
        tb = data_02082214[0];
        p += 0x24;
    }
}
}

// @symbol func_ov006_02119bdc
extern "C" {
extern void* data_ov006_0213f730[];
void func_ov006_02119bdc(void* scene) {
  char* c=(char*)scene;
  int i;
  char* o = c;
  for (i = 0; i < 10; i++) {
    if (*(unsigned char*)(o + 0x50f6) != 0 && *(unsigned char*)(o + 0x50f5) != 0) {
      int a1 = *(int*)(o + 0x50e8) >> 12;
      int a2 = *(int*)(o + 0x50ec) >> 12;
      int a4 = 0;
      if (*(int*)(c + 0x5608) != 1) a4 = 1;
      unsigned char sel = *(unsigned char*)(o + 0x50f7);
      Hud_RenderSprite((int)data_ov006_0213f730[sel], a1, a2, -1, a4);
    }
    o += 0x14;
  }
}
}

// @symbol func_ov006_02119bc4
extern "C" {
void func_ov006_02119bc4(void *scene)
{
    func_ov004_020b2574(*(unsigned char *)((char *)scene + 0x5000 + 0x626), 1);
}
}

// @symbol func_ov006_02119ba4
extern "C" {
void func_ov006_02119ba4(char *scene)
{
    dScMgSound_c *self = (dScMgSound_c *)scene;
    self->mSpriteA.active = 1;
    self->mSpriteA.timer = 0;
    self->mSpriteA.frame = 0;
}
}

// @symbol func_ov006_02119b00
extern "C" {
void func_ov006_02119b00(char *scene)
{
    dScMgSound_c *self = (dScMgSound_c *)scene;
    if (self->mSpriteA.active == 0)
        return;
    if (self->mSpriteA.frame >= 2)
        return;
    {
        u16 *p = (u16 *)(((int)scene + 0x55f0));
        *p = *p + 1;
    }
    if (self->mSpriteA.timer < data_ov006_0212ee28[self->mSpriteA.frame])
        return;
    self->mSpriteA.timer = 0;
    {
        u8 *q = (u8 *)(((int)scene + 0x55f6));
        *q = *q + 1;
    }
}
}

// @symbol func_ov006_02119aa8
extern "C" {
void func_ov006_02119aa8(void*scene){
  char*c=(char*)scene;
  if(*(unsigned char*)(c+0x5000+0x5f5)==0) return;
  int x=*(int*)(c+0x5000+0x5e8);
  int y=*(int*)(c+0x5000+0x5ec);
  Hud_RenderSprite(data_ov006_02138d28[*(unsigned char*)(c+0x5000+0x5f6)],
    x>>0xc, y>>0xc, -1, -1);
}
}

// @symbol func_ov006_02119a88
extern "C" {
void func_ov006_02119a88(char *scene)
{
    dScMgSound_c *self = (dScMgSound_c *)scene;
    self->mSpriteB.active = 1;
    self->mSpriteB.timer = 0;
    self->mSpriteB.frame = 0;
}
}

// @symbol func_ov006_02119a18
extern "C" {
void func_ov006_02119a18(char *scene)
{
    dScMgSound_c *self = (dScMgSound_c *)scene;
    if (self->mSpriteB.active == 0) return;
    if (self->mSpriteB.frame >= 3) return;
    *(unsigned short *)A(scene + 0x5600) += 1;
    if (self->mSpriteB.timer < data_ov006_0212ee38[self->mSpriteB.frame]) return;
    self->mSpriteB.timer = 0;
    *(unsigned char *)A(scene + 0x5606) += 1;
}
}

// @symbol func_ov006_021199c0
extern "C" {
void func_ov006_021199c0(void*scene){
  char*c=(char*)scene;
  if(*(unsigned char*)(c+0x5000+0x605)==0) return;
  int x=*(int*)(c+0x5000+0x5f8);
  int y=*(int*)(c+0x5000+0x5fc);
  Hud_RenderSprite(data_ov006_02137ae8[*(unsigned char*)(c+0x5000+0x606)],
    x>>0xc, y>>0xc, -1, -1);
}
}

// @symbol _ZN12dScMgSound_cD1Ev
// @symbol _ZN12dScMgSound_cD0Ev
/* Both destructor variants come from the inline body in dScMgSound_c.h.
   Declared first in the class, it gives D1 below D0, the order ov006 has. */
