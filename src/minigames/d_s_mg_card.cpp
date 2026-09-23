//cpp
/* Picture Poker: the scene, the player's cards and the dealer's cards
 * (ov006/dScMgCard_c, 37 functions).
 *
 * Keep definition order: the compiler emits functions in reverse. The scene
 * destructor is inline in dScMgCard_c.h, which with the four card
 * constructor and destructor callbacks keeps D1 ahead of D0 as in the
 * cartridge.
 *
 * Blocked: the ROM has no dScMgCard_c constructor, so the factory builds the
 * scene by hand and its base constructor, operator new and vtable stores stay
 * mangled; and the ov004 and ov006 helpers are unnamed in symbols.txt.
 */

#include "dScMgCard_c.h"
#include "types.h"
#include "decl_common.h"
#include "Sound.h"

/* By-value five-card ordering table. */
typedef struct 
{
  s32 v[5];
} Five;

/* Sprite template record; attr3 terminates the table at 0xffff. */
typedef struct OamAttrTmpl {
    u32 attr0; /* 0x0 */
    u16 attr2; /* 0x4 */
    u16 attr3; /* 0x6 */
} OamAttrTmpl;

extern "C" {
extern int data_ov006_0213acb0[2];
extern int data_ov006_0213aca8[2];
/* The two element vtables are emitted here by their key functions. The four
   measured array-construction ABI callbacks reference the ROM address points;
   these names are already mangled, so C linkage means "use literally". */
extern unsigned short data_ov006_0213bd64[];
extern void *data_ov006_02141774[];
extern void Hud_RenderSprite(void *sprite, int x, int y, int a3, int a4);
extern void **data_ov006_0213bd30[];
extern void *data_ov006_0213406c[];
unsigned int func_02012790(unsigned int a);
extern int data_ov006_02141768;
extern int data_ov006_0214176c;
extern int data_ov006_02141770;
extern unsigned char data_020a0e40;
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern unsigned char data_020a0dea[][4];
extern unsigned char data_020a0deb[][4];
extern "C" void Vec2_Sub(int* o, int* a, int* b);
extern "C" void func_0203d680(int* out, int* in, int scale);
extern const Five data_ov006_0213bcd8;
extern int RandomIntInternal(int *seed);
extern int data_ov006_0213bd18[];
extern int data_0209e650;
extern "C" void func_ov006_020c0aa8(void *c);
extern "C" void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
extern "C" void func_ov004_020b1bc8(char *a0, int a1, int a2, int a3);
extern "C" void func_ov004_020b1e34(void *a0, int a1, int a2, int a3);
extern "C" void func_ov006_020c1804(void *c);
extern int data_ov006_02134028;
void func_ov006_020c19d0(void *table);
void func_ov004_020b65e4(void);
extern int func_ov006_020c0f0c(void *p);
extern int func_ov006_020c1718(void *p);
extern void func_ov006_020c1420(void *c, s16 arg1, void *arg2);
extern void func_ov006_020c0d68(void *c);
extern void func_ov006_020c0c80(void *c);
extern void func_ov004_020ad79c(int a0, int a1);
extern void func_ov004_020b0cac(int a0, int a1, int a2, int a3, int a4, s16 a5);
extern void func_ov004_020adb1c(int v);
extern void func_ov004_020b0a54(int v);
extern int data_ov004_020bf9ec;
extern int data_ov006_0213bc44;
extern int data_ov004_020bfa18;
extern int data_ov006_0213bd48[];
extern void FreeGfxSlotsById(int arg);
extern void func_ov004_020b56c8(int a);
extern s16 data_ov004_020bf9e4;
/* The active scene, as a dScMgBase_c. */
extern void* data_ov004_020beb68;
extern "C" void func_ov006_020c1604(char *c, int unused, short a2, int a3);
extern "C" void func_ov004_020b66d4(void);
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern void func_ov006_0210a534(char *);
extern void *LoadFile(int);
extern int GetGameLanguage(void);
extern void DecompressLZ16(void *, void *);
extern int func_ov006_020c1a88(char *);
void* _ZN7fBase_cnwEj(unsigned int);
void _ZN11dScMgBase_cC2Ev(void*);
void _ZN8Particle10SysTrackerC1Ev(void*);
typedef void (*CardElementCallback)(void *);
void __cxa_vec_ctor(void *, unsigned int, unsigned int,
                    CardElementCallback, CardElementCallback);
extern int _ZTV19dScMgSingle3DBase_c;
extern int _ZTV11dScMgCard_c[];
extern int _ZTV12dMgCardObj_c[];
extern int _ZTV17dMgDilarCardObj_c[];
void _ZN12dMgCardObj_cC1Ev(void*);
void _ZN17dMgDilarCardObj_cC1Ev(void*);
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020da8e4, from the legacy file for func_ov006_020d9c5c, NOT applied: int func_ov006_020da8e4(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020da8e4, from the legacy file for func_ov006_020da00c, NOT applied: extern "C" int func_ov006_020da8e4(void); */
/* TUBUILD CONFLICT -- alternate declaration of Vec2_Sub, from the legacy file for func_ov006_020da0ac, NOT applied: extern "C" void Vec2_Sub(int *o, int *a, int *b); */
/* TUBUILD CONFLICT -- alternate declaration of func_0203d680, from the legacy file for func_ov006_020da0ac, NOT applied: extern "C" void func_0203d680(int *out, int *in, int scale); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020da8e4, from the legacy file for func_ov006_020da0ac, NOT applied: extern "C" int func_ov006_020da8e4(void); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020da834, from the legacy file for func_ov006_020dac34, NOT applied: extern int func_ov006_020da834(void *p); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020da860, from the legacy file for func_ov006_020dac34, NOT applied: extern int func_ov006_020da860(void *p, int v); */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020c1718, from the legacy file for _ZN11dScMgCard_c13OnTurnIntoEggEi, NOT applied: extern int func_ov006_020c1718(void* p); */
/* TUBUILD CONFLICT -- alternate declaration of data_ov006_02134028, from the legacy file for _ZN11dScMgCard_c13InitResourcesEv, NOT applied: extern u32 *data_ov006_02134028; */
/* TUBUILD CONFLICT -- alternate declaration of func_ov006_020c0aa8, from the legacy file for _ZN11dScMgCard_c13InitResourcesEv, NOT applied: extern void func_ov006_020c0aa8(char *); */
}

// @symbol _ZN12dMgCardObj_cC1Ev
extern "C" void _ZN12dMgCardObj_cC1Ev(void* elem)
{
    *(int*)elem = (int)&_ZTV12dMgCardObj_c[2];
}

// @symbol _ZN17dMgDilarCardObj_cC1Ev
extern "C" void _ZN17dMgDilarCardObj_cC1Ev(void* elem)
{
    *(int*)elem = (int)&_ZTV12dMgCardObj_c[2];
    *(int*)elem = (int)&_ZTV17dMgDilarCardObj_c[2];
}

// @symbol dScMgCard_c_classInit
extern "C" void* dScMgCard_c_classInit()
{
    char* p = (char*)_ZN7fBase_cnwEj(sizeof(dScMgCard_c));
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        *(int*)p = (int)&_ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(p + 0x471c);
        *(int*)p = (int)&_ZTV11dScMgCard_c[2];
        func_ov006_020c1d80(p + 0x4f38);
        __cxa_vec_ctor(p + 0x51a8, 5, 0x30,
                     _ZN12dMgCardObj_cC1Ev,
                     _ZN12dMgCardObj_cD1Ev);
        __cxa_vec_ctor(p + 0x5298, 5, 0x30,
                     _ZN17dMgDilarCardObj_cC1Ev,
                     _ZN17dMgDilarCardObj_cD1Ev);
    }
    return p;
}

