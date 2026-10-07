//cpp
/* Boom-box minigame. Each round deals mNotes on the touch screen; a touch
 * plays that pitch and pops icons in mPops. The class is a
 * dScMgSingle3DBase_c (its RTTI is at 0x0213f6e4).
 *
 * The compiler emits .text in reverse source order, so the functions run
 * here from the highest address down. Do not reorder.
 *
 * Leftover: func_ov006_02119bdc reads mState as the raw word at 0x5608.
 *   The member spelling in the note draw loop grows it by 8 bytes.
 * Leftover: func_ov006_0211c080's deal loop reaches mPairUses and
 *   mPairNotes as q + 0x561a and q + 0x561f, with q = this + pair.
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

/* The state/phase dispatch tables hold pointer-to-member records: the
   sinit copies {pmf, delta} descriptors into them. */
typedef void (dScMgSound_c::*Pmf)(int);
struct PmfEntry { Pmf pmf; };


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
extern PmfEntry data_ov006_02142db0[];
extern int data_ov006_0212eeac[];
extern int data_ov006_0212ee88[];
extern PmfEntry data_ov006_02142d08[];
extern int data_ov006_0212ee58[];
extern int data_ov006_0212ee4c[];
extern Pmf data_ov006_02142cc0[];
extern int data_ov006_0212ee64[];
extern int data_ov006_0212ef00[];
extern Pmf data_ov006_02142d98[];
extern int data_ov006_0212eedc[];
extern PmfEntry data_ov006_02142d68[];
extern int data_ov006_0212ee94[];
extern PmfEntry data_ov006_02142d20[];
extern u32 data_ov006_0212ee70[];
extern PmfEntry data_ov006_02142cf0[];
extern int data_ov006_0212ee7c[];
extern PmfEntry data_ov006_02142cd8[];
extern int data_ov006_0212ee40[];
extern PmfEntry data_ov006_02142de0[];
extern int data_ov006_0212ef18[];
extern int data_ov006_0212ef24[];
extern PmfEntry data_ov006_02142dc8[];
extern int data_ov006_0212eef4[];
extern int data_ov006_0212eee8[];
extern PmfEntry data_ov006_02142d80[];
extern int data_ov006_0212eed0[];
extern int data_ov006_0212eec4[];
extern PmfEntry data_ov006_02142d50[];
extern int data_ov006_0212eeb8[];
extern int data_ov006_0212eea0[];
extern unsigned char data_ov006_0212ee0c[];
extern Pmf data_ov006_02142d38[];
extern PmfEntry data_ov006_02142df8[];
extern PmfEntry data_ov006_02142e20[];
extern u8 data_ov006_0212ee30[];
extern void func_02012790(int);
extern void func_ov006_020c271c(void *c);
extern void func_ov006_020c2664(char *c);
extern int data_ov006_0212ef7c[];
extern int data_ov006_0212ef8c[];
extern int data_ov006_0212ef5c[];
extern int data_ov006_0212ef6c[];
extern unsigned short data_ov006_0212ef4c[];
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
    func_ov006_0211c478();
    func_ov006_0211c080();
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
        func_ov006_0211b954();
        func_ov006_0211b80c();
        func_ov006_0211b5e0();
        func_ov006_0211b790();
        break;
    case 2:
        func_ov006_0211b954();
        func_ov006_0211b5e0();
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
                    func_ov006_02119ba4();
                    func_ov006_02119a88();
                    mPromptEnabled = 0;
                } else {
                    mState = 3;
                    mResultTimer = 0x20;
                    func_ov006_0211b9c8();
                }
            }
        } else {
            func_ov006_02119b00();
            func_ov006_02119a18();
        }
        break;
    case 3:
        func_ov006_0211b954();
        func_ov006_0211b5e0();
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
    func_ov006_02119c74();
    func_ov006_02119bdc();
    func_ov006_02119bc4();
    func_ov006_021199c0();
    func_ov006_02119aa8();
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
    func_ov006_0211c478();

    mTries = 3;
    func_ov006_0211c080();

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

