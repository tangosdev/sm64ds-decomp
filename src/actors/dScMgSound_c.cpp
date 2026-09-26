//cpp
/* Boom-box minigame. Each round deals mNotes on the touch screen; a touch
 * plays that pitch and pops icons in mPops. The class is a
 * dScMgSingle3DBase_c (its RTTI is at 0x0213f6e4).
 *
 * The compiler emits .text in reverse source order, so the functions run
 * here from the highest address down. Do not reorder.
 *
 * Leftover: func_ and data_ helpers are still the linker names.
 * Leftover: func_ov006_02119bdc reads mState as the raw word at 0x5608.
 *   The member spelling in the note draw loop grows it by 8 bytes.
 * Leftover: func_ov006_0211c080's deal loop reaches mPairUses and
 *   mPairNotes as q + 0x561a and q + 0x561f, with q = scene + pair.
 *   Indexing the members by pair grows the function by 8 bytes.
 * Leftover: InitResources passes raw + 0x4660, the camera that
 *   dScMgSingle3DBase_c.h still leaves as padding.
 */

#include "dScMgSound_c.h"
#include "types.h"
#include "decl_common.h"

/* A cursor keeps the add-#0x5000 split on a walk that advances the scene
   pointer. Pointing the record itself at the slot pools the base instead. */
struct dMgSoundNoteCur {
    char pad[0x50e8];
    dMgSoundNote_c note;
};
struct dMgSoundPopCur {
    char pad[0x51b0];
    dMgSoundPop_c pop;
};

/* C is incomplete and CFull is complete on purpose: the two pointer-to-member
   types get different adjustments, and the dispatch tables need both. */
struct C;
typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };

struct Elem {
    unsigned char idx;
    char pad[0x23];
};
struct CFull {
    char pad[0x51d1];
    Elem arr[1];
};
typedef void (CFull::*PMFFull)(int);
struct EntryFull { PMFFull pmf; };

/* func_ov006_0211b9c8 calls Virtual8C through the scene vtable. */
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
void func_ov006_0211ba88(dScMgSound_c *base, int idx);
extern unsigned short data_ov006_0212ef4c[];
void func_ov006_0211b654(dScMgSound_c *m, int n);
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
extern "C" void *_ZN11dScMgBase_cC2Ev(void *);
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
    mTries = 3;
    mState = 1;
    mIntroTimer = 0x20;
    func_ov004_020b6808();
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
    mHudScore = 0;
    return 1;
}

// @symbol _ZN12dScMgSound_c8BehaviorEv
/* One frame of the round, by mState: 0 starts, 1 counts down mIntroTimer,
   2 plays the round and, when mResultTimer runs out, bumps the win counter
   (capped at 9999), 3 counts down a retry.
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
        if (mIntroTimer != 0) {
            (*(u16 *)(int)&mIntroTimer)--;
            if (mIntroTimer == 0) {
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
        if (mResultTimer != 0) {
            (*(u16 *)(int)&mResultTimer)--;
            if (mResultTimer == 0) {
                if (mTries != 0) {
                    mTable.mSuppressSound = 0;
                    func_ov006_020c2594((char *)&mTable);
                    if (mTries == 3)
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
                    mResultTimer = 0x20;
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
        if (mResultTimer != 0) {
            (*(u16 *)(int)&mResultTimer)--;
            if (*(s16 *)&mResultTimer <= 0) {
                mTable.mSuppressSound = 0;
                func_ov006_020c2440((char *)&mTable);
                func_ov004_020b0a54(0x12);
                mPromptEnabled = 0;
                mResultTimer = 0;
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

    mTries = 3;
    func_ov006_0211c080(raw);

    mIntroTimer = 0x20;
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);

    Sound::LoadAndSetMusic_Layer1(6);
}

// @symbol _ZN12dScMgSound_c9Virtual50Ev
/* Virtual50 is dScMgBase_c.h's placeholder name; what the slot does is not
   known yet. */