/* Engine calls by their C++ names. The parameter types are read off the
   ROM's mangled names: `j` unsigned int, `t` unsigned short, `s` short,
   `Riii` int reference, `Rsss` short reference. */
namespace GX { void LoadOBJPltt(const void *plt, u32 base, u32 size); }
namespace GXS { void LoadOBJPltt(const void *plt, u32 base, u32 size); }
namespace G2x { void SetBlendAlpha(volatile u16 *reg, u16 a, u16 b, u16 c, u32 d); }
int  ApproachLinear(int &value, int target, int step);
int  ApproachLinear2(s16 &value, s16 target, s16 step);

// @symbol _ZN11dScMgCard_c13InitResourcesEv
/* Slot 0. Loads and decompresses the OBJ tiles and palettes for both
 * engines, sets blending, patches the OAM template list, spawns the two rows
 * of five card sprites and resets the shared counters. It ends with a
 * virtual call to OnYoshiTryEat(-1), which resets the round. */
/* One OAM attribute-template entry: the list is a u32 attr word, then the two
 * 16-bit attrs, then an 8-byte stride. Spelled as a struct rather than as casts
 * off a u32* because the ROM addresses all three off ONE base register at
 * displacements 0/4/6; `*((u16 *)e + 2)` makes b56 materialise a second base
 * (`add r2, r3, #4`), and that one extra instruction moves the literal pool. */
s32 dScMgCard_c::InitResources()
{
    dScMgCard_c *self = this;
    char *c = (char *)this;
    void *mainTiles, *mainPalette, *subTiles, *subPalette;
    int pos[2];

    func_ov004_020b04d0(0x20);
    func_ov006_0210a534(c);
    data_0209d45c = 0x11;
    data_0209d454 = 0x18;
    mainTiles = LoadFile(0xbd);
    mainPalette = LoadFile(0xbe);
    subTiles = LoadFile(data_ov006_0213bcb0[GetGameLanguage()]);
    subPalette = LoadFile(0xbb);
    DecompressLZ16(mainTiles, (void *)0x6400000);
    GX::LoadOBJPltt(mainPalette, 0, 0x20);
    DecompressLZ16(subTiles, (void *)0x6600000);
    GXS::LoadOBJPltt(subPalette, 0, 0x100);
    Deallocate(mainTiles);
    Deallocate(mainPalette);
    Deallocate(subTiles);
    Deallocate(subPalette);
    G2x::SetBlendAlpha((volatile u16 *)0x04000050, 0, 1, 0x10, 8);
    *(volatile u16 *)0x04001050 = 0;

    {
        OamAttrTmpl *entry = (OamAttrTmpl *)data_ov006_02134028;
        u16 last;
        do {
            entry->attr0 = (entry->attr0 & ~0xc00) | 0x400;
            /* NOT `entry->attr2 &= ~0xf000;`. Compound assignment makes b56 CSE the
             * field's address and materialise it (`add r2, r3, #4`), then reach
             * attr3 at +2 off that; the ROM keeps ONE base and uses +4 / +6.
             * The extra instruction moves the literal pool and shifts every
             * pc-relative load in the function. */
            u16 bits = entry->attr2;
            entry->attr2 = (u16)(bits & ~0xf000);
            last = entry->attr3;
            entry++;
        } while (last != 0xffff);
    }

    *(volatile u16 *)0x04000008 = (*(volatile u16 *)0x04000008 & ~3) | 1;

    func_ov006_020c0aa8(pad_4660);
    if (func_ov006_020c1a88((char *)&mShared) == 0)
        return 0;

    {
        int y = 0x2c;
        int i;
        for (i = 0; i < 5; i++) {
            pos[0] = y << 12;
            pos[1] = 0x80000;
            mArray1[i].DealIn(pos);
            pos[1] = 0x10000;
            mArray2[i].DealIn(pos);
            y += 0x28;
        }
    }

    self->mState = 0;
    self->mStateTimer = 0;
    unk_0a8 = func_ov004_020ad8b8();
    unk_0ac = unk_0a8;
    {
        int r = func_ov004_020ad878();
        if (data_ov004_020beb68 != 0)
            ((dScMgBase_c *)data_ov004_020beb68)->mHudScore = r;
    }
    self->mScore = 0;
    func_ov004_020b682c();
    OnYoshiTryEat(-1);
    return 1;
}

// @symbol _ZN11dScMgCard_c13OnYoshiTryEatEi
/* Slot 18 resets the round: clear the selected-card counter and highlights,
 * restore the face weights, redeal both hands, and restart at state 1.
 * The inherited actor-style name is retained for the existing interface;
 * it does not establish this scene callback's original name or event. */
void dScMgCard_c::OnYoshiTryEat(int mode)
{
    int i;
    char *p1, *p2;

    unk_538c = 0;
    unk_538e = 6;
    unk_5390 = 6;
    unk_5392 = 6;
    unk_5394 = 6;
    func_ov006_020c1604((char *)&mShared, 4, 5, (int)&unk_538c);

    mShared.unk_1e6 = 1;
    mShared.unk_01a = 2;
    dScMgCard_c::FillWeights(5);

    p1 = (char *)mArray1;
    p2 = (char *)mArray2;
    for (i = 0; i < 5; i++) {
        ((dMgCardObj_c *)p1)->Redeal();
        ((dMgCardObj_c *)p2)->Redeal();
        p1 += 0x30;
        p2 += 0x30;
    }

    func_ov004_020b66d4();

    data_ov004_020bc7d4 = 1;
    data_ov006_0214176c = 0;
    data_ov006_02141768 = 0;
    data_ov006_02141770 = 0;
    unk_539a = 0;
    data_ov006_0213bc44 = 1;
    mState = 1;
}

// @symbol _ZN11dScMgCard_c13OnTurnIntoEggEi
/* Slot 19 advances the round-end sequence: show the comparison result,
 * settle the score, fly both hands off, and wait for the final timer.
 * Returns 1 when complete. The inherited actor-style name does not imply
 * that a Picture Poker player has been swallowed. */
int dScMgCard_c::OnTurnIntoEgg(int mode)
{
    switch (mState) {
    case 0xe: {
        int r = dScMgCard_c::CompareHands((const dMgCardObj_c *)mArray1, (const dMgCardObj_c *)mArray2);
        if (r == -1)
            FreeGfxSlotsById(0xa);
        else if (r == 1)
            FreeGfxSlotsById(9);
        mStateTimer = 0;
        mState += 1;
        break;
    }
    case 0xf:
        if (func_ov006_020c1718(&mShared) != 0) {
            int r = dScMgCard_c::CompareHands((const dMgCardObj_c *)mArray1, (const dMgCardObj_c *)mArray2);
            if (r == -1) {
                func_ov004_020b5ed0();
                mStateTimer = 0;
            } else if (r == 1) {
                int v = data_ov006_0213bd48[dScMgCard_c::EvaluateHand((const dMgCardObj_c *)mArray1, 0)];
                func_ov004_020b56c8(v * data_ov004_020bfa18);
                mStateTimer = 0;
            } else {
                func_ov004_020b5d74();
                mStateTimer = 0x1e;
            }
            mState += 1;
        }
        break;
    case 0x10:
        if (data_ov004_020bf9e4 == 1) {
            int r = dScMgCard_c::CompareHands((const dMgCardObj_c *)mArray1, (const dMgCardObj_c *)mArray2);
            dScMgCard_c::EvaluateHand((const dMgCardObj_c *)mArray1, 0);
            if (r == -1) {
                mState += 1;
            } else {
                return 1;
            }
        } else {
            if (ApproachLinear2(mStateTimer, 0, 1) != 0)
                FreeGfxSlotsById(0x1d);
        }
        break;
    case 0x11: {
        int i;
        char* a;
        char* b;
        void* p = data_ov004_020beb68;
        if ((p != 0 ? ((dScMgBase_c *)p)->unk_0a8 : 0) != 0)
            return 1;
        a = (char *)mArray1;
        b = (char *)mArray2;
        for (i = 0; i < 5; i++) {
            ((dMgCardObj_c *)a)->FlyOff();
            ((dMgCardObj_c *)b)->FlyOff();
            a += 0x30;
            b += 0x30;
        }
        mStateTimer = 0x3c;
        mState += 1;
        break;
    }
    case 0x12:
        if (ApproachLinear2(mStateTimer, 0, 1) != 0)
            return 1;
        break;
    }
    return 0;
}