// @symbol _ZN12dScMgSound_c19func_ov006_0211c478Ev
void dScMgSound_c::func_ov006_0211c478()
{
        int i;
    int cnt1;
    int cnt2;
    char *b = (char *)this;

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
        this->mPairUses[cnt2] = 0;
        this->mPairNotes[cnt2] = 0;
    }

    b = (char *)this;
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

    this->mSpriteB.active = 0;
    this->mSpriteB.visible = 1;
    this->mSpriteB.x = 0xbd000;
    this->mSpriteB.y = 0x97000;
    this->mSpriteB.frame = 0;
    this->mSpriteA.active = 0;
    this->mSpriteA.visible = 1;
    this->mSpriteA.x = 0xa0000;
    this->mSpriteA.y = 0x9d000;
    this->mSpriteA.frame = 0;

    this->mQueue[1] = 0;
    this->mQueue[0] = this->mQueue[1];
    this->mQueueTimer = 0;
    this->mQueueLen = 0;
    this->mTouchCount = 0;
    this->mResultTimer = 0;

    func_ov006_020c2924((char *)&this->mTable);
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211c080Ev
void dScMgSound_c::func_ov006_0211c080()
{
        int sel;
    int k;
    int count;
    int n;
    int half;
    int speed;
    int i;

    sel = this->unk_0bc;
    this->mPrevPattern = this->mPattern;
    if (sel >= 5) {
        sel = (u32)(RND * 5) >> 15;
        if (sel == this->mPrevPattern) {
            sel += ((u32)(RND << 2) >> 15) + 1;
            if (sel >= 5)
                sel -= 5;
        }
    }
    this->mPattern = sel;
    k = this->mPattern;
    count = data_ov006_0212ee18[k];
    speed = data_ov006_0212ee10[k];
    if (k == 3) {
        int a = (u32)(RND << 2) >> 15;
        int b = (u32)(RND * 3) >> 15;
        this->mPairNotes[0] = b;
        this->mPairNotes[1] = this->mPairNotes[0] + 1;
        if (this->mPairNotes[1] >= 3)
            this->mPairNotes[1] -= 3;
        this->mPairNotes[2] = this->mPairNotes[1] + 1;
        if (this->mPairNotes[2] >= 3)
            this->mPairNotes[2] -= 3;
        this->mPairNotes[0] += a * 3;
        this->mPairNotes[1] += a * 3;
        this->mPairNotes[2] += a * 3;
    } else {
        half = count >> 1;
        for (i = 0; i < half; i++) {
            int v = (u32)(RND * speed) >> 15;
            this->mPairNotes[i] = v;
            if (i != 0) {
                int dup;
                int j;
                for (;;) {
                    dup = 0;
                    j = 0;
                    for (; j < i; j++) {
                        if (this->mPairNotes[i] == this->mPairNotes[j]) {
                            dup = 1;
                            break;
                        }
                    }
                    if (dup == 0)
                        break;
                    this->mPairNotes[i] = (u32)(RND * speed) >> 15;
                }
            }
        }
    }
    k = this->mPattern;
    if (k == 1 || k == 4) {
        this->mBank = data_ov006_0212efb0[((u32)this->unk_0bc >> 2) & 3];
    }
    half = count >> 1;
    for (i = 0; i < half; i++) {
        this->mPairNotes[i] += data_ov006_0212ef9c[this->mPattern];
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
        p = (char *)this;
        do {
            dMgSoundNoteCur *note = (dMgSoundNoteCur *)p;
            t = data_ov006_0212ee20[this->mPattern];
            note->note.x = data_ov006_0213f6fc[t][xi] << 12;
            note->note.y = (*(u16 * volatile *)&data_ov006_0213f6fc[t])[zi] << 12;
            note->note.timer = 0;
            note->note.state = 0;
            note->note.alive = 1;
            note->note.shown = 1;
            note->note.frame = 0;
            note->note.checked = 0;
            do {
                q = (char *)this + ((u32)(RND * half) >> 15);
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

// @symbol _ZN12dScMgSound_c19func_ov006_0211bf44Ei
void dScMgSound_c::func_ov006_0211bf44(int slot)
{
    u8 player;
    int offset;
    int active;
    int dx;
    int dy;

    if (this->mState != 1) {
        return;
    }
    if (this->mTouchCount >= 2) {
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

    dx = data_020a0dea[player * 4] - (this->mNotes[slot].x >> 12);
    dy = data_020a0deb[player * 4] - (this->mNotes[slot].y >> 12);

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

    this->mNotes[slot].state = 1;
    this->mNotes[slot].timer = 0;
    this->mNotes[slot].frame = 0;
    this->mTouchCount++;
    Sound::PlayBank2_2D(0x201);
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211bc8cEi
/* Advances one note slot's animation. When the slot's last frame is
   reached its note goes on mQueue and is played; mPattern and Virtual8C
   choose between func_02012790, the bank handle mBank and
   Sound::PlayBank2_2D.
   func_ov006_0211b654 is passed `idx` even though it is already in r1:
   dropping it changes the register allocation of the queue block. */
void dScMgSound_c::func_ov006_0211bc8c(int idx)
{
    u8 note;
    u8 instrument;

    this->mNotes[idx].timer = this->mNotes[idx].timer + 1;
    if (this->mNotes[idx].timer < data_ov006_0212ef3c[this->mNotes[idx].frame]) return;
    this->mNotes[idx].timer = 0;
    this->mNotes[idx].frame = this->mNotes[idx].frame + 1;
    if (this->mNotes[idx].frame <= 6) return;
    this->mNotes[idx].frame = 6;
    this->mNotes[idx].state = 2;

    this->mQueue[this->mQueueLen] = this->mNotes[idx].note + 1;
    this->mQueue[this->mQueueLen] |= idx << 8;
    this->mQueueLen++;
    this->mQueueTimer = 0x20;

    func_ov006_0211b654(idx);
    note = this->mNotes[idx].note;

    if (this->Virtual8C()) {
        instrument = this->mPattern;
        if (instrument == 0 || instrument == 2) {
            if (data_ov006_0213f794[note] == 2) {
                func_02012790(data_ov006_0213f7e8[note]);
            } else {
                func_02012174(this->mBank, data_ov006_0213f7e8[note]);
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
        instrument = this->mPattern;
        if (instrument == 0 || instrument == 2) {
            func_02012790(data_ov006_0213f7e8[note]);
        } else if (instrument == 1) {
            func_02012174(this->mBank, data_ov006_0213f7e8[note]);
        } else if (instrument == 4) {
            if (data_ov006_0213f794[note] == 2) {
                Sound::PlayBank2_2D(data_ov006_0213f7e8[note]);
            } else {
                func_02012174(this->mBank, data_ov006_0213f7e8[note]);
            }
        } else {
            Sound::PlayBank2_2D(data_ov006_0213f7e8[note]);
        }
    }

    if (this->mTouchCount == 1) func_ov006_020c2300((char *)&this->mTable);
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211bc68Ei
void dScMgSound_c::func_ov006_0211bc68(int slot) {
    if (this->mTouchCount == 0) {
        this->mNotes[slot].state = 3;
    }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211bbe0Ei
void dScMgSound_c::func_ov006_0211bbe0(int slot){
    unsigned char b;
    this->mNotes[slot].timer = (unsigned short)(this->mNotes[slot].timer + 1);
    b = this->mNotes[slot].frame;
    if (this->mNotes[slot].timer < data_ov006_0212ef4c[b]) return;
    this->mNotes[slot].timer = 0;
    this->mNotes[slot].frame = (unsigned char)(this->mNotes[slot].frame - 1);
    if (this->mNotes[slot].frame == 0) this->mNotes[slot].state = 0;
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211ba88Ei
/* Scrolls one note slot left by 0x10 per call and clears it once it has
   gone off screen. Its column comes from the per-mode tables
   data_ov006_0212ef5c and data_ov006_0212ef6c. */
void dScMgSound_c::func_ov006_0211ba88(int slot)
{
    int n;
    int i;
    int limit;

    this->mNotes[slot].x -= 0x10000;

    if (this->Virtual8C() != 0) {
        limit = data_ov006_0212ef5c[this->mPattern];
    } else {
        limit = data_ov006_0212ef6c[this->mPattern];
    }

    if (this->mNotes[slot].checked == 0) {
        n = slot;
        if (slot >= limit) {
            do {
                n = n - limit;
            } while (n >= limit);
        }
        if (n == 0) {
            this->mNotes[slot].checked = 1;
            return;
        }
        for (i = 0; i < n; i++) {
            int prev = slot - i - 1;
            if (this->mNotes[prev].alive != 0) {
                if ((this->mNotes[slot].x - this->mNotes[prev].x) >> 0xc <= 4) {
                    this->mNotes[slot].checked = 1;
                    this->mNotes[prev].state = 4;
                }
            }
        }
    }

    if (this->mNotes[slot].x >> 0xc > -0x18) return;

    this->mNotes[slot].alive = 0;
    this->mNotes[slot].shown = 0;
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211b9c8Ev
void dScMgSound_c::func_ov006_0211b9c8() {
        int lr;
    int ip;
    int found;
    int off;
    if (this->Virtual8C()) {
        lr = data_ov006_0212ef7c[this->mPattern];
    } else {
        lr = data_ov006_0212ef8c[this->mPattern];
    }
    for (ip = 0, off = 0; ip < 2; ip++) {
        int sb;
        found = -1;
        for (sb = 0; sb < lr; sb++) {
            int e = sb + off;
            if (this->mNotes[e].alive != 0)
                found = e;
        }
        if (found != -1) {
            this->mNotes[found].state = 4;
        }
        off += lr;
    }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211b954Ev
void dScMgSound_c::func_ov006_0211b954(){
  int i=0;
  char *r5 = (char *)this;
  do{
    dMgSoundNoteCur *note = (dMgSoundNoteCur *)r5;
    if(note->note.alive != 0){
      int idx=note->note.state;
      (this->*data_ov006_02142df8[idx].pmf)(i);
    }
    i++;
    r5+=0x14;
  }while(i<10);
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211b80cEv
void dScMgSound_c::func_ov006_0211b80c(){
    if(this->mQueueLen < 2) return;
  if(this->mQueueTimer != 0){
    this->mQueueTimer -= 1;
    if((short)this->mQueueTimer <= 0) this->mQueueTimer = 0;
    return;
  }
  if((this->mQueue[0] & 0xff) == (this->mQueue[1] & 0xff)){
    this->mNotes[this->mQueue[0] >> 8].alive = 0;
    this->mNotes[this->mQueue[1] >> 8].alive = 0;
    func_02012790(0x26);
    func_ov006_020c271c(&this->mTable);
  } else {
    this->mNotes[this->mQueue[0] >> 8].state = 3;
    this->mNotes[this->mQueue[1] >> 8].state = 3;
    func_02012790(0xe);
    this->mTries--;
    func_02012790(0x12f);
    func_ov006_020c2664((char *)&this->mTable);
  }
  this->mQueue[1] = 0;
  this->mQueue[0] = this->mQueue[1];
  this->mQueueLen = 0;
  this->mTouchCount = 0;
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211b790Ev
void dScMgSound_c::func_ov006_0211b790()
{
        if (this->mTries == 0) {
        this->mState = 2;
        this->mResultTimer = 0x50;
        return;
    }
    {
        int count = 0;
        int i = 0;
        char *p = (char *)this;
        do {
            if (((dMgSoundNoteCur *)p)->note.alive != 0) {
                count++;
                break;
            }
            i++;
            p += 0x14;
        } while (i < 0xa);
        if (count != 0) return;
        this->mResultTimer = 0x50;
        this->mState = 2;
    }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211b654Ei
void dScMgSound_c::func_ov006_0211b654(int n)
{
    int k;
    int i;
    u8* g = data_ov006_0212ee30;

    for (k = 0; k < 3; g++, k++) {
        for (i = 0; i < 30; i++) {
            if (this->mPops[i].active == 0) {
                this->mPops[i].active = 1;
                this->mPops[i].x = this->mNotes[n].x;
                this->mPops[i].y = this->mNotes[n].y - 0x8000;
                this->mPops[i].startY = this->mPops[i].y;
                this->mPops[i].sprite = *g;
                this->mPops[i].kind = *g;
                this->mPops[i].timer = 0;
                if (this->mNotes[n].note == 7) {
                    if (k >= 3) { this->mPops[i].timer = 0x10; }
                    else { this->mPops[i].timer = 8; }
                }
                this->mPops[i].scale = 0x1000;
                this->mPops[i].velX = 0;
                this->mPops[i].velY = 0;
                this->mPops[i].state = 0;
                this->mPops[i].phase = 0;
                break;
            }
        }
    }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211b5e0Ev
void dScMgSound_c::func_ov006_0211b5e0(){
  int i=0;
  char *r5 = (char *)this;
  do{
    dMgSoundPopCur *pop = (dMgSoundPopCur *)r5;
    if(pop->pop.active != 0){
      int idx=pop->pop.state;
      (this->*data_ov006_02142e20[idx].pmf)(i);
    }
    i++;
    r5+=0x24;
  }while(i<30);
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211b590Ei
void dScMgSound_c::func_ov006_0211b590(int slot)
{
    unsigned char idx = this->mPops[slot].phase;
    (this->*data_ov006_02142d38[idx])(slot);
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211b4fcEi
void dScMgSound_c::func_ov006_0211b4fc(int slot)
{
    this->mPops[slot].visible = 1;
    this->mPops[slot].x += data_ov006_0212eeb8[this->mPops[slot].kind];
    this->mPops[slot].velY = 0;
    this->mPops[slot].velX = data_ov006_0212eea0[this->mPops[slot].kind];
    this->mPops[slot].dir = data_ov006_0212ee0c[this->mPops[slot].kind];
    this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211b3ecEi
void dScMgSound_c::func_ov006_0211b3ec(int slot)
{
    int dir;

    this->mPops[slot].x += this->mPops[slot].velX;

    dir = this->mPops[slot].dir;
    if (dir == 0) {
        this->mPops[slot].velX -= 0x100;
        if (this->mPops[slot].velX < -0xc00) {
            this->mPops[slot].velX = -0xc00;
            this->mPops[slot].dir = 1;
        }
    } else if (dir != 0) {
        this->mPops[slot].velX += 0x100;
        if (this->mPops[slot].velX > 0xc00) {
            this->mPops[slot].velX = 0xc00;
            this->mPops[slot].dir = 0;
        }
    }

    this->mPops[slot].y += this->mPops[slot].velY;
    this->mPops[slot].velY -= 0xc0;

    if ((this->mPops[slot].startY - this->mPops[slot].y) >> 12 >= 0x40) {
        this->mPops[slot].active = 0;
        this->mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211b3e8Ev
void dScMgSound_c::func_ov006_0211b3e8()
{
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211b398Ei
void dScMgSound_c::func_ov006_0211b398(int slot)
{
    unsigned char state = this->mPops[slot].phase;
    (this->*data_ov006_02142d50[state].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211b308Ei
void dScMgSound_c::func_ov006_0211b308(int slot)
{
  this->mPops[slot].visible = 1;
  this->mPops[slot].x += data_ov006_0212eed0[this->mPops[slot].kind];
  this->mPops[slot].velY = -0x2800;
  this->mPops[slot].velX = data_ov006_0212eec4[this->mPops[slot].kind];
  this->mPops[slot].timer = 0;
  this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211b278Ei
void dScMgSound_c::func_ov006_0211b278(int slot){
    this->mPops[slot].x += this->mPops[slot].velX;
    this->mPops[slot].y += this->mPops[slot].velY;
    this->mPops[slot].velY += 0x200;
    if (this->mPops[slot].velY > 0) {
        this->mPops[slot].velY = 0;
        this->mPops[slot].phase = 2;
    }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211b1ccEi
void dScMgSound_c::func_ov006_0211b1cc(int slot){
  this->mPops[slot].x += this->mPops[slot].velX;
  this->mPops[slot].y += this->mPops[slot].velY;
  this->mPops[slot].velY += 0x100;
  if (((this->mPops[slot].startY - this->mPops[slot].y) >> 0xc) > 0x18)
    return;
  if (this->mPops[slot].velY > 0) {
    this->mPops[slot].active = 0;
    this->mPops[slot].visible = 0;
  }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211b17cEi
void dScMgSound_c::func_ov006_0211b17c(int slot) {
    unsigned char state = this->mPops[slot].phase;
    (this->*data_ov006_02142d80[state].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211b0ecEi
void dScMgSound_c::func_ov006_0211b0ec(int slot)
{
    this->mPops[slot].visible = 1;
    this->mPops[slot].x += data_ov006_0212eef4[this->mPops[slot].kind];
    this->mPops[slot].velY = -0x2000;
    this->mPops[slot].velX = data_ov006_0212eee8[this->mPops[slot].kind];
    this->mPops[slot].timer = 0;
    this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211b05cEi
void dScMgSound_c::func_ov006_0211b05c(int slot){
    this->mPops[slot].x += this->mPops[slot].velX;
    this->mPops[slot].y += this->mPops[slot].velY;
    this->mPops[slot].velY += 0x100;
    if (this->mPops[slot].velY > 0) {
        this->mPops[slot].velY = 0;
        this->mPops[slot].phase = 2;
    }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211afb0Ei
void dScMgSound_c::func_ov006_0211afb0(int slot){
  this->mPops[slot].x += this->mPops[slot].velX;
  this->mPops[slot].y += this->mPops[slot].velY;
  this->mPops[slot].velY += 0x100;
  if (((this->mPops[slot].startY - this->mPops[slot].y) >> 0xc) > 0x10)
    return;
  if (this->mPops[slot].velY > 0) {
    this->mPops[slot].active = 0;
    this->mPops[slot].visible = 0;
  }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211af60Ei
void dScMgSound_c::func_ov006_0211af60(int slot) {
    unsigned char state = this->mPops[slot].phase;
    (this->*data_ov006_02142dc8[state].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211aed0Ei
void dScMgSound_c::func_ov006_0211aed0(int slot)
{
  this->mPops[slot].visible = 1;
  this->mPops[slot].x += data_ov006_0212ef18[this->mPops[slot].kind];
  this->mPops[slot].velY = -0x4800;
  this->mPops[slot].velX = data_ov006_0212ef24[this->mPops[slot].kind];
  this->mPops[slot].timer = 0;
  this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211ae40Ei
void dScMgSound_c::func_ov006_0211ae40(int slot){
    this->mPops[slot].x += this->mPops[slot].velX;
    this->mPops[slot].y += this->mPops[slot].velY;
    this->mPops[slot].velY += 0x400;
    if (this->mPops[slot].velY > 0) {
        this->mPops[slot].velY = 0;
        this->mPops[slot].phase = 2;
    }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211ad94Ei
void dScMgSound_c::func_ov006_0211ad94(int slot){
  this->mPops[slot].x += this->mPops[slot].velX;
  this->mPops[slot].y += this->mPops[slot].velY;
  this->mPops[slot].velY += 0x200;
  if (((this->mPops[slot].startY - this->mPops[slot].y) >> 0xc) > 0x20)
    return;
  if (this->mPops[slot].velY > 0) {
    this->mPops[slot].active = 0;
    this->mPops[slot].visible = 0;
  }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211ad44Ei
void dScMgSound_c::func_ov006_0211ad44(int slot) {
    unsigned char state = this->mPops[slot].phase;
    (this->*data_ov006_02142de0[state].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211ad00Ei
void dScMgSound_c::func_ov006_0211ad00(int slot) {
    this->mPops[slot].scale = 0x3000;
    this->mPops[slot].visible = 1;
    this->mPops[slot].velY = -0x1000;
    this->mPops[slot].velX = data_ov006_0212ee40[this->mPops[slot].kind];
    this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211ac30Ei
void dScMgSound_c::func_ov006_0211ac30(int slot)
{
    this->mPops[slot].x += this->mPops[slot].velX;
    this->mPops[slot].y += this->mPops[slot].velY;
    if (this->mPops[slot].scale > 0x800) {
        this->mPops[slot].scale -= 0x100;
        if (this->mPops[slot].scale < 0x800)
            this->mPops[slot].scale = 0x800;
    }
    this->mPops[slot].velY -= 0x20;
    {
        int d = (this->mPops[slot].startY - this->mPops[slot].y) >> 12;
        if (d >= 0x38) {
            this->mPops[slot].active = 0;
            this->mPops[slot].visible = 0;
        }
    }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211ac2cEv
void dScMgSound_c::func_ov006_0211ac2c()
{
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211abdcEi
void dScMgSound_c::func_ov006_0211abdc(int slot) {
    unsigned char state = this->mPops[slot].phase;
    (this->*data_ov006_02142cd8[state].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211ab80Ei
void dScMgSound_c::func_ov006_0211ab80(int slot) {
    unsigned char k;
    this->mPops[slot].visible = 1;
    k = this->mPops[slot].kind;
    this->mPops[slot].x += data_ov006_0212ee7c[k];
    this->mPops[slot].velY = -0x4000;
    this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211ab0cEi
void dScMgSound_c::func_ov006_0211ab0c(int slot)
{
    this->mPops[slot].y += this->mPops[slot].velY;
    this->mPops[slot].velY += 0x400;
    if (this->mPops[slot].velY > 0) {
        this->mPops[slot].velY = 0;
        this->mPops[slot].phase = 2;
    }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211aa94Ei
void dScMgSound_c::func_ov006_0211aa94(int slot) {
    this->mPops[slot].y += this->mPops[slot].velY;
    this->mPops[slot].velY += 0x100;
    if (((this->mPops[slot].startY - this->mPops[slot].y) >> 12) <= 0x18) {
        this->mPops[slot].active = 0;
        this->mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211aa44Ei
void dScMgSound_c::func_ov006_0211aa44(int slot) {
    unsigned char state = this->mPops[slot].phase;
    (this->*data_ov006_02142cf0[state].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a9fcEi
/* Arms one pop: visible, no vertical speed, horizontal speed from its kind. */
void dScMgSound_c::func_ov006_0211a9fc(int slot)
{
    this->mPops[slot].visible = 1;
    this->mPops[slot].velY = 0;
    this->mPops[slot].velX = data_ov006_0212ee70[this->mPops[slot].kind];
    this->mPops[slot].phase = 1;
    this->mPops[slot].timer = 0;
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a910Ei
void dScMgSound_c::func_ov006_0211a910(int slot)
{
    int v;
    this->mPops[slot].x += this->mPops[slot].velX;
    this->mPops[slot].y += this->mPops[slot].velY;
    v = this->mPops[slot].velX;
    if (v > 0x20) {
        this->mPops[slot].velX = v - 0x20;
    } else if (v < -0x20) {
        this->mPops[slot].velX = v + 0x20;
    }
    this->mPops[slot].velY -= 0x40;
    this->mPops[slot].timer += 1;
    if (this->mPops[slot].timer >= 0x28) {
        this->mPops[slot].timer = 0;
        this->mPops[slot].phase = 2;
        this->mPops[slot].dir = 0;
        this->mPops[slot].velX = 0xc00;
    }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a7fcEi
void dScMgSound_c::func_ov006_0211a7fc(int slot)
{
    int f;
    this->mPops[slot].x += this->mPops[slot].velX;
    this->mPops[slot].y += this->mPops[slot].velY;
    this->mPops[slot].velY -= 0x40;
    f = this->mPops[slot].dir;
    if (f == 0) {
        this->mPops[slot].velX -= 0x100;
        if (this->mPops[slot].velX < -0xc00) {
            this->mPops[slot].velX = -0xc00;
            this->mPops[slot].dir = 1;
        }
    } else if (f != 0) {
        this->mPops[slot].velX += 0x100;
        if (this->mPops[slot].velX > 0xc00) {
            this->mPops[slot].velX = 0xc00;
            this->mPops[slot].dir = 0;
        }
    }
    if ((this->mPops[slot].startY - this->mPops[slot].y) >> 12 >= 0x30) {
        this->mPops[slot].active = 0;
        this->mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a7acEi
void dScMgSound_c::func_ov006_0211a7ac(int slot) {
    unsigned char state = this->mPops[slot].phase;
    (this->*data_ov006_02142d20[state].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a714Ei
void dScMgSound_c::func_ov006_0211a714(int slot) {
  unsigned short v = this->mPops[slot].timer;
  if (v != 0) {
    this->mPops[slot].timer = v - 1;
    if (*(short *)&this->mPops[slot].timer < 0)
      this->mPops[slot].timer = 0;
    return;
  }
  this->mPops[slot].visible = 1;
  this->mPops[slot].x += data_ov006_0212ee94[this->mPops[slot].kind];
  this->mPops[slot].velY = -0x4000;
  this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a69cEi
void dScMgSound_c::func_ov006_0211a69c(int slot)
{
    int a = this->mPops[slot].velY;
    int b = this->mPops[slot].y;
    this->mPops[slot].y = b + a;
    a = this->mPops[slot].velY;
    this->mPops[slot].velY = a + 0x400;
    {
        int d = this->mPops[slot].startY - this->mPops[slot].y;
        d >>= 12;
        if (d >= 0x20) {
            this->mPops[slot].active = 0;
            this->mPops[slot].visible = 0;
        }
    }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a698Ev
void dScMgSound_c::func_ov006_0211a698()
{
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a648Ei
void dScMgSound_c::func_ov006_0211a648(int slot) {
    unsigned char state = this->mPops[slot].phase;
    (this->*data_ov006_02142d68[state].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a5ecEi
void dScMgSound_c::func_ov006_0211a5ec(int slot) {
    unsigned char k;
    this->mPops[slot].visible = 1;
    k = this->mPops[slot].kind;
    this->mPops[slot].x += data_ov006_0212eedc[k];
    this->mPops[slot].velY = -0x4000;
    this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a578Ei
void dScMgSound_c::func_ov006_0211a578(int slot)
{
    this->mPops[slot].y += this->mPops[slot].velY;
    this->mPops[slot].velY += 0x400;
    if (this->mPops[slot].velY > 0) {
        this->mPops[slot].velY = 0;
        this->mPops[slot].phase = 2;
    }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a500Ei
void dScMgSound_c::func_ov006_0211a500(int slot) {
    this->mPops[slot].y += this->mPops[slot].velY;
    this->mPops[slot].velY -= 0x60;
    if ((this->mPops[slot].startY - this->mPops[slot].y) >> 0xc >= 0x3c) {
        this->mPops[slot].active = 0;
        this->mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a4b0Ei
void dScMgSound_c::func_ov006_0211a4b0(int slot)
{
    unsigned char idx = this->mPops[slot].phase;
    (this->*data_ov006_02142d98[idx])(slot);
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a420Ei
void dScMgSound_c::func_ov006_0211a420(int slot)
{
  unsigned char idx;
  this->mPops[slot].visible = 1;
  idx = this->mPops[slot].kind;
  this->mPops[slot].x += data_ov006_0212ee64[idx];
  this->mPops[slot].velY = -0x3800;
  this->mPops[slot].velX = data_ov006_0212ef00[this->mPops[slot].kind];
  this->mPops[slot].timer = 0;
  this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a388Ei
void dScMgSound_c::func_ov006_0211a388(int slot) {
  this->mPops[slot].x += this->mPops[slot].velX;
  this->mPops[slot].y += this->mPops[slot].velY;
  this->mPops[slot].velY += 0x200;
  if (this->mPops[slot].velY <= 0) return;
  this->mPops[slot].velY = 0;
  this->mPops[slot].phase = 2;
  this->mPops[slot].timer = 0;
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a314Ei
void dScMgSound_c::func_ov006_0211a314(int slot)
{
    int v = this->mPops[slot].scale;
    if (v > 0x800) {
        this->mPops[slot].scale = v - 0x60;
        if (this->mPops[slot].scale < 0x800)
            this->mPops[slot].scale = 0x800;
    }
    this->mPops[slot].timer += 1;
    if (this->mPops[slot].timer >= 0x20) {
        this->mPops[slot].active = 0;
        this->mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a2c4Ei
void dScMgSound_c::func_ov006_0211a2c4(int slot){
  unsigned char sel = this->mPops[slot].phase;
  (this->*data_ov006_02142cc0[sel])(slot);
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a234Ei
void dScMgSound_c::func_ov006_0211a234(int slot)
{
  unsigned char idx;
  this->mPops[slot].visible = 1;
  idx = this->mPops[slot].kind;
  this->mPops[slot].x += data_ov006_0212ee58[idx];
  this->mPops[slot].velY = -0x3200;
  this->mPops[slot].velX = data_ov006_0212ee4c[this->mPops[slot].kind];
  this->mPops[slot].timer = 0;
  this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a19cEi
void dScMgSound_c::func_ov006_0211a19c(int slot) {
  this->mPops[slot].x += this->mPops[slot].velX;
  this->mPops[slot].y += this->mPops[slot].velY;
  this->mPops[slot].velY += 0x200;
  if (this->mPops[slot].velY <= 0) return;
  this->mPops[slot].velY = 0;
  this->mPops[slot].phase = 2;
  this->mPops[slot].timer = 0;
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a128Ei
void dScMgSound_c::func_ov006_0211a128(int slot)
{
    int v = this->mPops[slot].scale;
    if (v > 0x800) {
        this->mPops[slot].scale = v - 0x70;
        if (this->mPops[slot].scale < 0x800)
            this->mPops[slot].scale = 0x800;
    }
    this->mPops[slot].timer += 1;
    if (this->mPops[slot].timer >= 0x20) {
        this->mPops[slot].active = 0;
        this->mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a0d8Ei
void dScMgSound_c::func_ov006_0211a0d8(int slot){
  unsigned char phase = this->mPops[slot].phase;
  (this->*(data_ov006_02142d08[phase].pmf))(slot);
}

// @symbol _ZN12dScMgSound_c19func_ov006_0211a048Ei
void dScMgSound_c::func_ov006_0211a048(int slot)
{
  this->mPops[slot].visible = 1;
  this->mPops[slot].x += data_ov006_0212eeac[this->mPops[slot].kind];
  this->mPops[slot].velY = -0x2800;
  this->mPops[slot].velX = data_ov006_0212ee88[this->mPops[slot].kind];
  this->mPops[slot].timer = 0;
  this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c19func_ov006_02119fb0Ei
void dScMgSound_c::func_ov006_02119fb0(int slot) {
  this->mPops[slot].x += this->mPops[slot].velX;
  this->mPops[slot].y += this->mPops[slot].velY;
  this->mPops[slot].velY += 0x200;
  if (this->mPops[slot].velY <= 0) return;
  this->mPops[slot].velY = 0;
  this->mPops[slot].phase = 2;
  this->mPops[slot].timer = 0;
}

// @symbol _ZN12dScMgSound_c19func_ov006_02119f3cEi
void dScMgSound_c::func_ov006_02119f3c(int slot)
{
    int v = this->mPops[slot].scale;
    if (v > 0xa80) {
        this->mPops[slot].scale = v - 0x60;
        if (this->mPops[slot].scale < 0xa80)
            this->mPops[slot].scale = 0xa80;
    }
    this->mPops[slot].timer += 1;
    if (this->mPops[slot].timer >= 0x18) {
        this->mPops[slot].active = 0;
        this->mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c19func_ov006_02119eecEi
void dScMgSound_c::func_ov006_02119eec(int slot){
  unsigned char phase = this->mPops[slot].phase;
  (this->*(data_ov006_02142db0[phase].pmf))(slot);
}

// @symbol _ZN12dScMgSound_c19func_ov006_02119e5cEi
void dScMgSound_c::func_ov006_02119e5c(int slot)
{
  unsigned char idx;
  this->mPops[slot].visible = 1;
  idx = this->mPops[slot].kind;
  this->mPops[slot].x += data_ov006_0212ef30[idx];
  this->mPops[slot].velY = -0x2400;
  this->mPops[slot].velX = data_ov006_0212ef0c[this->mPops[slot].kind];
  this->mPops[slot].timer = 0;
  this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c19func_ov006_02119dc4Ei
void dScMgSound_c::func_ov006_02119dc4(int slot) {
  this->mPops[slot].x += this->mPops[slot].velX;
  this->mPops[slot].y += this->mPops[slot].velY;
  this->mPops[slot].velY += 0x200;
  if (this->mPops[slot].velY <= 0) return;
  this->mPops[slot].velY = 0;
  this->mPops[slot].phase = 2;
  this->mPops[slot].timer = 0;
}

// @symbol _ZN12dScMgSound_c19func_ov006_02119d50Ei
void dScMgSound_c::func_ov006_02119d50(int slot)
{
    int v = this->mPops[slot].scale;
    if (v > 0xd80) {
        this->mPops[slot].scale = v - 0x60;
        if (this->mPops[slot].scale < 0xd80)
            this->mPops[slot].scale = 0xd80;
    }
    this->mPops[slot].timer += 1;
    if (this->mPops[slot].timer >= 0x18) {
        this->mPops[slot].active = 0;
        this->mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c19func_ov006_02119c74Ev
extern "C" void func_ov004_020b023c(void* a0, int a1, int a2, int a3, void* a4);

void dScMgSound_c::func_ov006_02119c74()
{
    char *p = (char *)this;
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

// @symbol _ZN12dScMgSound_c19func_ov006_02119bdcEv
extern void* data_ov006_0213f730[];
void dScMgSound_c::func_ov006_02119bdc() {
  char *c = (char *)this;
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

// @symbol _ZN12dScMgSound_c19func_ov006_02119bc4Ev
void dScMgSound_c::func_ov006_02119bc4()
{
    func_ov004_020b2574(this->mTries, 1);
}

// @symbol _ZN12dScMgSound_c19func_ov006_02119ba4Ev
void dScMgSound_c::func_ov006_02119ba4()
{
        this->mSpriteA.active = 1;
    this->mSpriteA.timer = 0;
    this->mSpriteA.frame = 0;
}

// @symbol _ZN12dScMgSound_c19func_ov006_02119b00Ev
void dScMgSound_c::func_ov006_02119b00()
{
        if (this->mSpriteA.active == 0)
        return;
    if (this->mSpriteA.frame >= 2)
        return;
    (*(u16 *)(int)&this->mSpriteA.timer)++;
    if (this->mSpriteA.timer < data_ov006_0212ee28[this->mSpriteA.frame])
        return;
    this->mSpriteA.timer = 0;
    (*(u8 *)(int)&this->mSpriteA.frame)++;
}

// @symbol _ZN12dScMgSound_c19func_ov006_02119aa8Ev
void dScMgSound_c::func_ov006_02119aa8(){
    if (this->mSpriteA.visible == 0) return;
  Hud_RenderSprite(data_ov006_02138d28[this->mSpriteA.frame],
    this->mSpriteA.x >> 0xc, this->mSpriteA.y >> 0xc, -1, -1);
}

// @symbol _ZN12dScMgSound_c19func_ov006_02119a88Ev
void dScMgSound_c::func_ov006_02119a88()
{
        this->mSpriteB.active = 1;
    this->mSpriteB.timer = 0;
    this->mSpriteB.frame = 0;
}

// @symbol _ZN12dScMgSound_c19func_ov006_02119a18Ev
void dScMgSound_c::func_ov006_02119a18()
{
        if (this->mSpriteB.active == 0) return;
    if (this->mSpriteB.frame >= 3) return;
    (*(u16 *)(int)&this->mSpriteB.timer) += 1;
    if (this->mSpriteB.timer < data_ov006_0212ee38[this->mSpriteB.frame]) return;
    this->mSpriteB.timer = 0;
    (*(u8 *)(int)&this->mSpriteB.frame) += 1;
}

// @symbol _ZN12dScMgSound_c19func_ov006_021199c0Ev
void dScMgSound_c::func_ov006_021199c0(){
    if (this->mSpriteB.visible == 0) return;
  Hud_RenderSprite(data_ov006_02137ae8[this->mSpriteB.frame],
    this->mSpriteB.x >> 0xc, this->mSpriteB.y >> 0xc, -1, -1);
}

// @symbol _ZN12dScMgSound_cD1Ev
// @symbol _ZN12dScMgSound_cD0Ev
/* Both destructor variants come from the inline body in dScMgSound_c.h.
   Declared first in the class, so D1 is emitted before D0. */