void dScMgSound_c::Virtual50()
{
    func_ov006_020c2594((char *)&mTable);
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
        dMgSoundNoteCur *note = (dMgSoundNoteCur *)b;
        note->note.x = 0;
        note->note.y = 0;
        note->note.timer = 0;
        note->note.state = 0;
        note->note.alive = 0;
        note->note.shown = 0;
        note->note.frame = 0;
        note->note.note = 0;
        b += 0x14;
    }

    for (cnt2 = 0; cnt2 < 5; cnt2++) {
        self->mPairUses[cnt2] = 0;
        self->mPairNotes[cnt2] = 0;
    }

    b = scene;
    for (i = 0; i < 0x1e; i++) {
        dMgSoundPopCur *pop = (dMgSoundPopCur *)b;
        pop->pop.x = 0;
        pop->pop.y = 0;
        pop->pop.velX = 0;
        pop->pop.velY = 0;
        pop->pop.scale = 0;
        pop->pop.timer = 0;
        pop->pop.active = 0;
        pop->pop.visible = 0;
        pop->pop.sprite = 0;
        pop->pop.unk_1f = 0;
        pop->pop.active = 0; /* stored twice, as in the ROM */
        pop->pop.state = 0;
        pop->pop.phase = 0;
        pop->pop.kind = 0;
        pop->pop.dir = 0;
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

    self->mQueue[1] = 0;
    self->mQueue[0] = self->mQueue[1];
    self->mQueueTimer = 0;
    self->mQueueLen = 0;
    self->mTouchCount = 0;
    self->mResultTimer = 0;

    func_ov006_020c2924((char *)&self->mTable);
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

    sel = self->unk_0bc;
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
        self->mPairNotes[0] = b;
        self->mPairNotes[1] = self->mPairNotes[0] + 1;
        if (self->mPairNotes[1] >= 3)
            self->mPairNotes[1] -= 3;
        self->mPairNotes[2] = self->mPairNotes[1] + 1;
        if (self->mPairNotes[2] >= 3)
            self->mPairNotes[2] -= 3;
        self->mPairNotes[0] += a * 3;
        self->mPairNotes[1] += a * 3;
        self->mPairNotes[2] += a * 3;
    } else {
        half = count >> 1;
        for (i = 0; i < half; i++) {
            int v = (u32)(RND * speed) >> 15;
            self->mPairNotes[i] = v;
            if (i != 0) {
                int dup;
                int j;
                for (;;) {
                    dup = 0;
                    j = 0;
                    for (; j < i; j++) {
                        if (self->mPairNotes[i] == self->mPairNotes[j]) {
                            dup = 1;
                            break;
                        }
                    }
                    if (dup == 0)
                        break;
                    self->mPairNotes[i] = (u32)(RND * speed) >> 15;
                }
            }
        }
    }
    k = self->mPattern;
    if (k == 1 || k == 4) {
        self->mBank = data_ov006_0212efb0[((u32)self->unk_0bc >> 2) & 3];
    }
    half = count >> 1;
    for (i = 0; i < half; i++) {
        self->mPairNotes[i] += data_ov006_0212ef9c[self->mPattern];
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
            dMgSoundNoteCur *note = (dMgSoundNoteCur *)p;
            t = data_ov006_0212ee20[self->mPattern];
            note->note.x = data_ov006_0213f6fc[t][xi] << 12;
            note->note.y = (*(u16 * volatile *)&data_ov006_0213f6fc[t])[zi] << 12;
            note->note.timer = 0;
            note->note.state = 0;
            note->note.alive = 1;
            note->note.shown = 1;
            note->note.frame = 0;
            note->note.checked = 0;
            do {
                q = scene + ((u32)(RND * half) >> 15);
                slot = (u8 *)LAUNDER(q + 0x561a);
            } while (*slot >= 2);
            note->note.note = *(u8 *)(q + 0x561f);
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
void func_ov006_0211bf44(dScMgSound_c* scene, int slot)
{
    u8 player;
    int offset;
    int active;
    int dx;
    int dy;

    if (scene->mState != 1) {
        return;
    }
    if (scene->mTouchCount >= 2) {
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

    dx = data_020a0dea[player * 4] - (scene->mNotes[slot].x >> 12);
    dy = data_020a0deb[player * 4] - (scene->mNotes[slot].y >> 12);

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

    scene->mNotes[slot].state = 1;
    scene->mNotes[slot].timer = 0;
    scene->mNotes[slot].frame = 0;
    scene->mTouchCount++;
    Sound::PlayBank2_2D(0x201);
}
}

// @symbol func_ov006_0211bc8c
/* Advances one note slot's animation. When the slot's last frame is
   reached its note goes on mQueue and is played; mPattern and Virtual8C
   choose between func_02012790, the bank handle mBank and
   Sound::PlayBank2_2D.
   func_ov006_0211b654 is passed `idx` even though it is already in r1:
   dropping it changes the register allocation of the queue block. */
extern "C" void func_ov006_0211bc8c(dScMgSound_c *self, int idx)
{
    u8 note;
    u8 instrument;

    self->mNotes[idx].timer = self->mNotes[idx].timer + 1;
    if (self->mNotes[idx].timer < data_ov006_0212ef3c[self->mNotes[idx].frame]) return;
    self->mNotes[idx].timer = 0;
    self->mNotes[idx].frame = self->mNotes[idx].frame + 1;
    if (self->mNotes[idx].frame <= 6) return;
    self->mNotes[idx].frame = 6;
    self->mNotes[idx].state = 2;

    self->mQueue[self->mQueueLen] = self->mNotes[idx].note + 1;
    self->mQueue[self->mQueueLen] |= idx << 8;
    self->mQueueLen++;
    self->mQueueTimer = 0x20;

    func_ov006_0211b654(self, idx);
    note = self->mNotes[idx].note;

    if (self->Virtual8C()) {
        instrument = self->mPattern;
        if (instrument == 0 || instrument == 2) {
            if (data_ov006_0213f794[note] == 2) {
                func_02012790(data_ov006_0213f7e8[note]);
            } else {
                func_02012174(self->mBank, data_ov006_0213f7e8[note]);
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
        instrument = self->mPattern;
        if (instrument == 0 || instrument == 2) {
            func_02012790(data_ov006_0213f7e8[note]);
        } else if (instrument == 1) {
            func_02012174(self->mBank, data_ov006_0213f7e8[note]);
        } else if (instrument == 4) {
            if (data_ov006_0213f794[note] == 2) {
                Sound::PlayBank2_2D(data_ov006_0213f7e8[note]);
            } else {
                func_02012174(self->mBank, data_ov006_0213f7e8[note]);
            }
        } else {
            Sound::PlayBank2_2D(data_ov006_0213f7e8[note]);
        }
    }

    if (self->mTouchCount == 1) func_ov006_020c2300((char *)&self->mTable);
}

// @symbol func_ov006_0211bc68
extern "C" {
void func_ov006_0211bc68(char* scene, int slot) {
    if (((dScMgSound_c *)scene)->mTouchCount == 0) {
        ((dScMgSound_c *)scene)->mNotes[slot].state = 3;
    }
}
}

// @symbol func_ov006_0211bbe0
extern "C" {
void func_ov006_0211bbe0(dScMgSound_c *scene, int slot){
    unsigned char b;
    scene->mNotes[slot].timer = (unsigned short)(scene->mNotes[slot].timer + 1);
    b = scene->mNotes[slot].frame;
    if (scene->mNotes[slot].timer < data_ov006_0212ef4c[b]) return;
    scene->mNotes[slot].timer = 0;
    scene->mNotes[slot].frame = (unsigned char)(scene->mNotes[slot].frame - 1);
    if (scene->mNotes[slot].frame == 0) scene->mNotes[slot].state = 0;
}
}

// @symbol func_ov006_0211ba88
/* Scrolls one note slot left by 0x10 per call and clears it once it has
   gone off screen. Its column comes from the per-mode tables
   data_ov006_0212ef5c and data_ov006_0212ef6c.
   Two spellings are load-bearing: Virtual8C is asked through `this` in
   C++, and the store at +0x0c is a byte. */
void func_ov006_0211ba88(dScMgSound_c *scene, int slot)
{
    int n;
    int i;
    int limit;

    scene->mNotes[slot].x -= 0x10000;

    if (scene->Virtual8C() != 0) {
        limit = data_ov006_0212ef5c[scene->mPattern];
    } else {
        limit = data_ov006_0212ef6c[scene->mPattern];
    }

    if (scene->mNotes[slot].checked == 0) {
        n = slot;
        if (slot >= limit) {
            do {
                n = n - limit;
            } while (n >= limit);
        }
        if (n == 0) {
            scene->mNotes[slot].checked = 1;
            return;
        }
        for (i = 0; i < n; i++) {
            int prev = slot - i - 1;
            if (scene->mNotes[prev].alive != 0) {
                if ((scene->mNotes[slot].x - scene->mNotes[prev].x) >> 0xc <= 4) {
                    scene->mNotes[slot].checked = 1;
                    scene->mNotes[prev].state = 4;
                }
            }
        }
    }

    if (scene->mNotes[slot].x >> 0xc > -0x18) return;

    scene->mNotes[slot].alive = 0;
    scene->mNotes[slot].shown = 0;
}

// @symbol func_ov006_0211b9c8
extern "C" void func_ov006_0211b9c8(char *scene) {
    SceneVt *obj = (SceneVt *)scene;
    int lr;
    int ip;
    int found;
    int off;
    if (obj->check()) {
        lr = data_ov006_0212ef7c[((dScMgSound_c *)obj)->mPattern];
    } else {
        lr = data_ov006_0212ef8c[((dScMgSound_c *)obj)->mPattern];
    }
    for (ip = 0, off = 0; ip < 2; ip++) {
        int sb;
        found = -1;
        for (sb = 0; sb < lr; sb++) {
            int e = sb + off;
            if (((dScMgSound_c *)obj)->mNotes[e].alive != 0)
                found = e;
        }
        if (found != -1) {
            ((dScMgSound_c *)obj)->mNotes[found].state = 4;
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
    dMgSoundNoteCur *note = (dMgSoundNoteCur *)r5;
    if(note->note.alive != 0){
      int idx=note->note.state;
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
  dScMgSound_c *self = (dScMgSound_c *)scene;
  if(self->mQueueLen < 2) return;
  if(self->mQueueTimer != 0){
    self->mQueueTimer -= 1;
    if((short)self->mQueueTimer <= 0) self->mQueueTimer = 0;
    return;
  }
  if((self->mQueue[0] & 0xff) == (self->mQueue[1] & 0xff)){
    self->mNotes[self->mQueue[0] >> 8].alive = 0;
    self->mNotes[self->mQueue[1] >> 8].alive = 0;
    func_02012790(0x26);
    func_ov006_020c271c(&self->mTable);
  } else {
    self->mNotes[self->mQueue[0] >> 8].state = 3;
    self->mNotes[self->mQueue[1] >> 8].state = 3;
    func_02012790(0xe);
    self->mTries--;
    func_02012790(0x12f);
    func_ov006_020c2664((char *)&self->mTable);
  }
  self->mQueue[1] = 0;
  self->mQueue[0] = self->mQueue[1];
  self->mQueueLen = 0;
  self->mTouchCount = 0;
}
}

// @symbol func_ov006_0211b790
extern "C" {
void func_ov006_0211b790(char* scene)
{
    dScMgSound_c *self = (dScMgSound_c *)scene;
    if (self->mTries == 0) {
        self->mState = 2;
        self->mResultTimer = 0x50;
        return;
    }
    {
        int count = 0;
        int i = 0;
        char* p = scene;
        do {
            if (((dMgSoundNoteCur *)p)->note.alive != 0) {
                count++;
                break;
            }
            i++;
            p += 0x14;
        } while (i < 0xa);
        if (count != 0) return;
        self->mResultTimer = 0x50;
        self->mState = 2;
    }
}
}

// @symbol func_ov006_0211b654
extern "C" {
void func_ov006_0211b654(dScMgSound_c *scene, int n)
{
    int k;
    int i;
    u8* g = data_ov006_0212ee30;

    for (k = 0; k < 3; g++, k++) {
        for (i = 0; i < 30; i++) {
            if (scene->mPops[i].active == 0) {
                scene->mPops[i].active = 1;
                scene->mPops[i].x = scene->mNotes[n].x;
                scene->mPops[i].y = scene->mNotes[n].y - 0x8000;
                scene->mPops[i].startY = scene->mPops[i].y;
                scene->mPops[i].sprite = *g;
                scene->mPops[i].kind = *g;
                scene->mPops[i].timer = 0;
                if (scene->mNotes[n].note == 7) {
                    if (k >= 3) { scene->mPops[i].timer = 0x10; }
                    else { scene->mPops[i].timer = 8; }
                }
                scene->mPops[i].scale = 0x1000;
                scene->mPops[i].velX = 0;
                scene->mPops[i].velY = 0;
                scene->mPops[i].state = 0;
                scene->mPops[i].phase = 0;
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
    dMgSoundPopCur *pop = (dMgSoundPopCur *)r5;
    if(pop->pop.active != 0){
      int idx=pop->pop.state;
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
    unsigned char idx = ((dScMgSound_c *)scene)->mPops[slot].phase;
    (((C *)scene)->*data_ov006_02142d38[idx])(slot);
}

// @symbol func_ov006_0211b4fc
extern "C" {
void func_ov006_0211b4fc(dScMgSound_c *scene, int slot)
{
    scene->mPops[slot].visible = 1;
    scene->mPops[slot].x += data_ov006_0212eeb8[scene->mPops[slot].kind];
    scene->mPops[slot].velY = 0;
    scene->mPops[slot].velX = data_ov006_0212eea0[scene->mPops[slot].kind];
    scene->mPops[slot].dir = data_ov006_0212ee0c[scene->mPops[slot].kind];
    scene->mPops[slot].phase = 1;
}
}

// @symbol func_ov006_0211b3ec
extern "C" {
void func_ov006_0211b3ec(dScMgSound_c *scene, int slot)
{
    int dir;

    scene->mPops[slot].x += scene->mPops[slot].velX;

    dir = scene->mPops[slot].dir;
    if (dir == 0) {
        scene->mPops[slot].velX -= 0x100;
        if (scene->mPops[slot].velX < -0xc00) {
            scene->mPops[slot].velX = -0xc00;
            scene->mPops[slot].dir = 1;
        }
    } else if (dir != 0) {
        scene->mPops[slot].velX += 0x100;
        if (scene->mPops[slot].velX > 0xc00) {
            scene->mPops[slot].velX = 0xc00;
            scene->mPops[slot].dir = 0;
        }
    }

    scene->mPops[slot].y += scene->mPops[slot].velY;
    scene->mPops[slot].velY -= 0xc0;

    if ((scene->mPops[slot].startY - scene->mPops[slot].y) >> 12 >= 0x40) {
        scene->mPops[slot].active = 0;
        scene->mPops[slot].visible = 0;
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
    unsigned char state = ((dScMgSound_c *)scene)->mPops[slot].phase;
    (((C*)scene)->*data_ov006_02142d50[state].pmf)(slot);
}

// @symbol func_ov006_0211b308
extern "C" {
void func_ov006_0211b308(dScMgSound_c *scene, int slot)
{
  scene->mPops[slot].visible = 1;
  scene->mPops[slot].x += data_ov006_0212eed0[scene->mPops[slot].kind];
  scene->mPops[slot].velY = -0x2800;
  scene->mPops[slot].velX = data_ov006_0212eec4[scene->mPops[slot].kind];
  scene->mPops[slot].timer = 0;
  scene->mPops[slot].phase = 1;
}
}

// @symbol func_ov006_0211b278
extern "C" {
void func_ov006_0211b278(dScMgSound_c *scene, int slot){
    scene->mPops[slot].x += scene->mPops[slot].velX;
    scene->mPops[slot].y += scene->mPops[slot].velY;
    scene->mPops[slot].velY += 0x200;
    if (scene->mPops[slot].velY > 0) {
        scene->mPops[slot].velY = 0;
        scene->mPops[slot].phase = 2;
    }
}
}

// @symbol func_ov006_0211b1cc
extern "C" {
void func_ov006_0211b1cc(dScMgSound_c *scene, int slot){
  scene->mPops[slot].x += scene->mPops[slot].velX;
  scene->mPops[slot].y += scene->mPops[slot].velY;
  scene->mPops[slot].velY += 0x100;
  if (((scene->mPops[slot].startY - scene->mPops[slot].y) >> 0xc) > 0x18)
    return;
  if (scene->mPops[slot].velY > 0) {
    scene->mPops[slot].active = 0;
    scene->mPops[slot].visible = 0;
  }
}
}

// @symbol func_ov006_0211b17c
extern "C" void func_ov006_0211b17c(char *scene, int slot) {
    unsigned char state = ((dScMgSound_c *)scene)->mPops[slot].phase;
    (((C*)scene)->*data_ov006_02142d80[state].pmf)(slot);
}

// @symbol func_ov006_0211b0ec
extern "C" {
void func_ov006_0211b0ec(dScMgSound_c *scene, int slot)
{
    scene->mPops[slot].visible = 1;
    scene->mPops[slot].x += data_ov006_0212eef4[scene->mPops[slot].kind];
    scene->mPops[slot].velY = -0x2000;
    scene->mPops[slot].velX = data_ov006_0212eee8[scene->mPops[slot].kind];
    scene->mPops[slot].timer = 0;
    scene->mPops[slot].phase = 1;
}
}

// @symbol func_ov006_0211b05c
extern "C" {
void func_ov006_0211b05c(dScMgSound_c *scene, int slot){
    scene->mPops[slot].x += scene->mPops[slot].velX;
    scene->mPops[slot].y += scene->mPops[slot].velY;
    scene->mPops[slot].velY += 0x100;
    if (scene->mPops[slot].velY > 0) {
        scene->mPops[slot].velY = 0;
        scene->mPops[slot].phase = 2;
    }
}
}

// @symbol func_ov006_0211afb0
extern "C" {
void func_ov006_0211afb0(dScMgSound_c *scene, int slot){
  scene->mPops[slot].x += scene->mPops[slot].velX;
  scene->mPops[slot].y += scene->mPops[slot].velY;
  scene->mPops[slot].velY += 0x100;
  if (((scene->mPops[slot].startY - scene->mPops[slot].y) >> 0xc) > 0x10)
    return;
  if (scene->mPops[slot].velY > 0) {
    scene->mPops[slot].active = 0;
    scene->mPops[slot].visible = 0;
  }
}
}

// @symbol func_ov006_0211af60
extern "C" void func_ov006_0211af60(char *scene, int slot) {
    unsigned char state = ((dScMgSound_c *)scene)->mPops[slot].phase;
    (((C*)scene)->*data_ov006_02142dc8[state].pmf)(slot);
}

// @symbol func_ov006_0211aed0
extern "C" {
void func_ov006_0211aed0(dScMgSound_c *scene, int slot)
{
  scene->mPops[slot].visible = 1;
  scene->mPops[slot].x += data_ov006_0212ef18[scene->mPops[slot].kind];
  scene->mPops[slot].velY = -0x4800;
  scene->mPops[slot].velX = data_ov006_0212ef24[scene->mPops[slot].kind];
  scene->mPops[slot].timer = 0;
  scene->mPops[slot].phase = 1;
}
}

// @symbol func_ov006_0211ae40
extern "C" {
void func_ov006_0211ae40(dScMgSound_c *scene, int slot){
    scene->mPops[slot].x += scene->mPops[slot].velX;
    scene->mPops[slot].y += scene->mPops[slot].velY;
    scene->mPops[slot].velY += 0x400;
    if (scene->mPops[slot].velY > 0) {
        scene->mPops[slot].velY = 0;
        scene->mPops[slot].phase = 2;
    }
}
}

// @symbol func_ov006_0211ad94
extern "C" {
void func_ov006_0211ad94(dScMgSound_c *scene, int slot){
  scene->mPops[slot].x += scene->mPops[slot].velX;
  scene->mPops[slot].y += scene->mPops[slot].velY;
  scene->mPops[slot].velY += 0x200;
  if (((scene->mPops[slot].startY - scene->mPops[slot].y) >> 0xc) > 0x20)
    return;
  if (scene->mPops[slot].velY > 0) {
    scene->mPops[slot].active = 0;
    scene->mPops[slot].visible = 0;
  }
}
}

// @symbol func_ov006_0211ad44
extern "C" void func_ov006_0211ad44(char *scene, int slot) {
    unsigned char state = ((dScMgSound_c *)scene)->mPops[slot].phase;
    (((C*)scene)->*data_ov006_02142de0[state].pmf)(slot);
}

// @symbol func_ov006_0211ad00
extern "C" {
void func_ov006_0211ad00(dScMgSound_c *scene, int slot) {
    scene->mPops[slot].scale = 0x3000;
    scene->mPops[slot].visible = 1;
    scene->mPops[slot].velY = -0x1000;
    scene->mPops[slot].velX = data_ov006_0212ee40[scene->mPops[slot].kind];
    scene->mPops[slot].phase = 1;
}
}

// @symbol func_ov006_0211ac30
extern "C" {
void func_ov006_0211ac30(dScMgSound_c *self, int slot)
{
    self->mPops[slot].x += self->mPops[slot].velX;
    self->mPops[slot].y += self->mPops[slot].velY;
    if (self->mPops[slot].scale > 0x800) {
        self->mPops[slot].scale -= 0x100;
        if (self->mPops[slot].scale < 0x800)
            self->mPops[slot].scale = 0x800;
    }
    self->mPops[slot].velY -= 0x20;
    {
        int d = (self->mPops[slot].startY - self->mPops[slot].y) >> 12;
        if (d >= 0x38) {
            self->mPops[slot].active = 0;
            self->mPops[slot].visible = 0;
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
    unsigned char state = ((dScMgSound_c *)scene)->mPops[slot].phase;
    (((C*)scene)->*data_ov006_02142cd8[state].pmf)(slot);
}

// @symbol func_ov006_0211ab80
extern "C" {
void func_ov006_0211ab80(dScMgSound_c *scene, int slot) {
    unsigned char k;
    scene->mPops[slot].visible = 1;
    k = scene->mPops[slot].kind;
    scene->mPops[slot].x += data_ov006_0212ee7c[k];
    scene->mPops[slot].velY = -0x4000;
    scene->mPops[slot].phase = 1;
}
}

// @symbol func_ov006_0211ab0c
extern "C" {
void func_ov006_0211ab0c(dScMgSound_c *scene, int slot)
{
    scene->mPops[slot].y += scene->mPops[slot].velY;
    scene->mPops[slot].velY += 0x400;
    if (scene->mPops[slot].velY > 0) {
        scene->mPops[slot].velY = 0;
        scene->mPops[slot].phase = 2;
    }
}
}

// @symbol func_ov006_0211aa94
extern "C" {
void func_ov006_0211aa94(dScMgSound_c *self, int slot) {
    self->mPops[slot].y += self->mPops[slot].velY;
    self->mPops[slot].velY += 0x100;
    if (((self->mPops[slot].startY - self->mPops[slot].y) >> 12) <= 0x18) {
        self->mPops[slot].active = 0;
        self->mPops[slot].visible = 0;
    }
}
}

// @symbol func_ov006_0211aa44
extern "C" void func_ov006_0211aa44(char *scene, int slot) {
    unsigned char state = ((dScMgSound_c *)scene)->mPops[slot].phase;
    (((C*)scene)->*data_ov006_02142cf0[state].pmf)(slot);
}

// @symbol func_ov006_0211a9fc
/* Arms one pop: visible, no vertical speed, horizontal speed from its kind. */
extern "C" {
void func_ov006_0211a9fc(dScMgSound_c *self, int slot)
{
    self->mPops[slot].visible = 1;
    self->mPops[slot].velY = 0;
    self->mPops[slot].velX = data_ov006_0212ee70[self->mPops[slot].kind];
    self->mPops[slot].phase = 1;
    self->mPops[slot].timer = 0;
}
}

// @symbol func_ov006_0211a910
extern "C" {
void func_ov006_0211a910(dScMgSound_c *scene, int slot)
{
    int v;
    scene->mPops[slot].x += scene->mPops[slot].velX;
    scene->mPops[slot].y += scene->mPops[slot].velY;
    v = scene->mPops[slot].velX;
    if (v > 0x20) {
        scene->mPops[slot].velX = v - 0x20;
    } else if (v < -0x20) {
        scene->mPops[slot].velX = v + 0x20;
    }
    scene->mPops[slot].velY -= 0x40;
    scene->mPops[slot].timer += 1;
    if (scene->mPops[slot].timer >= 0x28) {
        scene->mPops[slot].timer = 0;
        scene->mPops[slot].phase = 2;
        scene->mPops[slot].dir = 0;
        scene->mPops[slot].velX = 0xc00;
    }
}
}

// @symbol func_ov006_0211a7fc
extern "C" {
void func_ov006_0211a7fc(dScMgSound_c *scene, int slot)
{
    int f;
    scene->mPops[slot].x += scene->mPops[slot].velX;
    scene->mPops[slot].y += scene->mPops[slot].velY;
    scene->mPops[slot].velY -= 0x40;
    f = scene->mPops[slot].dir;
    if (f == 0) {
        scene->mPops[slot].velX -= 0x100;
        if (scene->mPops[slot].velX < -0xc00) {
            scene->mPops[slot].velX = -0xc00;
            scene->mPops[slot].dir = 1;
        }
    } else if (f != 0) {
        scene->mPops[slot].velX += 0x100;
        if (scene->mPops[slot].velX > 0xc00) {
            scene->mPops[slot].velX = 0xc00;
            scene->mPops[slot].dir = 0;
        }
    }
    if ((scene->mPops[slot].startY - scene->mPops[slot].y) >> 12 >= 0x30) {
        scene->mPops[slot].active = 0;
        scene->mPops[slot].visible = 0;
    }
}
}

// @symbol func_ov006_0211a7ac
extern "C" void func_ov006_0211a7ac(char *scene, int slot) {
    unsigned char state = ((dScMgSound_c *)scene)->mPops[slot].phase;
    (((C*)scene)->*data_ov006_02142d20[state].pmf)(slot);
}

// @symbol func_ov006_0211a714
extern "C" {
void func_ov006_0211a714(dScMgSound_c *scene, int slot) {
  unsigned short v = scene->mPops[slot].timer;
  if (v != 0) {
    scene->mPops[slot].timer = v - 1;
    if (*(short *)&scene->mPops[slot].timer < 0)
      scene->mPops[slot].timer = 0;
    return;
  }
  scene->mPops[slot].visible = 1;
  scene->mPops[slot].x += data_ov006_0212ee94[scene->mPops[slot].kind];
  scene->mPops[slot].velY = -0x4000;
  scene->mPops[slot].phase = 1;
}
}

// @symbol func_ov006_0211a69c
extern "C" {
void func_ov006_0211a69c(dScMgSound_c *scene, int slot)
{
    int a = scene->mPops[slot].velY;
    int b = scene->mPops[slot].y;
    scene->mPops[slot].y = b + a;
    a = scene->mPops[slot].velY;
    scene->mPops[slot].velY = a + 0x400;
    {
        int d = scene->mPops[slot].startY - scene->mPops[slot].y;
        d >>= 12;
        if (d >= 0x20) {
            scene->mPops[slot].active = 0;
            scene->mPops[slot].visible = 0;
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
    unsigned char state = ((dScMgSound_c *)scene)->mPops[slot].phase;
    (((C*)scene)->*data_ov006_02142d68[state].pmf)(slot);
}

// @symbol func_ov006_0211a5ec
extern "C" {
void func_ov006_0211a5ec(dScMgSound_c *scene, int slot) {
    unsigned char k;
    scene->mPops[slot].visible = 1;
    k = scene->mPops[slot].kind;
    scene->mPops[slot].x += data_ov006_0212eedc[k];
    scene->mPops[slot].velY = -0x4000;
    scene->mPops[slot].phase = 1;
}
}

// @symbol func_ov006_0211a578
extern "C" {
void func_ov006_0211a578(dScMgSound_c *scene, int slot)
{
    scene->mPops[slot].y += scene->mPops[slot].velY;
    scene->mPops[slot].velY += 0x400;
    if (scene->mPops[slot].velY > 0) {
        scene->mPops[slot].velY = 0;
        scene->mPops[slot].phase = 2;
    }
}
}

// @symbol func_ov006_0211a500
extern "C" {
void func_ov006_0211a500(dScMgSound_c *self, int slot) {
    self->mPops[slot].y += self->mPops[slot].velY;
    self->mPops[slot].velY -= 0x60;
    if ((self->mPops[slot].startY - self->mPops[slot].y) >> 0xc >= 0x3c) {
        self->mPops[slot].active = 0;
        self->mPops[slot].visible = 0;
    }
}
}

// @symbol func_ov006_0211a4b0
extern "C" void func_ov006_0211a4b0(char *scene, int slot)
{
    unsigned char idx = ((dScMgSound_c *)scene)->mPops[slot].phase;
    (((C *)scene)->*data_ov006_02142d98[idx])(slot);
}

// @symbol func_ov006_0211a420
extern "C" {
void func_ov006_0211a420(dScMgSound_c *scene, int slot)
{
  unsigned char idx;
  scene->mPops[slot].visible = 1;
  idx = scene->mPops[slot].kind;
  scene->mPops[slot].x += data_ov006_0212ee64[idx];
  scene->mPops[slot].velY = -0x3800;
  scene->mPops[slot].velX = data_ov006_0212ef00[scene->mPops[slot].kind];
  scene->mPops[slot].timer = 0;
  scene->mPops[slot].phase = 1;
}
}

// @symbol func_ov006_0211a388
extern "C" {
void func_ov006_0211a388(dScMgSound_c *scene, int slot) {
  scene->mPops[slot].x += scene->mPops[slot].velX;
  scene->mPops[slot].y += scene->mPops[slot].velY;
  scene->mPops[slot].velY += 0x200;
  if (scene->mPops[slot].velY <= 0) return;
  scene->mPops[slot].velY = 0;
  scene->mPops[slot].phase = 2;
  scene->mPops[slot].timer = 0;
}
}

// @symbol func_ov006_0211a314
extern "C" {
void func_ov006_0211a314(dScMgSound_c *scene, int slot)
{
    int v = scene->mPops[slot].scale;
    if (v > 0x800) {
        scene->mPops[slot].scale = v - 0x60;
        if (scene->mPops[slot].scale < 0x800)
            scene->mPops[slot].scale = 0x800;
    }
    scene->mPops[slot].timer += 1;
    if (scene->mPops[slot].timer >= 0x20) {
        scene->mPops[slot].active = 0;
        scene->mPops[slot].visible = 0;
    }
}
}

// @symbol func_ov006_0211a2c4
extern "C" void func_ov006_0211a2c4(C* scene, int slot){
  unsigned char sel = ((dScMgSound_c *)scene)->mPops[slot].phase;
  (scene->*data_ov006_02142cc0[sel])(slot);
}

// @symbol func_ov006_0211a234
extern "C" {
void func_ov006_0211a234(dScMgSound_c *scene, int slot)
{
  unsigned char idx;
  scene->mPops[slot].visible = 1;
  idx = scene->mPops[slot].kind;
  scene->mPops[slot].x += data_ov006_0212ee58[idx];
  scene->mPops[slot].velY = -0x3200;
  scene->mPops[slot].velX = data_ov006_0212ee4c[scene->mPops[slot].kind];
  scene->mPops[slot].timer = 0;
  scene->mPops[slot].phase = 1;
}
}

// @symbol func_ov006_0211a19c
extern "C" {
void func_ov006_0211a19c(dScMgSound_c *scene, int slot) {
  scene->mPops[slot].x += scene->mPops[slot].velX;
  scene->mPops[slot].y += scene->mPops[slot].velY;
  scene->mPops[slot].velY += 0x200;
  if (scene->mPops[slot].velY <= 0) return;
  scene->mPops[slot].velY = 0;
  scene->mPops[slot].phase = 2;
  scene->mPops[slot].timer = 0;
}
}

// @symbol func_ov006_0211a128
extern "C" {
void func_ov006_0211a128(dScMgSound_c *scene, int slot)
{
    int v = scene->mPops[slot].scale;
    if (v > 0x800) {
        scene->mPops[slot].scale = v - 0x70;
        if (scene->mPops[slot].scale < 0x800)
            scene->mPops[slot].scale = 0x800;
    }
    scene->mPops[slot].timer += 1;
    if (scene->mPops[slot].timer >= 0x20) {
        scene->mPops[slot].active = 0;
        scene->mPops[slot].visible = 0;
    }
}
}

// @symbol func_ov006_0211a0d8
extern "C" void func_ov006_0211a0d8(CFull* scene, int slot){
  unsigned char phase = ((dScMgSound_c *)scene)->mPops[slot].phase;
  (scene->*(data_ov006_02142d08[phase].pmf))(slot);
}

// @symbol func_ov006_0211a048
extern "C" {
void func_ov006_0211a048(dScMgSound_c *scene, int slot)
{
  scene->mPops[slot].visible = 1;
  scene->mPops[slot].x += data_ov006_0212eeac[scene->mPops[slot].kind];
  scene->mPops[slot].velY = -0x2800;
  scene->mPops[slot].velX = data_ov006_0212ee88[scene->mPops[slot].kind];
  scene->mPops[slot].timer = 0;
  scene->mPops[slot].phase = 1;
}
}

// @symbol func_ov006_02119fb0
extern "C" {
void func_ov006_02119fb0(dScMgSound_c *scene, int slot) {
  scene->mPops[slot].x += scene->mPops[slot].velX;
  scene->mPops[slot].y += scene->mPops[slot].velY;
  scene->mPops[slot].velY += 0x200;
  if (scene->mPops[slot].velY <= 0) return;
  scene->mPops[slot].velY = 0;
  scene->mPops[slot].phase = 2;
  scene->mPops[slot].timer = 0;
}
}

// @symbol func_ov006_02119f3c
extern "C" {
void func_ov006_02119f3c(dScMgSound_c *scene, int slot)
{
    int v = scene->mPops[slot].scale;
    if (v > 0xa80) {
        scene->mPops[slot].scale = v - 0x60;
        if (scene->mPops[slot].scale < 0xa80)
            scene->mPops[slot].scale = 0xa80;
    }
    scene->mPops[slot].timer += 1;
    if (scene->mPops[slot].timer >= 0x18) {
        scene->mPops[slot].active = 0;
        scene->mPops[slot].visible = 0;
    }
}
}

// @symbol func_ov006_02119eec
extern "C" void func_ov006_02119eec(CFull* scene, int slot){
  unsigned char phase = ((dScMgSound_c *)scene)->mPops[slot].phase;
  (scene->*(data_ov006_02142db0[phase].pmf))(slot);
}

// @symbol func_ov006_02119e5c
extern "C" {
void func_ov006_02119e5c(dScMgSound_c *scene, int slot)
{
  unsigned char idx;
  scene->mPops[slot].visible = 1;
  idx = scene->mPops[slot].kind;
  scene->mPops[slot].x += data_ov006_0212ef30[idx];
  scene->mPops[slot].velY = -0x2400;
  scene->mPops[slot].velX = data_ov006_0212ef0c[scene->mPops[slot].kind];
  scene->mPops[slot].timer = 0;
  scene->mPops[slot].phase = 1;
}
}

// @symbol func_ov006_02119dc4
extern "C" {
void func_ov006_02119dc4(dScMgSound_c *scene, int slot) {
  scene->mPops[slot].x += scene->mPops[slot].velX;
  scene->mPops[slot].y += scene->mPops[slot].velY;
  scene->mPops[slot].velY += 0x200;
  if (scene->mPops[slot].velY <= 0) return;
  scene->mPops[slot].velY = 0;
  scene->mPops[slot].phase = 2;
  scene->mPops[slot].timer = 0;
}
}

// @symbol func_ov006_02119d50
extern "C" {
void func_ov006_02119d50(dScMgSound_c *scene, int slot)
{
    int v = scene->mPops[slot].scale;
    if (v > 0xd80) {
        scene->mPops[slot].scale = v - 0x60;
        if (scene->mPops[slot].scale < 0xd80)
            scene->mPops[slot].scale = 0xd80;
    }
    scene->mPops[slot].timer += 1;
    if (scene->mPops[slot].timer >= 0x18) {
        scene->mPops[slot].active = 0;
        scene->mPops[slot].visible = 0;
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
        dMgSoundPopCur *pop = (dMgSoundPopCur *)p;
        if (pop->pop.visible) {
            int m[4];
            long long vv = pop->pop.scale;
            int x = pop->pop.x;
            int y = pop->pop.y;
            int a = (int)((ta * vv + 0x800) >> 12);
            int b = (int)(((long long)tb * (int)vv + 0x800) >> 12);
            m[0] = a;
            m[3] = a;
            m[1] = b;
            m[2] = -b;
            func_ov004_020b023c(data_ov006_0213f6f0[pop->pop.sprite],
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
    dMgSoundNoteCur *note = (dMgSoundNoteCur *)o;
    if (note->note.shown != 0 && note->note.alive != 0) {
      int a1 = note->note.x >> 12;
      int a2 = note->note.y >> 12;
      int a4 = 0;
      if (*(int*)(c + 0x5608) != 1) a4 = 1;
      unsigned char sel = note->note.frame;
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
    func_ov004_020b2574(((dScMgSound_c *)scene)->mTries, 1);
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
    (*(u16 *)(int)&self->mSpriteA.timer)++;
    if (self->mSpriteA.timer < data_ov006_0212ee28[self->mSpriteA.frame])
        return;
    self->mSpriteA.timer = 0;
    (*(u8 *)(int)&self->mSpriteA.frame)++;
}
}

// @symbol func_ov006_02119aa8
extern "C" {
void func_ov006_02119aa8(void *scene){
  dScMgSound_c *self = (dScMgSound_c *)scene;
  if (self->mSpriteA.visible == 0) return;
  Hud_RenderSprite(data_ov006_02138d28[self->mSpriteA.frame],
    self->mSpriteA.x >> 0xc, self->mSpriteA.y >> 0xc, -1, -1);
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
    (*(u16 *)(int)&self->mSpriteB.timer) += 1;
    if (self->mSpriteB.timer < data_ov006_0212ee38[self->mSpriteB.frame]) return;
    self->mSpriteB.timer = 0;
    (*(u8 *)(int)&self->mSpriteB.frame) += 1;
}
}

// @symbol func_ov006_021199c0
extern "C" {
void func_ov006_021199c0(void *scene){
  dScMgSound_c *self = (dScMgSound_c *)scene;
  if (self->mSpriteB.visible == 0) return;
  Hud_RenderSprite(data_ov006_02137ae8[self->mSpriteB.frame],
    self->mSpriteB.x >> 0xc, self->mSpriteB.y >> 0xc, -1, -1);
}
}

// @symbol _ZN12dScMgSound_cD1Ev
// @symbol _ZN12dScMgSound_cD0Ev
/* Both destructor variants come from the inline body in dScMgSound_c.h.
   Declared first in the class, so D1 is emitted before D0. */