// @symbol _ZN11dScMgCard_c15OnGroundPoundedEv
/* Slot 21 selects a tier from 1 to 4 according to mHudScore and passes it
 * to func_ov004_020b6324. The inherited name does not establish an actor
 * ground-pound event or the original name of this scene callback. */
void dScMgCard_c::OnGroundPounded() {
    int x = mHudScore;
    int v;
    if (x < 5) {
        v = 1;
    } else if (x < 10) {
        v = 2;
    } else if (x < 15) {
        v = 3;
    } else {
        v = 4;
    }
    func_ov004_020b6324(v);
}

// @symbol _ZN11dScMgCard_c11UpdateStateEv
/* dScMgCard_c::UpdateState -- the round's own state machine, one long
 * switch on mState that Behavior drives every frame; the tail past the
 * switch Updates all ten cards, both hands, every frame. Steps 1-5 arm
 * the prompt and read the touched slot, mirroring the pick onto the
 * matching card of BOTH hands and waiting for the lift; 6-8 run the
 * dealer's reveal the same way; 9-13 settle the round -- arrange both
 * hands, flip and lift them, evaluate and compare them, then pay the
 * win out or take the loss's chip. mState mostly just `++`s and
 * mStateTimer carries each step's countdown; the two highlight pairs
 * and the card states carry the rest. The singleton
 * data_ov004_020beb68 is the active dScMgBase_c (see its own note in
 * notes/minigame-provenance.md): unk_0a8 is the coin counter,
 * mHudScore the chip tally, unk_0b8 its high-water mark. Coined name;
 * the ROM's symbol is address-only. */
void dScMgCard_c::UpdateState()
{
    int cnt3;
    int i;
    int j;
    int k;
    int n;
    unsigned char *p;
    unsigned char *p1;
    unsigned char *p2;
    int ok;
    int idx;
    int v5;
    int v4;
    int r;
    int a;
    int b;
    int t4;
    int t5;
    void *g;
    int a0;
    int a1;
    s16 cnt;

    switch (mState) {
    case 1:
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        if (func_ov006_020c0f0c(&mShared) != 0)
            mState++;
        break;
    case 2:
        if (func_ov006_020c1718(&mShared) != 0) {
            mStateTimer = 0x10;
            mState++;
        } else {
            cnt = unk_538c;
            if (cnt > 0) {
                idx = cnt - 1;
                if (cnt != 0) {
                    idx = idx * 0x30;
                    p = &mArray1[0].mState;
                    if (p[idx] == 0) {
                        p[idx] = 1;
                        ((dMgCardObj_c *)((char *)mArray2 + idx))->mState = 1;
                    }
                }
            }
        }
        break;
    case 3:
        mStateTimer--;
        if (mStateTimer == 0)
            mState++;
        break;
    case 4:
        ok = 0;
        idx = data_020a0e40 * 4;
        if (data_020a0de8[idx] != 0 && data_020a0de9[idx] != 0)
            ok = 1;
        if (ok != 0 && dScMgCard_c::AllLifted((const dMgCardObj_c *)mArray1) != 0 && data_ov006_02141768 == 0) {
            v5 = data_020a0dea[data_020a0e40][0];
            v4 = data_020a0deb[data_020a0e40][0];
            if (dScMgCard_c::AllInState((const dMgCardObj_c *)mArray1, 2) != 0) {
                r = v5 - 0x80;
                if (r < 0)
                    r = -r;
                if (r < 0x48) {
                    r = v4 - 0x58;
                    if (r < 0)
                        r = -r;
                    if (r < 0x14) {
                        data_ov006_0214176c = 1;
                        data_ov006_02141768 = 1;
                        mState++;
                        data_ov004_020bf9ec = 1;
                        Sound::PlayBank2_2D(0x151);
                        g = data_ov004_020beb68;
                        a0 = g ? ((dScMgBase_c *)g)->unk_0a8 : 0;
                        a1 = g ? ((dScMgBase_c *)g)->mHudScore : 0;
                        func_ov004_020ad79c(a0, a1);
                    }
                }
            } else {
                r = v5 - 0x80;
                if (r < 0)
                    r = -r;
                if (r < 0x48) {
                    r = v4 - 0x58;
                    if (r < 0)
                        r = -r;
                    if (r < 0x14) {
                        data_ov006_0214176c = 1;
                        data_ov006_02141768 = 1;
                        mState++;
                        data_ov004_020bf9ec = 1;
                        Sound::PlayBank2_2D(0x151);
                        g = data_ov004_020beb68;
                        a0 = g ? ((dScMgBase_c *)g)->unk_0a8 : 0;
                        a1 = g ? ((dScMgBase_c *)g)->mHudScore : 0;
                        func_ov004_020ad79c(a0, a1);
                        cnt3 = 0;
                        i = cnt3;
                        do {
                            r = mArray1[i].mState;
                            i++;
                            if (r == 3)
                                cnt3++;
                        } while (i < 5);
                        if (cnt3 < 2)
                            Sound::PlayBank2_2D(0x144);
                        else
                            Sound::PlayBank2_2D(0x145);
                    }
                }
            }
        }
        break;
    case 5:
        if (data_ov006_02141768 != 0 && dScMgCard_c::AllInState((const dMgCardObj_c *)mArray1, 2) != 0
            && func_ov006_020c1718(&mShared) != 0) {
            mState++;
        } else if (func_ov006_020c1718(&mShared) != 0) {
            r = dScMgCard_c::CountInState((const dMgCardObj_c *)mArray1, 6);
            if (r != 0) {
                unk_538c = 0;
                mShared.unk_1e6 = 0;
                func_ov006_020c1420(&mShared, (s16)r, &unk_538c);
            }
        } else {
            r = dScMgCard_c::FindInState((const dMgCardObj_c *)mArray1, 6);
            if (r > -1 && unk_538c != 0) {
                ((dMgCardObj_c *)mArray1)[r].mState = 5;
                unk_538c--;
            }
        }
        break;
    case 6:
        dScMgCard_c::MarkSingles((dMgCardObj_c *)mArray2);
        mState++;
        break;
    case 7:
        if (dScMgCard_c::AllInState((const dMgCardObj_c *)mArray2, 1) != 0 && func_ov006_020c1718(&mShared) != 0) {
            r = data_ov006_0213bc44 - 1;
            data_ov006_0213bc44 = r;
            if (r == 0) {
                mState++;
            } else {
                mState = 4;
                data_ov006_0214176c = 0;
                data_ov006_02141768 = 0;
                data_ov006_02141770 = 0;
            }
        } else if (func_ov006_020c1718(&mShared) != 0) {
            r = dScMgCard_c::CountInState((const dMgCardObj_c *)mArray2, 6);
            if (r != 0) {
                unk_538c = 0;
                mShared.unk_1e6 = 0;
                func_ov006_020c1420(&mShared, (s16)r, &unk_538c);
            }
        } else {
            r = dScMgCard_c::FindInState((const dMgCardObj_c *)mArray2, 6);
            if (r > -1 && unk_538c != 0) {
                ((dMgCardObj_c *)mArray2)[r].mState = 5;
                unk_538c--;
            }
        }
        break;
    case 8:
        j = 0;
        mPromptEnabled = (unsigned char)j;
        do {
            mArray2[j].mState = 2;
            j++;
        } while (j < 5);
        mState++;
        break;
    case 9:
        a = dScMgCard_c::ArrangeHand((dMgCardObj_c *)mArray1);
        b = dScMgCard_c::ArrangeHand((dMgCardObj_c *)mArray2);
        if (a != 0 || b != 0)
            Sound::PlayBank2_2D(0x152);
        mState++;
        break;
    case 10:
        if (dScMgCard_c::AllInState((const dMgCardObj_c *)mArray1, 8) != 0 && dScMgCard_c::AllInState((const dMgCardObj_c *)mArray2, 8) != 0) {
            mStateTimer = 0x14;
            mState++;
        }
        break;
    case 11:
        mStateTimer--;
        if (mStateTimer == 0) {
            k = 0;
            do {
                mArray1[k].mLiftFlag = 1;
                mArray2[k].mLiftFlag = 1;
                k++;
            } while (k < 5);
            dScMgCard_c::SettleHand((dMgCardObj_c *)mArray1);
            dScMgCard_c::SettleHand((dMgCardObj_c *)mArray2);
            Sound::PlayBank2_2D(0x147);
            mStateTimer = 0x3c;
            mState++;
        }
        break;
    case 12:
        mStateTimer--;
        if (mStateTimer == 0) {
            t4 = dScMgCard_c::EvaluateHand((const dMgCardObj_c *)mArray2, &unk_5392);
            t5 = dScMgCard_c::EvaluateHand((const dMgCardObj_c *)mArray1, &unk_538e);
            if (data_ov006_02141768 != 0 && data_ov006_02141768 != 0) {
                /* the hand-value fanfares sit at sound bank 22 up */
                func_ov004_020b0cac(t4 + 22, 0x80, 0x48, 0, -1, 1);
                func_ov004_020b0cac(t5 + 22, 0x80, 0x78, 0, -1, 2);
                if (t4 != t5) {
                    unk_5392 = 6;
                    unk_5394 = 6;
                    unk_538e = 6;
                    unk_5390 = 6;
                } else if (unk_5392 != unk_538e) {
                    unk_5394 = 6;
                    unk_5390 = 6;
                }
            }
            r = dScMgCard_c::CompareHands((const dMgCardObj_c *)mArray1, (const dMgCardObj_c *)mArray2);
            if (r == -1) {
                g = data_ov004_020beb68;
                a0 = g ? ((dScMgBase_c *)g)->unk_0a8 : 0;
                a1 = g ? ((dScMgBase_c *)g)->mHudScore : 0;
                func_ov004_020ad79c(a0, a1 - 1);
                mStateTimer = 0x5a;
            } else if (r == 1) {
                g = data_ov004_020beb68;
                {
                    int *tbl = data_ov006_0213bd48;
                    int scale;
                    int prod;
                    a0 = g ? ((dScMgBase_c *)g)->unk_0a8 : 0;
                    r = tbl[t5];
                    scale = data_ov004_020bfa18;
                    prod = r * scale + a0;
                    r = g ? ((dScMgBase_c *)g)->mHudScore : 0;
                    func_ov004_020ad79c(prod, r + 1);
                }
                mStateTimer = 0x1e;
            } else {
                mStateTimer = 0x5a;
            }
            mState++;
        }
        break;
    case 13:
        mStateTimer--;
        if (mStateTimer == 0) {
            r = dScMgCard_c::CompareHands((const dMgCardObj_c *)mArray1, (const dMgCardObj_c *)mArray2);
            if (r == -1) {
                func_ov006_020c0d68(&mShared);
                g = data_ov004_020beb68;
                if (g != 0) {
                    if (((dScMgBase_c *)g)->mHudScore > 0) {
                        ((dScMgBase_c *)g)->mHudScore = ((dScMgBase_c *)g)->mHudScore - 1;
                    }
                }
                func_ov004_020b0a54(5);
            } else if (r == 1) {
                func_ov006_020c0c80(&mShared);
                g = data_ov004_020beb68;
                if (g != 0) {
                    if (((dScMgBase_c *)g)->mHudScore < 0x270f) {
                        int *fld = &((dScMgBase_c *)g)->mHudScore;
                        *fld = *fld + 1;
                    }
                    a1 = ((dScMgBase_c *)g)->mHudScore;
                    if (a1 > ((dScMgBase_c *)g)->unk_0b8)
                        ((dScMgBase_c *)g)->unk_0b8 = a1;
                }
                func_ov004_020adb1c(mHudScore);
                func_ov004_020b0a54(4);
            } else {
                func_ov004_020b0a54(7);
                Sound::PlayBank2_2D(0x140);
                Sound::PlayBank2_2D(0x138);
            }
            mPromptEnabled = 0;
            mStateTimer = 0;
            mState++;
        }
        break;
    }

    p1 = (unsigned char *)mArray1;
    p2 = (unsigned char *)mArray2;
    n = 0;
    do {
        ((dMgCardObj_c *)p1)->Update();
        ((dMgCardObj_c *)p2)->Update();
        n++;
        p1 += 0x30;
        p2 += 0x30;
    } while (n < 5);
    data_ov006_0214176c = 0;
}

// @symbol _ZN11dScMgCard_c8BehaviorEv
/* Slot 6. mShared is the 0x270-byte table this family of scenes shares;
 * func_ov006_020c19d0 steps it. */
s32 dScMgCard_c::Behavior()
{
    mFrameCounter += 1;
    func_ov006_020c19d0(&mShared);
    UpdateState();
    func_ov004_020b65e4();
    return 1;
}

// @symbol _ZN11dScMgCard_c6RenderEv
/* dScMgCard_c::Render -- vtable slot 9.
 *
 * Attributed by tools/rtti_vtables.py --own dScMgCard_c, this class's own slot 9
 * (dScMgBase_c::Render). */
s32 dScMgCard_c::Render()
{
    char *c = (char *)this;
    int hiddenRank;
    short state;
    int i;
    int y;

    func_ov006_020c0aa8(pad_4660);

    state = this->mState;
    if (state > 3 && state < 0x11) {
        hiddenRank = -1;
        if (state > 0xe) {
            if (this->mFrameCounter & 8) {
                if (dScMgCard_c::CompareHands((const dMgCardObj_c *)mArray1, (const dMgCardObj_c *)mArray2) == 1) {
                    hiddenRank = 6 - dScMgCard_c::EvaluateHand((const dMgCardObj_c *)mArray1, 0);
                }
            }
        }

        i = 0;
        y = 0xc;
        for (; i < 6; i++) {
            if (i != hiddenRank)
                RenderOamMainScreen(data_ov006_02134010[i], 0x28, y, -1, -1);
            y += 0x10;
        }

        RenderOamMainScreen(data_ov006_02134028, 0x28, 0x38, -1, -1);
    }

    func_ov004_020b1bc8(c, 0xc, 0xc, 0);
    func_ov004_020b1e34(c, 0xe0, 0x14, 1);

    if (this->mScore < this->mHudScore)
        this->mScore = this->mHudScore;
    func_ov004_020adb1c(this->mScore);

    func_ov004_020b6430();

    RenderHud();

    {
        /* The player's five cards, element 4 first: a card outside the marked
         * pair always draws, a marked one draws on the blink phase only.
         * Spelled as indexed member access -- mValue read, Render() called
         * through the element's own vtable, k walking 4 down to 0 -- which
         * matches the cartridge byte-identically (0x228/0x228). The folded
         * one-walker member form (a typed pointer stepping back 0x30 a
         * card) is the spelling that measurably moves bytes (0x210), and is
         * not used. */
        int k;
        for (k = 4; k >= 0; k--) {
            unsigned char face = mArray1[k].mValue;
            if (this->unk_538e == face)
                goto chk1;
            if (this->unk_5390 != face)
                goto docall1;
        chk1:
            if (!(this->mFrameCounter & 8))
                goto skip1;
        docall1:
            mArray1[k].Render();
        skip1: ;
        }
    }

    {
        /* the dealer's five: same walk, the dealer's pair */
        int k;
        for (k = 4; k >= 0; k--) {
            unsigned char face = mArray2[k].mValue;
            if (this->unk_5392 == face)
                goto chk2;
            if (this->unk_5394 != face)
                goto docall2;
        chk2:
            if (!(this->mFrameCounter & 8))
                goto skip2;
        docall2:
            mArray2[k].Render();
        skip2: ;
        }
    }

    func_ov006_020c1804(&mShared);
    return 1;
}

// @symbol _ZN11dScMgCard_c16CleanupResourcesEv
/* Slot 3. Only clears three ov006 globals; `this` is never read. */
s32 dScMgCard_c::CleanupResources()
{
    data_ov006_0214176c = 0;
    data_ov006_02141768 = 0;
    data_ov006_02141770 = 0;
    return 1;
}

// @symbol _ZN11dScMgCard_c11FillWeightsEi
/* dScMgCard_c::FillWeights -- sets every one of the six card-face weights to
 * the same value: the deal code passes 5 ("all six faces equally likely"),
 * the win/lose branches pass 0 (the deck goes dead). Coined name; the ROM's
 * symbol is address-only. */
void dScMgCard_c::FillWeights(int v) {
    int i;
    for (i = 0; i < 6; i++) {
        data_ov006_0213bd18[i] = v;
    }
}

// @symbol _ZN11dScMgCard_c13DrawCardValueEv
/* dScMgCard_c::DrawCardValue -- one weighted draw from the six-face deck:
 * roll the seeded RNG scaled by the weight total, then walk the weights
 * subtracting until the running sum goes negative -- that face is the draw,
 * and its weight is spent. Coined name; the ROM's symbol is address-only. */
int dScMgCard_c::DrawCardValue(void)
{
  int value = 0;
  int sum = 0;
  int i;
  for (i = 0; i < 6; i++)
  {
    sum += data_ov006_0213bd18[i];
  }

  sum = (sum * ((int) ((((unsigned int) RandomIntInternal(&data_0209e650)) & 0x7fffffff) >> 0x13))) >> 0xc;
  for (i = 0; i < 6; i++)
  {
    sum -= data_ov006_0213bd18[i];
    if (sum < 0)
    {
      value = i & 0xff;
      data_ov006_0213bd18[i] -= 1;
      break;
    }
  }

  return value;
}

// @symbol _ZN11dScMgCard_c11FindInStateEPK12dMgCardObj_ci
/* dScMgCard_c::FindInState -- index of the first card in the hand sitting in
 * the given state, or -1 when no card is. Coined name; the ROM's symbol is
 * address-only. */
int dScMgCard_c::FindInState(const dMgCardObj_c *hand, int state)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (state == hand->mState)
            return i;
        hand++;
    }
    return -1;
}

// @symbol _ZN11dScMgCard_c12CountInStateEPK12dMgCardObj_ci
/* dScMgCard_c::CountInState -- how many of the hand's five cards sit in the
 * given state. Coined name; the ROM's symbol is address-only. */
int dScMgCard_c::CountInState(const dMgCardObj_c *hand, int state)
{
    int count = 0;
    int i;
    for (i = 0; i < 5; i++) {
        if (state == hand->mState)
            count++;
        hand++;
    }
    return count;
}

// @symbol _ZN11dScMgCard_c10AllInStateEPK12dMgCardObj_ci
/* dScMgCard_c::AllInState -- true only when every card of the hand sits in
 * the given state. Coined name; the ROM's symbol is address-only. */
int dScMgCard_c::AllInState(const dMgCardObj_c *hand, int state)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (state != hand->mState)
            return 0;
        hand++;
    }
    return 1;
}

// @symbol _ZN11dScMgCard_c9AllLiftedEPK12dMgCardObj_c
/* dScMgCard_c::AllLifted -- true only when every card of the hand is raised
 * to the full 0x4000 lift. Coined name; the ROM's symbol is address-only. */
int dScMgCard_c::AllLifted(const dMgCardObj_c *hand)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (hand->mLift < 0x4000)
            return 0;
        hand++;
    }
    return 1;
}

// @symbol _ZN11dScMgCard_c12CompareHandsEPK12dMgCardObj_cS2_
/* dScMgCard_c::CompareHands -- poker comparison of the two five-card hands:
 * histogram each hand's face counts, find each hand's strongest count and
 * its runner-up (preferring the higher face index on equal counts), then
 * compare strongest count, runner-up count,
 * strongest face, runner-up face, in that order. Singleton face values do
 * not break ties. Coined name; the ROM's symbol is address-only. */
int dScMgCard_c::CompareHands(const dMgCardObj_c *handA, const dMgCardObj_c *handB)
{
    short countsA[6];
    short countsB[6];
    /* Integer round-trips preserve the pinned compiler's histogram stores. */
    short *countA = (short *)(int)countsA;
    short *countB = (short *)(int)countsB;
    int bestCountA, nextCountA;
    short bestFaceA, nextFaceA;
    int bestCountB, nextCountB;
    short bestFaceB, nextFaceB;
    int i;
    short faceTemp;

    countA[0] = 0;
    countA[1] = 0;
    countA[2] = 0;
    countA[3] = 0;
    countA[4] = 0;
    countA[5] = 0;
    countB[0] = 0;
    countB[1] = 0;
    countB[2] = 0;
    countB[3] = 0;
    countB[4] = 0;
    countB[5] = 0;

    bestCountA = 0;
    bestFaceA = 6;
    nextCountA = 0;
    nextFaceA = 6;
    bestCountB = 0;
    nextCountB = 0;
    bestFaceB = 6;
    nextFaceB = 6;

    for (i = 0; i < 5; i++) {
        countsA[handA->mValue]++;
        handA++;
        countsB[handB->mValue]++;
        handB++;
    }

    for (i = 0; i < 6; i++) {
        if (bestCountA <= countsA[i]) {
            nextCountA = bestCountA;
            nextFaceA = bestFaceA;
            bestCountA = countsA[i];
            bestFaceA = i;
        } else if (nextCountA <= countsA[i]) {
            nextCountA = countsA[i];
            nextFaceA = i;
        }
        if (bestCountB <= countsB[i]) {
            nextCountB = bestCountB;
            nextFaceB = bestFaceB;
            bestCountB = countsB[i];
            bestFaceB = i;
        } else if (nextCountB <= countsB[i]) {
            nextCountB = countsB[i];
            nextFaceB = i;
        }
    }

    if (bestCountA == nextCountA && bestFaceA < nextFaceA) {
        faceTemp = bestFaceA;
        bestFaceA = nextFaceA;
        nextFaceA = faceTemp;
    }
    if (bestCountB == nextCountB && bestFaceB < nextFaceB) {
        faceTemp = bestFaceB;
        bestFaceB = nextFaceB;
        nextFaceB = faceTemp;
    }

    if (bestCountA > bestCountB) return 1;
    if (bestCountA < bestCountB) return -1;
    if (nextCountA > nextCountB) return 1;
    if (nextCountA < nextCountB) return -1;
    if (bestCountA == 1 && bestCountB == 1) return 0;
    if (bestFaceA > bestFaceB) return 1;
    if (bestFaceA < bestFaceB) return -1;
    if (nextCountA == 1 && nextCountB == 1) return 0;
    if (nextFaceA > nextFaceB) return 1;
    if (nextFaceA < nextFaceB) return -1;
    return 0;
}

// @symbol _ZN11dScMgCard_c12EvaluateHandEPK12dMgCardObj_cPs
/* dScMgCard_c::EvaluateHand -- score one hand on the poker scale (nothing,
 * one pair, two pair, three of a kind, full house, four, five), and when out
 * is non-null, name the faces of the winning combination into it. Coined
 * name; the ROM's symbol is address-only. */
int dScMgCard_c::EvaluateHand(const dMgCardObj_c *hand, s16 *out)
{
    s16 counts[6] = {0};
    int i;
    int j;
    int result = 0;
    int bestCount = 0;
    int nextCount = 0;
    s16 bestFace = 6;
    s16 nextFace = bestFace;

    for (i = 0; i < 5; i++) {
        counts[hand->mValue]++;
        hand++;
    }

    for (j = 0; j < 6; j++) {
        s16 count = counts[j];
        if (bestCount <= count) {
            nextCount = bestCount;
            nextFace = bestFace;
            bestCount = count;
            bestFace = j;
        } else if (nextCount <= count) {
            nextCount = count;
            nextFace = j;
        }
    }

    if (bestCount == nextCount && bestFace < nextFace) {
        s16 faceTemp = bestFace;
        bestFace = nextFace;
        nextFace = faceTemp;
    }

    switch (bestCount) {
    case 2:
        result = (nextCount == 2) ? 2 : 1;
        break;
    case 3:
        result = (nextCount == 2) ? 4 : 3;
        break;
    case 4:
        result = 5;
        break;
    case 5:
        result = 6;
        break;
    default:
        break;
    }

    if (out != 0) {
        if (bestCount > 1)
            out[0] = (s16)bestFace;
        if (nextCount > 1)
            out[1] = (s16)nextFace;
    }
    return result;
}

// @symbol _ZN11dScMgCard_c11MarkSinglesEP12dMgCardObj_c
/* dScMgCard_c::MarkSingles -- stamp every card whose face appears exactly
 * once in the hand to state 4: the settle step for the cards left out of
 * the winning combination. Coined name; the ROM's symbol is address-only. */
void dScMgCard_c::MarkSingles(dMgCardObj_c *hand){
    int i;
    dMgCardObj_c *card;
    short counts[6];
    short *count = (short *)(int)counts;
    count[0] = 0;
    count[1] = 0;
    count[2] = 0;
    count[3] = 0;
    count[4] = 0;
    count[5] = 0;
    card = hand;
    for (i = 0; i < 5; i++) {
        counts[card->mValue]++;
        card++;
    }
    for (i = 0; i < 5; i++) {
        if (counts[hand->mValue] == 1)
            hand->mState = 4;
        hand++;
    }
}

// @symbol _ZN11dScMgCard_c11ArrangeHandEP12dMgCardObj_c
/* dScMgCard_c::ArrangeHand -- the sort that pulls a scored hand together:
 * histogram the faces, decide the winning combination's two faces (the
 * stronger face first), then bubble the five cards by combination rank
 * through the order table at data_ov006_0213bcd8, stamp the combination
 * cards' mComboFlag, and SetHome every card to its sorted slot. Returns
 * whether the order changed at all. Coined name; the ROM's symbol is
 * address-only. */
int dScMgCard_c::ArrangeHand(dMgCardObj_c *hand)
{
  s16 counts[6];
  Five order;
  s16 i;
  s32 j;
  s32 t;
  s32 n;
  s32 neg1;
  s32 neg2;
  s32 zero;
  dMgCardObj_c *p;
  s16 *count;
  s32 bestCount;
  s32 nextCount;
  s32 bestFace;
  s32 nextFace;
  bestCount = 0;
  count = (s16 *)(u32)counts;
  count[0] = bestCount;
  count[1] = bestCount;
  count[2] = bestCount;
  count[3] = bestCount;
  count[4] = bestCount;
  bestFace = 6;
  nextCount = bestCount;
  nextFace = bestFace;
  i = bestCount;
  count[5] = bestCount;
  zero = bestCount;
  for (i = 0; i < 5; i++)
  {
    s32 idx = hand[i].mValue;
    counts[idx] = (s16) (counts[idx] + 1);
  }
  p = hand;

  for (i = 0; i < 6; i++)
  {
    s32 v = counts[i];
    if (bestCount <= v)
    {
      nextCount = bestCount;
      nextFace = bestFace;
      bestCount = v;
      bestFace = i;
    }
    else
      if (nextCount <= v)
    {
      nextCount = v;
      nextFace = i;
    }
  }

  if (bestCount == nextCount)
  {
    if (bestCount > 1)
    {
      if (nextCount > 1)
      {
        p = hand;
        for (t = 0; t < 5; t++)
        {
          s32 c = p->mValue;
          if (bestFace == c)
          {
            break;
          }
          if (nextFace == c)
          {
            s32 tmp = nextFace;
            nextFace = bestFace;
            bestFace = tmp;
            break;
          }
          p++;
        }

      }
    }
  }
  order = data_ov006_0213bcd8;
  n = 4;
  zero = 0;
  neg1 = -1;
  neg2 = -2;

  {
    do
    {
      s32 k = 0;
      while (k < n)
      {
        s32 v1 = order.v[k];
        dMgCardObj_c *ptr1 = &hand[v1];
        s32 u1 = ptr1->mValue;
        s32 next = k + 1;
        s32 v2;
        dMgCardObj_c *ptr2 = &hand[order.v[next]];
        s32 u2 = ptr2->mValue;
        if (u1 == bestFace)
        {
          if (bestCount > 1)
          {
            u1 = neg2;
            goto u1_done;
          }
        }
        if (u1 == nextFace)
        {
          if (nextCount > 1)
          {
            u1 = neg1;
            goto u1_done;
          }
        }
        u1 = zero;
        u1_done:
        if (u2 == bestFace)
        {
          if (bestCount > 1)
          {
            u2 = neg2;
            goto u2_done;
          }
        }

        if (u2 == nextFace)
        {
          if (nextCount > 1)
          {
            u2 = neg1;
            goto u2_done;
          }
        }
        u2 = zero;
        u2_done:
        if (u1 > u2)
        {
          order.v[k] = order.v[next];
          order.v[next] = v1;
        }

        k++;
      }

      n = n - 1;
    }
    while (n > 0);
  }

  {
    dMgCardObj_c *q = hand;
    s32 one = 1;
    s32 z;
    j = 0;
    z = j;
    for (; j < 5; j++)
    {
      s32 c = q->mValue;
      if (bestFace == c)
      {
        if (bestCount > 1)
        {
          goto store1c;
        }
      }
      if (nextFace == c)
      {
        if (nextCount <= 1)
        {
          goto store0c;
        }
      }
      else
      {
        goto store0c;
      }
      store1c:
      q->mComboFlag = one;

      goto flag_nextc;
      store0c:
      q->mComboFlag = z;

      flag_nextc:
      q++;

    }

  }
  for (j = 0; j < 5; j++)
  {
    s32 v = order.v[j];
    dMgCardObj_c *ptr = &hand[v];
    ptr->SetHome(j);
  }

  for (j = 0; j < 5; j++)
  {
    if (j != order.v[j])
    {
      return 1;
    }
  }

  return 0;
}

// @symbol _ZN11dScMgCard_c10SettleHandEP12dMgCardObj_c
/* dScMgCard_c::SettleHand -- the round's settle step: stamp all five cards
 * of one hand to state 9. Coined name; the ROM's symbol is address-only. */
void dScMgCard_c::SettleHand(dMgCardObj_c *hand) {
    int i;
    for (i = 0; i < 5; i++) {
        hand->mState = 9;
        hand++;
    }
}

// @symbol _ZN12dMgCardObj_c6DealInEPKi
/* dMgCardObj_c::DealIn -- deal one fresh card in: park it at the {x, y} the
 * source pair names, then retarget the approach steps off GetOffscreenY's
 * start height through the Vec2_Sub/func_0203d680 transform pair (the same
 * 1:256 fixed-point rounding both hands use), reset the raise state, and
 * draw its face. The mid-body alias reads the just-stored {x, y} back out
 * through memory into the target pair -- spelled exactly so, because a
 * plain `mTargetX = mX` would let the registers be reused and the reloads
 * would vanish. Coined name; the ROM's symbol is address-only. */
void dMgCardObj_c::DealIn(const s32 *src)
{
    int buf[4];
    mX = src[0];
    mY = src[1];
    {
        int *b = (int *)(((int)this + 4));
        mTargetX = b[0];
        mTargetY = b[1];
    }
    mY = GetOffscreenY();
    mX = 0x80000;
    Vec2_Sub(&buf[0], &mTargetX, &mX);
    func_0203d680(&buf[2], &buf[0], 0x100);
    mXStep = buf[2];
    mYStep = buf[3];
    if (mXStep < 0) mXStep = -mXStep;
    mLift = 0;
    mLiftFlag = 0;
    mValue = (unsigned char)dScMgCard_c::DrawCardValue();
}

// @symbol _ZN12dMgCardObj_c6RedealEv
/* dMgCardObj_c::Redeal -- re-fly the card from its own target: the same
 * retarget off GetOffscreenY's start height as DealIn, but from the target
 * pair already on the card, and a wider reset -- the state, the raise
 * amount and its flag, the fly-away stagger, the combo marker -- before
 * drawing a fresh face. Coined name; the ROM's symbol is address-only. */
void dMgCardObj_c::Redeal()
{
    int buf[4];
    mX = mTargetX;
    mY = mTargetY;
    mY = GetOffscreenY();
    mX = 0x80000;
    Vec2_Sub(&buf[0], &mTargetX, &mX);
    func_0203d680(&buf[2], &buf[0], 0x100);
    mXStep = buf[2];
    mYStep = buf[3];
    if (mXStep < 0) mXStep = -mXStep;
    mState = 0;
    mLift = 0;
    mLiftFlag = 0;
    mFlyDelay = 0;
    mComboFlag = 0;
    mValue = (unsigned char)dScMgCard_c::DrawCardValue();
}

// @symbol _ZN12dMgCardObj_c6UpdateEv
/* dMgCardObj_c::Update -- vtable slot 1, the player's card motion state
 * machine. mState: 1 fly in from off-screen and settle (2, lift raising, on
 * both axes arriving -- the one card sound); 3 retreat a row (the touched
 * card dodging); 4 deal a fresh card at the deck position (6) after asking
 * GetOffscreenY for the start height; 7 fly home to the mHomeX slot (8);
 * 9 settle into the dealt row; 0xa fly off home after the mFlyDelay stagger.
 * The lift runs every frame below the state block (raised toward 0x4000 =
 * one flip frame per 0x1000 of mLift when mLiftFlag is set). The tail is
 * the touch hit-test: the engine globals gate it, then the card must be
 * mid-lift (state 2 or 3, via the st + 254 (0xfe) wrap trick), fully raised (mLift
 * 0x4000), and inside the touch's on-screen window, with 0x14f picking the
 * card up (state 3) and 0x150 putting it back (state 2). */
void dMgCardObj_c::Update()
{
    int yReached;

    if (mState == 0)
        return;

    if (mState == 1) {
        if (mFlyDelay != 0) {
            ApproachLinear2(mFlyDelay, 0, 1);
            return;
        }
        yReached = ApproachLinear(mY, mTargetY, mYStep);
        if (ApproachLinear(mX, mTargetX, mXStep) != 0 && yReached != 0) {
            Sound::PlayBank2_2D(0x144);
            mState = 2;
            mLiftFlag = 1;
        }
    }

    if (mLiftFlag != 0)
        ApproachLinear(mLift, 0x4000, 0x200);
    else
        ApproachLinear(mLift, 0, 0x200);

    {
        int state = mState;
        if (state == 3) {
            ApproachLinear(mY, mTargetY - 0x10000, 0x2000);
        } else if (state == 5) {
            yReached = ApproachLinear(mY, mTargetY, mYStep);
            if (ApproachLinear(mX, mTargetX, mXStep) != 0 && yReached != 0) {
                Sound::PlayBank2_2D(0x144);
                mState = 2;
                mLiftFlag = 1;
            }
        } else if (state == 2) {
            ApproachLinear(mY, mTargetY, 0x4000);
        } else if (state == 4 && mLift == 0) {
            int result = GetOffscreenY();
            if (ApproachLinear(mY, result, 0x8000) != 0) {
                mX = 0x80000;
                mState = 6;
                mValue = (unsigned char)dScMgCard_c::DrawCardValue();
            }
        } else if (state == 7) {
            if (ApproachLinear(mX, mHomeX, 0x6000) != 0)
                mState = 8;
        } else if (state == 9) {
            ApproachLinear(mY, 0x80000, 0x6000);
        } else if (state == 0xa) {
            if (ApproachLinear2(mFlyDelay, 0, 1) != 0)
                ApproachLinear(mX, mHomeX, mXStep);
        }
    }

    if (data_ov006_0214176c != 0) {
        if (mState == 3) {
            mState = 4;
            mLiftFlag = 0;
        }
        return;
    }
    if (data_ov006_02141768 != 0)
        return;

    {
        int touchIndex = data_020a0e40;
        unsigned char touchHeld;
        int touchActive = 0;
        touchHeld = data_020a0de8[touchIndex * 4];
        if (touchHeld != 0 && data_020a0de9[touchIndex * 4] != 0)
            touchActive = 1;
        if (touchActive == 0)
            return;
        {
            unsigned char state = mState;
            if ((unsigned char)(state + 254) > 1)
                return;
            if (mLift != 0x4000)
                return;
            {
                int touchDeltaX = (int)data_020a0dea[touchIndex][0] - (mX >> 12);
                int touchDeltaY = (int)data_020a0deb[touchIndex][0] - (mY >> 12);
                if (touchDeltaX <= 7)
                    return;
                if (touchDeltaX >= 0x29)
                    return;
                if (touchDeltaY <= -1)
                    return;
                if (touchDeltaY >= 0x31)
                    return;
                if (state == 2) {
                    func_02012790(0x14f);
                    mState = 3;
                } else {
                    func_02012790(0x150);
                    mState = 2;
                }
            }
        }
    }
}

// @symbol _ZN12dMgCardObj_c6RenderEv
/* dMgCardObj_c::Render -- vtable slot 0, the player's card. Same table and
 * same index math as the dealer's Render above; the temporaries are spelled
 * out (idx, then e) where the dealer's stay one, because the two bodies'
 * register allocations differ and each spelling is the one its own bytes
 * need. */
void dMgCardObj_c::Render()
{
    int idx;
    unsigned short e;
    if (mState == 0) return;
    idx = (mValue + 1) * 5 + (mLift >> 12);
    e = data_ov006_0213bd64[idx];
    Hud_RenderSprite(
        data_ov006_02141774[e],
        (mX >> 12) + 24,
        (mY >> 12) + 24,
        -1, -1);
}

// @symbol _ZN12dMgCardObj_c13GetOffscreenYEv
/* dMgCardObj_c::GetOffscreenY -- vtable slot 2. The y the card starts flying
 * in from: 6 screens above the table in 1:4096 fixed point. DealIn reads it
 * through the virtual slot, so the dealer's override carries the same value. */
s32 dMgCardObj_c::GetOffscreenY()
{
    return -0x60000;
}

// @symbol _ZN11dScMgCard_c9RenderHudEv
/* dScMgCard_c::RenderHud -- the hand-rank table and the hold prompt. States 4
 * through 0x10 stack six face sprites at x=0x14 from y=0xb0 down; the two
 * hands' highlight pairs (unk_538e/unk_5390 for the player's hand,
 * unk_5392/unk_5394 for the dealer's) name the rows drawn in a highlight
 * variant -- 1 for the player's values, 2 for the dealer's, and when both
 * hands name the same row the two variants alternate on mFrameCounter bit
 * 4. State 4, once the whole player hand is lifted, adds the prompt
 * sprite at (0x80, 0x58), its variant picking whether every card has
 * reached state 2. Coined name; the ROM's symbol is address-only. */
void dScMgCard_c::RenderHud()
{
    short state = mState;
    if (state > 3 && state < 0x11) {
        int y = 0xb0;
        int i;
        for (i = 0; i < 6; i++) {
            int highlight = 0;
            if ((i == unk_5392 && i == unk_538e) || (i == unk_5394 && i == unk_5390)) {
                if (mFrameCounter & 0x10) highlight = 1; else highlight = 2;
            } else if (i == unk_5392 || i == unk_5394) {
                if (mFrameCounter & 0x10) highlight = 2;
            } else if (i == unk_538e || i == unk_5390) {
                if (mFrameCounter & 0x10) highlight = 1;
            }
            Hud_RenderSprite(data_ov006_0213bd30[i][highlight], 0x14, y, -1, -1);
            y -= 0x10;
        }
    }
    if (mState != 4) return;
    if (dScMgCard_c::AllLifted((const dMgCardObj_c *)mArray1) == 0) return;
    if (dScMgCard_c::AllInState((const dMgCardObj_c *)mArray1, 2) == 0)
        Hud_RenderSprite(data_ov006_0213406c[0], 0x80, 0x58, -1, -1);
    else
        Hud_RenderSprite(data_ov006_0213406c[1], 0x80, 0x58, -1, -1);
}

// @symbol _ZN12dMgCardObj_c7SetHomeEi
/* dMgCardObj_c::SetHome -- aim the card at home slot n (the row pitch is
 * 0x28, the first slot sits at 0x2c) and put it into state 7, the fly-home
 * step. Coined name; the ROM's symbol is address-only. */
void dMgCardObj_c::SetHome(s32 n){
  mHomeX = (n*0x28+0x2c)<<0xc;
  mHomeY = 0x80000;
  mState = 7;
}

// @symbol _ZN12dMgCardObj_c6FlyOffEv
/* dMgCardObj_c::FlyOff -- the end-of-round fly-away: home becomes the
 * off-screen left, the y step tracks the card's own height, and the
 * x step's frame count becomes the stagger -- one card leaves per frame,
 * counted down from how high the card sits. State 0xa is the fly-away
 * step. Coined name; the ROM's symbol is address-only. */
void dMgCardObj_c::FlyOff(){
  mHomeX = -0x28000;
  mHomeY = mY;
  int t = mX;
  unsigned ip = 0x80000 - t;
  mFlyDelay = (s16)((int)ip / 0x10000);
  mXStep = 0x10000;
  mYStep = 0;
  mState = 0xa;
}

// @symbol _ZN17dMgDilarCardObj_c13GetOffscreenYEv
/* dMgDilarCardObj_c::GetOffscreenY -- vtable slot 2, the same answer the base
 * gives: both hands' cards fly in from 6 screens above the table (-0x60000
 * in 1:4096 fixed point), and the override exists so the dealer's class
 * carries the full set of its own slots. */
s32 dMgDilarCardObj_c::GetOffscreenY()
{
    return -0x60000;
}

// @symbol _ZN17dMgDilarCardObj_c6UpdateEv
/* dMgDilarCardObj_c::Update -- vtable slot 1, the dealer's card motion.
 * Same states as the player's card (see dMgCardObj_c::Update below) minus
 * the touch handling -- the dealer's card is never touched -- with state 9
 * approaching the row the card was dealt into (0x10000, the dealer's row,
 * where the base flies its player's cards to 0x80000) and the early-return
 * chain kept in exactly this order for the register allocation. */
void dMgDilarCardObj_c::Update()
{
    if (mState == 0)
        return;

    if (mLiftFlag != 0)
        ApproachLinear(mLift, 0x4000, 0x200);
    else
        ApproachLinear(mLift, 0, 0x200);

    u8 state = mState;

    if (state == 5) {
        int r5 = ApproachLinear(mY, mTargetY, mYStep);
        int r0 = ApproachLinear(mX, mTargetX, mXStep);
        if (r5 == 0)
            return;
        if (r0 != 0)
            mState = 1;
        return;
    }

    if (state == 4 && mLift == 0) {
        int v = GetOffscreenY();
        int r0 = ApproachLinear(mY, v, 0x8000);
        if (r0 == 0)
            return;
        mX = 0x80000;
        mState = 6;
        mValue = dScMgCard_c::DrawCardValue();
        return;
    }

    if (state == 7) {
        int r0 = ApproachLinear(mX, mHomeX, 0x6000);
        if (r0 != 0)
            mState = 8;
        return;
    }

    if (state == 9) {
        ApproachLinear(mY, 0x10000, 0x6000);
        return;
    }

    if (state == 1) {
        if (mFlyDelay != 0) {
            ApproachLinear2(mFlyDelay, 0, 1);
            return;
        }
        ApproachLinear(mY, mTargetY, mYStep);
        ApproachLinear(mX, mTargetX, mXStep);
        return;
    }

    if (state != 0xa)
        return;

    if (ApproachLinear2(mFlyDelay, 0, 1) == 0)
        return;

    ApproachLinear(mX, mHomeX, mXStep);
}

// @symbol _ZN17dMgDilarCardObj_c6RenderEv
/* dMgDilarCardObj_c::Render -- vtable slot 0, the dealer's card face down.
 * The sprite bank table data_ov006_0213bd64 is indexed by the card's face
 * (mValue + 1, the +1 skipping the card-back row) and its flip frame
 * (mLift >> 12), and Hud_RenderSprite takes the 1:4096 fixed-point mX/mY
 * back down to pixels, plus the 24-pixel hand margin. Kept compact -- one index
 * temporary -- because that is the register shape the cartridge carries;
 * the base class's Render below spells its temporaries out instead, and
 * the two spellings are not interchangeable. */
void dMgDilarCardObj_c::Render()
{
  int k;
  if(mState==0) return;
  k=data_ov006_0213bd64[(mValue+1)*5+(mLift>>12)];
  Hud_RenderSprite(
    data_ov006_02141774[k],
    (mX>>12)+24,
    (mY>>12)+24,
    -1,-1);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN17dMgDilarCardObj_cD1Ev
/* Genuine C++ produces these two stores only when the base destructor is
 * inline, but then emits the base D1 between the scene D1/D0. This callback
 * is the measured section-order ABI bridge. */
extern "C" void _ZN17dMgDilarCardObj_cD1Ev(void *elem)
{
  *(int *)elem = (int)&_ZTV17dMgDilarCardObj_c[2];
  *(int *)elem = (int)&_ZTV12dMgCardObj_c[2];
}

// @symbol _ZN12dMgCardObj_cD1Ev
/* The real inline body is byte-identical but cannot occupy the cartridge's
 * post-scene-D0 section position. */
extern "C" void _ZN12dMgCardObj_cD1Ev(void *elem)
{
  *(int *)elem = (int)&_ZTV12dMgCardObj_c[2];
}

/* The destructor is DEFINED INLINE in include/dScMgCard_c.h. mwcc emits the
 * D1/D0 pair at ROM ordinals 1 and 2 (0x020d95a4 size 0x94, 0x020d9638 size
 * 0xa8) from the vtable slots that name them, in cartridge order -- written
 * out of line here it emitted D0 ahead of D1 and rombuild's fail-closed
 * isolate refused the TU. The header's own banner carries the full reason. */
