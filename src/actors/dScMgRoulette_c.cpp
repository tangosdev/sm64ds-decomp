//cpp
/* dScMgRoulette_c -- Mushroom Roulette. Racers are dealt onto a spinning
 * board, the countdown runs, and each racer is paid by the tile it stops on.
 *
 * Functions are in reverse ROM order; do not reorder.
 *
 * common.h comes first so its flat Matrix4x3 wins the MATRIX4X3_DEFINED
 * guard; func_ov006_02107db8 copies a whole matrix and needs that spelling.
 *
 * Leftover: the camera at 0x4660 stays an offset; it lives on the base.
 * Behavior keeps mPhase, mPhaseTimer, mDealIndex, mTableReady (0x53d8),
 * mTilesScored (0x53dc) and the payout clear at 0x53f4 behind H/HA/I,
 * and its meter and slider calls at raw offsets: member form DIFFed
 * there. The slider's PMF stays a local view.
 * Most func_ and data_ helpers are unnamed.
 */

#include "common.h"
#include "types.h"
#include "dScMgRoulette_c.h"
#include "Model.h"
#include "decl_common.h"

/* Local views of the records the helpers walk. */

/* Three consecutive Fix12 words, written a component at a time. */
struct Vec3 { int x, y, z; };

/* One row of the 12-entry angle table at data_ov006_0213e334, copied whole. */
typedef struct { short v[12]; } ShortTbl12;

/* Two Fix12 words copied as a unit out of data_ov006_0213e2f8 and
   data_ov006_0213e2e8. Both keep the array member: a whole-struct copy of
   `{int a, b;}` is split into load, store, load, store, where the ROM does
   both loads first. */
struct Pair   { int w[2]; };
struct Pair2W { int w[2]; };

/* The board's tile centres: data_ov006_02142ab4 holds x and
   data_ov006_02142ab8 the same array offset to y, stride 8. decl_common.h
   types both as `char[]`, so every read casts. */
struct TileXY { int x, y; };

/* The two BMD files behind the slider model pair, at +0x530c. */
struct ResPair { char pad[8]; void *a; void *b; };

/* A ModelAnim-shaped subobject, reached only through its slot 5. */
struct Sub {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int a);
};

/* The slider as func_ov006_02107db8 sees it: a pointer-to-member state
   callback at +0x0 and a subobject with four virtuals at +0x10. */
struct ObjV {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
};
struct C;
typedef void (C::*PMF)();
struct C {
    PMF pmf;            /* 0x0 */
    char pad8[0x10 - 8];
    ObjV obj;           /* 0x10 */
};

/* One racer, 0x34 bytes, five of them at 0x51a8. Thing, RacerXY and
   RacerPos are three views of the same record; each helper keeps the view
   it was matched with. x/y slide toward targetX/targetY by stepX/stepY.
   state: 0 handled with 3, 1 sliding, 2 under the stylus,
   3 stopped, 4 about to hop, 5 hopping, 6 settled, 7 sent to the side
   pile. */
typedef struct Thing {
    s32 x;
    s32 y;
    s32 grabX;
    s32 grabY;
    s32 stepX;
    s32 stepY;
    s32 targetX;
    s32 targetY;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s16 tile;
    s16 unk2E;
    s16 hop;
    u8 state;
    u8 unk33;
} Thing;

struct RacerXY {
    int pad[4];
    int stepX;
    int stepY;
    int targetX;
    int targetY;
};

struct RacerPos {
    char pad[0x18];
    int f18;
    int f1c;
    char pad2[0x2c - 0x20];
    short f2c;
};

typedef struct { int x, y; } Vec2_Fix12;

/* The racer array as Render walks it. */
typedef struct Racer {
    char b[0x34];
} Racer;
typedef struct Obj9 {
    char _p0[0x51a8];
    Racer racers[5]; /* 0x51a8 -- 5 x 0x34, see the header */
} Obj9;

/* Address launders and raw field macros. They stop the compiler reusing a
   field address across a call, which the ROM does not do. Behavior's reads of
   mPhase, mPhaseTimer and mDealIndex go through H, HA and I on purpose:
   spelled as members, the code changes. */
#define AT(p,off) ((void*)(int)(((long long)(int)((char*)(p)+(off)))))
#define LNDR(e) ((int)(((long long)(e))))
#define I(o) (*(int*)(c + (o)))
#define H(o) (*(short*)(c + (o)))
#define HA(o) (*(short*)AT(c,(o)))
/* The active minigame scene's bonus count, or 0 with no scene.
   data_ov004_020beb68 is declared `void *` by dScMgBase_c.h. */
#define SCENE_BONUS ((data_ov004_020beb68 != 0) \
                        ? *(int *)((char *)data_ov004_020beb68 + 0xa8) : 0)

namespace Sound {
    void PlayBank2_2D(unsigned int id);
}

namespace cstd {
    int fdiv(int a, int b);
}

namespace G2S {
    int GetBG2CharPtr();
    int GetBG2ScrPtr();
    int GetBG0ScrPtr();
}

namespace GX {
    void LoadOBJPltt(const void *data, unsigned int offset, unsigned int size);
}

namespace GXS {
    void LoadBGPltt(const void *data, unsigned int offset, unsigned int size);
    void LoadOBJPltt(const void *data, unsigned int offset, unsigned int size);
}

int ApproachLinear(int &value, int target, int step);

extern "C" {


int  GetGameLanguage(void);
void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
void RenderOamBothScreens(void *a0, int a1, int a2, int a3, int a4, void *a5);
void Vec2_Sub(int *out, int *a, int *b);
void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *mF, int x, int y, int z);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
int  Sound_PlayIfNotActive(int a, int b, int c, int d);
int  RandomIntInternal(int *seed);
void *LoadFile(int id);
void DecompressLZ16(void *src, int dst);
void Ov004_Deallocate(void *p);
void FreeGfxSlotsById(int a);
/* local extern: declared only in the sibling scene headers (dScMgAmida_c.h,
   dScMgSmartball_c.h, ...), none of which this TU includes. */
void NullDestructor_0203d47c(void);
void Camera_UpdateMatrices(char *cam);

void func_02012790(int a);
void func_020731dc(int a, int b, void **node);
void func_0203d680(Vec2_Fix12 *out, const Vec2_Fix12 *in, int scale);
void *func_ov004_020adc68(int id);
void func_ov004_020b1b08(int c);
void func_ov004_020b1b40(int c);
void func_ov004_020b1bc8(char *, int, int, int);
void func_ov004_020b2220(int, int, int, int, int, int, int);
void func_ov004_020b56c8(void);
void func_ov004_020b65e4(void);
void func_ov004_020b66d4(void);
void func_ov006_020c0aa8(char *c);
int  func_ov006_020c0efc(char *a);
int  func_ov006_020c1718(char *c);
void func_ov006_020c1804(char *c);
void func_ov006_020c19d0(char *thiz);
int  func_ov006_020c1a88(char *c);

extern int   data_ov006_021428d0;
extern int   data_ov006_0213e2dc;
extern char *data_ov006_02142aa4[];
extern int   data_020a0db0;
extern s32   data_ov006_0212ed24[];
extern void *data_ov006_021376ec[];
extern void *data_ov006_0213e320[];
extern int   data_ov006_0212ed14[];
extern void *data_ov006_02138c3c;
extern Matrix4x3 data_020a0e68;
extern short data_02082214[];
extern short data_ov006_0213e334[];
extern short data_ov006_0212ed34[];
extern struct Pair   data_ov006_0213e2f8;
extern struct Pair2W data_ov006_0213e2e8;
extern int   data_0209e650[];
extern int   data_ov006_0212ed4c[];
extern int   data_ov006_0212ed68[];
extern short data_ov006_0212edb8[];
extern short data_ov006_0212edba[];
extern short data_ov006_0212ed88[];
extern short data_ov006_0212ed8a[];
extern short data_ov006_0212ed8c[];
extern short data_ov006_0212ed8e[];
extern int   data_ov006_021428cc;
extern int   data_ov006_021428d4[];
extern void *data_ov006_02142948;
extern u8    data_020a0e40;
extern u8    data_020a0de8[];
extern u8    data_020a0de9[];
extern u8    data_020a0dea[];
extern u8    data_020a0deb[];
extern s32   data_ov004_020b9488;
extern u16   data_ov004_020bf9e4;
extern u8    data_0209d45c;
extern u8    data_0209d454;

/* This file's helpers. The file runs from the highest address down, so
   most calls between them are forward references. */
void func_ov006_021079cc(void);
void func_ov006_02107d20(int *p, int val);
void func_ov006_02107d58(int *p);
void func_ov006_02107db8(C *c);
void func_ov006_02107ea8(char *self);
void func_ov006_021082c4(char *p);
void func_ov006_021082fc(char *self);
void func_ov006_02108508(char *p);
int  func_ov006_0210858c(ResPair *o);
short func_ov006_02108650(int x, int y);
int  func_ov006_02108b90(short *r0, int r1);
void func_ov006_02108d28(int *p);
int  func_ov006_02108e24(int *p);
void func_ov006_02108f2c(Thing *t);
void func_ov006_0210927c(char *a);
void func_ov006_021092e8(void *self);
void func_ov006_0210935c(RacerPos *self, int idx);
void func_ov006_021094ac(RacerXY *self, int *src);
void func_ov006_02109530(int *out, int *a, int scale);
void func_ov006_021095ac(int *a, int *b);
void func_ov006_02107b94(void **sl);
void func_ov006_02107c50(short *a0, int a1);

}

// @symbol _ZN15dScMgRoulette_c13InitResourcesEv
/* Sets up both screens: the sub BG layers, the board tiles, map and
   palette, and the shared OBJ tiles and palettes for both engines. Then
   sets up the camera, the table and the slider and starts the first round. */
s32 dScMgRoulette_c::InitResources()
{
    char *raw = (char *)this;
    void *bgChars, *bgMap, *f6, *f5;

    func_ov004_020b04d0(0x20);

    *(volatile u16 *)0x0400100c = (*(volatile u16 *)0x0400100c & 0x43) | 0x404;
    *(volatile u16 *)0x04001008 = (*(volatile u16 *)0x04001008 & 0x43) | 0x504;

    bgChars = LoadFile(0x6f);
    bgMap = LoadFile(0x71);
    f6 = LoadFile(0x1e4);
    f5 = LoadFile(0x70);
    DecompressLZ16(bgChars, G2S::GetBG2CharPtr());
    DecompressLZ16(bgMap, G2S::GetBG2ScrPtr());
    DecompressLZ16(f6, G2S::GetBG0ScrPtr());
    GXS::LoadBGPltt(f5, 0x1a0, 0x60);
    Deallocate(bgChars);
    Deallocate(bgMap);
    Deallocate(f6);
    Deallocate(f5);

    {
        volatile u16 *a = (volatile u16 *)0x04001008;
        *a = *a & ~0x40;
        *(volatile u16 *)0x0400100c = *(volatile u16 *)0x0400100c & ~0x40;
        SetSubBg0Offset(0, 0);
        SetSubBg2Offset(0, 0);
        *a = (*a & ~3) | 2;
        *(volatile u16 *)0x0400100c = (*(volatile u16 *)0x0400100c & ~3) | 2;
    }

    f6 = LoadFile(0xe9);
    f5 = LoadFile(0xea);
    DecompressLZ16(f6, 0x6400000);
    DecompressLZ16(f6, 0x6600000);
    GX::LoadOBJPltt(f5, 0, 0x100);
    GXS::LoadOBJPltt(f5, 0, 0x100);
    Deallocate(f6);
    Deallocate(f5);

    data_0209d45c = 0x11;
    data_0209d454 = 0x15;
    *(volatile u16 *)0x04000008 = (*(volatile u16 *)0x04000008 & ~3) | 1;

    func_ov006_020c0aa8(raw + 0x4660);
    if (func_ov006_020c1a88((char *)mTable) == 0)
        return 0;
    if (func_ov006_021085c0((char *)mSlider) == 0)
        return 0;

    unk_0a8 = func_ov004_020ad8b8();
    unk_0ac = unk_0a8;
    func_ov004_020b682c();
    func_ov006_02107b70((char *)mMeter);
    OnYoshiTryEat(-1);
    return 1;
}

// @symbol _ZN15dScMgRoulette_c8BehaviorEv
/* One frame of the round, by mPhase: 1 deals the racers out one at a time,
   2 runs the countdown and scores the board, 3 pays out per landed tile and
   4 announces the result. The racer update loops run every frame. */
s32 dScMgRoulette_c::Behavior()
{
    char *c = (char *)this;

    switch (mPhase) {
    case 0:
        break;
    case 1:
        if (func_ov006_020c0efc((char *)mTable) != 0) {
            if (H(0x53d8) == 0) {
                H(0x53d8) = 1;
            }
        }
        if (mPromptBlinkCount == 0) {
            mPromptEnabled = 1;
            mPromptBlinkCount = 1;
            mPromptBlinkTimer = 0;
        }
        if (func_ov006_020c0efc((char *)mTable) != 0) {
            mPhaseTimer -= 1;
            if (H(0x53e8) == 0) {
                mCameraPreset = 1;
                if (I(0x53f8) < mRacerCount) {
                    int dealPos[2];
                    int dealY;
                    dealY = LNDR(0 - (I(0x53f8) << 16)) + 0x80000;
                    dealPos[0] = 0x20000;
                    dealPos[1] = dealY;
                    func_ov006_021095ac((int *)(mArray + I(0x53f8) * 0x34), dealPos);
                    mDealIndex += 1;
                    H(0x53e8) = 8;
                    func_ov004_020b1b08(1);
                    Sound::PlayBank2_2D(0x14d);
                } else {
                    int i = 0;
                    if (mRacerCount > 0) {
                        char *p = c;
                        do {
                            *(unsigned char *)(p + 0x51da) = 1;
                            i++;
                            p += 0x34;
                        } while (i < mRacerCount);
                    }
                    H(0x53e8) = 0x258;
                    func_ov006_020c1764((char *)mTable);
                    HA(0x53e6) += 1;
                    I(0x53f8) = 0;
                    Sound::PlayBank2_2D(0x15b);
                }
            }
        }
        break;
    case 2: {
        int timer = H(0x53e8);
        if (timer % 60 == 0 && timer > 0 && timer <= 0xb4) {
            Sound::PlayBank2_2D(0xa6);
        }
        mPhaseTimer -= 1;
        if (H(0x53e8) == 0) {
            {
                short tileType;
                int onTile;
                int i = 0;
                if (mRacerCount > 0) {
                    char *racerRaw = c;
                    char *racer = (char *)mArray;
                    do {
                        *(short *)(racerRaw + 0x51d4) = func_ov006_02108650(*(int *)(racerRaw + 0x51c0) >> 12, *(int *)(racerRaw + 0x51c4) >> 12);
                        onTile = 0;
                        tileType = *(short *)(racerRaw + 0x51d4);
                        if (tileType >= 0xc && tileType <= 0x1c)
                            onTile = 1;
                        if (onTile != 0)
                            goto landedInRange;
                        if (tileType >= 0x1d && tileType <= 0x22)
                            onTile = 1;
                        else
                            onTile = 0;
                        if (onTile == 0)
                            goto landedElsewhere;
                    landedInRange:
                        func_ov006_02109530((int *)racer, (int *)&((TileXY *)data_ov006_02142ab4)[tileType].x, 0x100);
                        goto nextRacer;
                    landedElsewhere:
                        func_ov006_0210935c((RacerPos *)racer, tileType);
                    nextRacer:
                        i++;
                        racerRaw += 0x34;
                        racer += 0x34;
                    } while (i < mRacerCount);
                }
            }
            I(0x53f8) = 0;
            mCameraPreset += 1;
            HA(0x53e6) += 1;
            I(0x53dc) = 1;
            H(0x53e8) = 1;
            Sound::PlayBank2_2D(0x15a);
            {
                int sounded = 0;
                int j = 0;
                if (mRacerCount > 0) {
                    char *rowRaw = c;
                    char *row = (char *)mArray;
                    do {
                        if (*(short *)(rowRaw + 0x51d4) == 0x25) {
                            func_ov006_021092e8(row);
                            if (sounded == 0) {
                                Sound::PlayBank2_2D(0x15c);
                                sounded = 1;
                            }
                        } else {
                            *(unsigned char *)(rowRaw + 0x51da) = 3;
                            mTargetScore += 1;
                        }
                        j++;
                        rowRaw += 0x34;
                        row += 0x34;
                    } while (j < mRacerCount);
                }
                func_ov004_020ad79c(mRacerCount - mTargetScore + SCENE_BONUS, 0);
            }
        }
        break;
    }
    case 3: {
        int onTile;
        short wait;
        int tile;
        int j;
        int payout;
        char *p;
        onTile = (mBoardBusy == 0) ? 1 : 0;
        if (LNDR(onTile) != 0) {
            j = 0;
            mPromptEnabled = 0;
            wait = H(0x53e8);
            mPhaseTimer -= 1;
            if (wait == 0) {
                tile = mSelectedTile;
                mScore = 0;
                if (mRacerCount > 0) {
                    p = (char *)mArray;
                    do {
                        payout = func_ov006_02108b90((short *)p, tile);
                        /* mMeter[3..0] (payouts 2, 3, 6, 12) DIFFed in this
                           function; the offsets are those strips. */
                        if (payout == 2) {
                            func_ov006_02107d20((int *)(c + 0x52f4), (int)p);
                        } else if (payout == 3) {
                            func_ov006_02107d20((int *)(c + 0x52dc), (int)p);
                        } else if (payout == 6) {
                            func_ov006_02107d20((int *)(c + 0x52c4), (int)p);
                        } else if (payout == 0xc) {
                            func_ov006_02107d20((int *)(c + 0x52ac), (int)p);
                        }
                        mScore += payout;
                        j++;
                        p += 0x34;
                    } while (j < mRacerCount);
                }
                func_ov004_020ad79c(mScore + SCENE_BONUS, 0);
                H(0x53f4) = 0;
                I(0x53f8) = 0;
                H(0x53e8) = 0x5a;
                HA(0x53e6) += 1;
            }
        }
        break;
    }
    case 4: {
        mPhaseTimer -= 1;
        if (H(0x53e8) == 0) {
            short target = mTargetScore;
            short score = mScore;
            if (score > target) {
                func_ov006_020c0c80(mTable);
                func_ov004_020b0a54(4);
            } else if (score < target) {
                func_ov006_020c0d68(mTable);
                func_ov004_020b0a54(5);
            } else {
                func_ov004_020b0a54(8);
            }
            mCameraPreset = 0;
            HA(0x53e6) += 1;
        }
        break;
    }
    }

    {
        int n = 0;
        if (mRacerCount > 0) {
            char *p = (char *)mArray;
            do {
                func_ov006_02108f2c((Thing *)p);
                n++;
                p += 0x34;
            } while (n < mRacerCount);
        }
    }
    {
        int n = 0;
        if (mRacerCount > 0) {
            char *p = (char *)mArray;
            do {
                func_ov006_02108d28((int *)p);
                n++;
                p += 0x34;
            } while (n < mRacerCount);
        }
    }
    func_ov006_02107db8((C *)(c + 0x530c));
    func_ov006_020c19d0((char *)mTable);
    func_ov004_020b65e4();
    return 1;
}

// @symbol func_ov006_02109aa0
/* A tail-call veneer to func_ov006_021079cc. */
extern "C" {
void func_ov006_02109aa0(char *) {
    func_ov006_021079cc();
}
}

// @symbol _ZN15dScMgRoulette_c6RenderEv
/* Draws the racers back to front, the cursor on the selected tile, the
   countdown (its volume ramps over the last 3 seconds), and moves the camera
   to the current preset. */
s32 dScMgRoulette_c::Render()
{
    char *raw = (char *)this;
    func_ov004_020b1bc8(raw, 0xc, 0xc, 0);
    func_ov004_020b6430();

    {
        int i = mRacerCount - 1;
        for (; i >= 0; i--)
            func_ov006_02108cc0(((Obj9 *)raw)->racers[i].b);
    }

    func_ov006_02109aa0(raw);

    {
        int idle = (int)(((long long)(mBoardBusy == 0)));
        if (idle != 0) {
            if (mPhase < 8) {
                if (data_020a0db0 & 8) {
                    s16 idx = mSelectedTile;
                    Hud_RenderSprite((void *)data_ov006_02138c18,
                                        ((TileXY *)data_ov006_02142ab4)[idx].x >> 12,
                                        ((TileXY *)data_ov006_02142ab8)[idx].x >> 12, /* = [idx].y */
                                        -1, 1);
                }
            }
        }
    }

    if (mPhase == 2) {
        int timer = mPhaseTimer;
        if (timer <= 0xb4) {
            int frames = timer + 0x3b;
            s16 rem = frames % 60;
            int seconds = frames / 60;
            int vol = rem * 0x88 + 0x1000;
            if (vol > 0x2000)
                vol = 0x2000;
            func_ov004_020b2220(0x80, 0x60, seconds, 1, -1, func_02053200(vol), 0);
        }
    }

    {
        s16 preset = mCameraPreset;
        if (preset != 0) {
            int off = preset * 0xc;
            s16 angle = *(s16 *)(data_ov006_0213e2e0 + preset * 2);
            {
                int d1 = *(int *)(data_ov006_0213e354 + off);
                int b1 = *(int *)(data_ov006_0213e350 + off);
                int a1 = *(int *)(data_ov006_0213e34c + off);
                mCameraTargetX = a1;
                mCameraTargetY = b1;
                mCameraTargetZ = d1;
            }
            {
                int f2 = *(int *)(data_ov006_0213e378 + off);
                int e2 = *(int *)(data_ov006_0213e374 + off);
                int d2 = *(int *)(data_ov006_0213e370 + off);
                mCameraEyeX = d2;
                mCameraEyeY = e2;
                mCameraEyeZ = f2;
            }
            mCameraAngle = angle;
            Camera_UpdateMatrices(raw + 0x4660);
        } else {
            func_ov006_020c0aa8(raw + 0x4660);
        }
        if (preset != 0)
            func_ov006_02107d80((char *)mSlider);
        else
            func_ov006_020c1804((char *)mTable);
    }

    return 1;
}

// @symbol _ZN15dScMgRoulette_c16CleanupResourcesEv
/* Frees the slider's two model files. */
s32 dScMgRoulette_c::CleanupResources()
{
    func_ov006_0210858c((ResPair *)mSlider);
    return 1;
}

// @symbol _ZN15dScMgRoulette_c13OnTurnIntoEggEi
/* Runs the end of the round, phases 5 to 9. Returns 1 when it is done. */
int dScMgRoulette_c::OnTurnIntoEgg(int /* mode */)
{
    char *raw = (char *)this;

    switch (mPhase) {
    case 5:
        FreeGfxSlotsById(0x1d);
        if (func_ov006_02107a6c() != 0)
            (mPhase)++;
        break;
    case 6:
        if (func_ov006_020c1718((char *)mTable) != 0) {
            mPhaseTimer = 0x3c;
            mDealIndex = 0;
            (mPhase)++;
        }
        break;
    case 7:
        (mPhaseTimer)--;
        if (mPhaseTimer == 0) {
            if (mScore != 0)
                func_ov004_020b56c8();
            (mPhase)++;
        }
        break;
    case 8:
        (mPhase)++;
        /* fall through */
    case 9:
    default:
        if (data_ov004_020bf9e4 <= 1)
            return 1;
        break;
    }
    return 0;
}

// @symbol _ZN15dScMgRoulette_c13OnYoshiTryEatEi
/* Resets the round: the table, the racers, the score and the slider, with
   one racer per bonus the scene holds, up to five. InitResources calls it
   with -1. */
void dScMgRoulette_c::OnYoshiTryEat(int /* arg */)
{
    char *raw = (char *)this;

    int i;
    int *racer;
    int count;

    func_ov006_02107b14();
    func_ov006_020c0e8c((int *)mTable);

    mScore = 0;
    mPhaseTimer = 0xa;

    racer = (int *)mArray;
    for (i = 0; i < 5; i++) {
        func_ov006_021092a0(racer);
        racer = (int *)((char *)racer + 0x34);
    }

    mDealIndex = 0;
    mTargetScore = 0;
    count = 0;
    if (data_ov004_020beb68 != 0) count = ((int *)data_ov004_020beb68)[0xa8 / 4];
    if (count >= 5) count = 5;
    mRacerCount = count;

    func_ov006_02108524((char *)mSlider);

    data_ov006_021428c8 = 0;
    mCameraPreset = 0;
    unk_53e0 = 0x100;
    unk_53ea = 0;
    unk_53ec = 0;
    unk_53ed = 0;
    unk_53ee = 0;
    unk_53ef = 0;
    unk_53f0 = 0;
    mPhase = 1;

    func_ov004_020b66d4();
    data_ov004_020bc7d4 = 1;
}

// @symbol func_ov006_021095ac
extern "C" {
void func_ov006_021095ac(int* racer, int* target)
{
    racer[8] = target[0];
    racer[9] = target[1];
    func_ov006_02109530(racer, target, 0x80);
}
}

// @symbol func_ov006_02109530
extern "C" {
void func_ov006_02109530(int* out, int* target, int scale){
  Vec2_Fix12 v1;
  Vec2_Fix12 v2;
  out[6]=target[0];
  out[7]=target[1];
  Vec2_Sub((int*)&v1, out+6, out);
  func_0203d680(&v2, &v1, scale);
  out[4]=v2.x;
  out[5]=v2.y;
  if(out[4]<0) out[4]=-out[4];
  if(out[5]<0) out[5]=-out[5];
}
}

// @symbol func_ov006_021094ac
extern "C" {
void func_ov006_021094ac(RacerXY *racer, int *target) {
    int v[2];
    racer->targetX = target[0];
    racer->targetY = target[1];
    Vec2_Sub(v, &racer->targetX, (int *)racer);
    racer->stepX = v[0];
    racer->stepY = v[1];
    if (func_0203d434(&racer->stepX)) {
        func_0203d630(&racer->stepX, 0x18000);
    }
    if (racer->stepX < 0) racer->stepX = -racer->stepX;
    if (racer->stepY < 0) racer->stepY = -racer->stepY;
}
}

// @symbol func_ov006_0210935c
extern "C" {
void func_ov006_0210935c(RacerPos *racer, int tile)
{
    int buf[7];
    int f18 = racer->f18;
    int a, b, lr, t, hi;
    buf[0] = f18;
    buf[1] = racer->f1c;
    if ((unsigned short)(short)(tile - 0x23) <= 1) {
        a = ((TileXY *)data_ov006_02142ab4)[tile].x;
        b = ((TileXY *)data_ov006_02142ab8)[tile].x;
        buf[2] = a;
        buf[3] = b;
        lr = a + 0x8000;
        hi = a + 0x18000;
        if (f18 >= lr) {
            if (f18 <= hi)
                hi = f18;
            lr = hi;
        }
        b = buf[3];
        t = buf[1];
        {
            int lo = b - 0x20000;
            buf[0] = lr;
            lr = lo;
            hi = b + 0x20000;
            if (t >= lo) {
                if (t <= hi)
                    hi = t;
                lr = hi;
            }
        }
        buf[1] = lr;
        func_ov006_02109530((int *)racer, buf, 0x100);
        return;
    }
    {
        int early = (racer->f2c <= 0xb) ? 1 : 0;
        if (LNDR(early) == 0)
            return;
    }
    a = ((TileXY *)data_ov006_02142ab4)[tile].x;
    b = ((TileXY *)data_ov006_02142ab8)[tile].x;
    buf[4] = a;
    buf[5] = b;
    lr = a - 0x10000;
    hi = a + 0x10000;
    if (buf[0] >= lr) {
        if (buf[0] <= hi)
            hi = buf[0];
        lr = hi;
    }
    b = buf[5];
    t = buf[1];
    buf[0] = lr;
    lr = b - 0xc000;
    hi = b + 0xc000;
    if (t >= lr) {
        if (t <= hi)
            hi = t;
        lr = hi;
    }
    buf[1] = lr;
    func_ov006_02109530((int *)racer, buf, 0x100);
}
}

// @symbol func_ov006_021092e8
extern "C" {
void func_ov006_021092e8(void* racer)
{
    int b = data_ov006_021428cc & 1;
    if (b == 0) {
        data_ov006_021428d4[0] = 0xc000;
        data_ov006_021428d4[1] = 0xc000;
        func_020731dc((int)data_ov006_021428d4, (int)NullDestructor_0203d47c, &data_ov006_02142948);
        data_ov006_021428cc |= 1;
    }
    func_ov006_02109530((int *)racer, data_ov006_021428d4, 0x80);
    *(unsigned char*)((char*)racer + 0x32) = 7;
}
}

// @symbol func_ov006_021092a0
extern "C" {
void func_ov006_021092a0(int *racer) {
    racer[0] = 0xc000;
    racer[1] = 0xc000;
    racer[6] = racer[0];
    racer[7] = racer[1];
    racer[4] = 0;
    racer[5] = 0;
    *(unsigned char*)((char*)racer + 0x32) = 6;
    *(short*)((char*)racer + 0x2c) = 0x25;
    *(short*)((char*)racer + 0x2e) = 0;
    racer[10] = 1;
}
}

// @symbol func_ov006_0210927c
extern "C" {
void func_ov006_0210927c(char* racer)
{
    *(short*)(racer + 0x2c) = 0x25;
    *(racer + 0x32) = 1;
    func_ov006_02109530((int *)racer, (int *)(racer + 0x20), 0x100);
}
}

// @symbol func_ov006_02108f2c
extern "C" {
void func_ov006_02108f2c(Thing* racer)
{
    u8 state;
    int b;
    s16 type;

    state = racer->state;
    if (state == 0 || state == 3) {
        ApproachLinear(racer->x, racer->targetX, racer->stepX);
        ApproachLinear(racer->y, racer->targetY, racer->stepY);
    }

    state = racer->state;
    if (state == 1) {
        func_ov006_02108e24((int *)racer);
        ApproachLinear(racer->x, racer->targetX, racer->stepX);
        ApproachLinear(racer->y, racer->targetY, racer->stepY);
    } else if (state == 7) {
        s32 out[2];
        ApproachLinear(racer->x, racer->targetX, racer->stepX);
        ApproachLinear(racer->y, racer->targetY, racer->stepY);
        Vec2_Sub(out, &racer->targetX, (int *)racer);
        if (out[0] != 0)
            return;
        if (out[1] != 0)
            return;
        racer->state = 6;
        func_ov004_020b1b40(1);
    } else if (state == 4) {
        s32 out[2];
        ApproachLinear(racer->x, racer->targetX, racer->stepX);
        ApproachLinear(racer->y, racer->targetY, racer->stepY);
        Vec2_Sub(out, &racer->targetX, (int *)racer);
        if (out[0] != 0)
            return;
        if (out[1] == 0) {
            racer->hop = 0x14;
            racer->state = 5;
        }
    } else if (state == 5) {
        s32 d;
        s32* p;
        s32 q;
        d = data_ov004_020b9488;
        p = (s32*)(((s32)racer + 4));
        q = -((racer->hop - (d >> 1)) << 12) / d - 0x600;
        *p += (s32)((((s64)q << 14) + 0x800) >> 12);
        *(s16*)(((s32)racer + 0x30)) -= 1;
        if (racer->hop == 0)
            racer->state = 6;
    } else if (state == 2) {
        s32 vec[2];
        u8 idx = data_020a0e40;
        if (data_020a0de8[idx * 4] != 0) {
            vec[0] = data_020a0dea[idx * 4] << 12;
            vec[1] = data_020a0deb[idx * 4] << 12;
            func_ov006_021094ac((RacerXY *)racer, vec);
        } else {
            type = func_ov006_02108650(racer->targetX >> 12, racer->targetY >> 12);
            racer->tile = type;
            if (type != 0x25) {
                b = 0;
                type = racer->tile;
                if (type >= 0xc && type <= 0x1c)
                    b = 1;
                if (b != false)
                    goto use_table;
                if (type >= 0x1d && type <= 0x22)
                    b = 1;
                else
                    b = 0;
                if (b == false)
                    goto no_table;
            use_table:
                func_ov006_02109530((int *)racer, (int *)&data_ov006_02142ab4[type * 8], 0x100);
                goto snd;
            no_table:
                func_ov006_0210935c((RacerPos *)racer, type);
            snd:
                Sound::PlayBank2_2D(0x15e);
            } else {
                Sound::PlayBank2_2D(0x15f);
                func_ov006_0210927c((char *)racer);
            }
            data_ov006_021428c8 = 0;
            racer->state = 1;
        }
        ApproachLinear(racer->x, racer->targetX, racer->stepX);
        ApproachLinear(racer->y, racer->targetY, racer->stepY);
    }
}
}

// @symbol func_ov006_02108e24
extern "C" {
int func_ov006_02108e24(int *racer)
{
  int idx;
  int flag;
  int stride;
  int dx;
  int dy;
  stride = 4;
  if (data_ov006_021428c8 != 0)
  {
    return 0;
  }
  idx = data_020a0e40;
  flag = 0;
  if (data_020a0de8[idx * 4])
  {
    if (data_020a0de9[idx * 4] != 0)
    {
      flag = 1;
    }
  }
  if (flag == 0)
  {
    return 0;
  }
  idx = data_020a0e40;
  dx = (racer[0] >> 0xc) - data_020a0dea[idx * stride];
  dy = (racer[1] >> 0xc) - data_020a0deb[idx * 4];
  if ((((dx >= (-8)) && (dx < 8)) && (dy >= (-8))) && (dy < 8))
  {
    Sound::PlayBank2_2D(0x15d);
    data_ov006_021428c8 = (void *)racer;
    *((u8 *) (((char *) racer) + 0x32)) = 2;
    racer[2] = dx << 0xc;
    racer[3] = dy << 0xc;
    return 1;
  }
  return 0;
}
}

// @symbol func_ov006_02108d28
extern "C" {
void func_ov006_02108d28(int *racer)
{
  int idx;
  int flag;
  int stride;
  int dx;
  int dy;
  int px, py, touchX, touchY;
  s16 tile;
  stride = 4;
  if (data_ov006_021428c8 != 0) return;
  idx = data_020a0e40;
  flag = 0;
  if (data_020a0de8[idx * 4])
  {
    if (data_020a0de9[idx * 4] != 0)
    {
      flag = 1;
    }
  }
  if (flag == 0) return;
  if (*((u8 *)(((char *)racer) + 0x32)) != 1) return;
  idx = data_020a0e40;
  px = racer[0];
  py = racer[1];
  touchX = data_020a0dea[idx * stride];
  tile = *((s16 *)(((char *)racer) + 0x2c));
  px >>= 12;
  touchY = data_020a0deb[idx * 4];
  py >>= 12;
  dx = px - touchX;
  dy = py - touchY;
  if (tile != 0x25) return;
  if (func_ov006_02108650(touchX, touchY) == 0x25) return;
  Sound::PlayBank2_2D(0x15d);
  data_ov006_021428c8 = (void *)racer;
  *((u8 *)(((char *)racer) + 0x32)) = 2;
  racer[2] = dx << 12;
  racer[3] = dy << 12;
}
}

// @symbol func_ov006_02108cc0
extern "C" {
void func_ov006_02108cc0(char *racer) {
    if (*(unsigned char *)(racer + 0x32) == 6) return;
    RenderOamBothScreens(data_ov006_02138c3c,
                        *(int *)racer >> 0xc,
                        *(int *)(racer + 4) >> 0xc,
                        -1,
                        *(int *)(racer + 0x28),
                        (void *)0);
    *(int *)(racer + 0x28) = 1;
}
}

// @symbol func_ov006_02108b90
/* The three range tests are kept as 0 or 1 values held in a local and
   tested; the ROM materialises them. The 0x23 case tests `tile < 6`, the
   opposite of the case beside it, and the two share one `payout = 2`. */
extern "C" {
int func_ov006_02108b90(short *racer, int tile)
{
    int landed = *(short *)((char *)racer + 0x2c);
    int payout = 0;
    int inRange;

    inRange = (landed <= 0xb) ? 1 : 0;
    if (inRange != 0) {
        if (landed == tile) payout = 0xc;
    } else {
        inRange = (landed >= 0xc && landed <= 0x1c) ? 1 : 0;
        if (inRange != 0) {
            inRange = (landed - 0xc) << 2;
            if (tile == *(short *)((char *)data_ov006_0212edb8 + inRange) ||
                tile == *(short *)((char *)data_ov006_0212edba + inRange))
                payout = 6;
        } else {
            inRange = (landed >= 0x1d && landed <= 0x22) ? 1 : 0;
            if (inRange != 0) {
                inRange = (landed - 0x1d) << 3;
                if (tile == *(short *)((char *)data_ov006_0212ed88 + inRange) ||
                    tile == *(short *)((char *)data_ov006_0212ed8a + inRange) ||
                    tile == *(short *)((char *)data_ov006_0212ed8c + inRange) ||
                    tile == *(short *)((char *)data_ov006_0212ed8e + inRange))
                    payout = 3;
            } else if ((landed == 0x23 && tile < 6) || (landed == 0x24 && tile >= 6)) {
                payout = 2;
            }
        }
    }
    *(short *)((char *)racer + 0x2e) = payout;
    return payout;
}
}

// @symbol func_ov006_02108650
extern "C" {
short func_ov006_02108650(int x, int y)
{
    short tile = 0x25;

    if (x > data_ov006_0212ed4c[0] && x <= data_ov006_0212ed4c[1]) {
        if (y > 0x10 && y <= 0x60) {
            tile = 0x23;
        } else if (y > 0x60 && y <= 0xb0) {
            tile = 0x24;
        }
    } else if (x > data_ov006_0212ed4c[1] && x <= data_ov006_0212ed4c[2]) {
        if (y > data_ov006_0212ed68[0] && y <= data_ov006_0212ed68[1]) {
            tile = 0;
        } else if (y > data_ov006_0212ed68[1] && y <= data_ov006_0212ed68[2]) {
            tile = 0x14;
        } else if (y > data_ov006_0212ed68[2] && y <= data_ov006_0212ed68[3]) {
            tile = 3;
        } else if (y > data_ov006_0212ed68[3] && y <= data_ov006_0212ed68[4]) {
            tile = 0x17;
        } else if (y > data_ov006_0212ed68[4] && y <= data_ov006_0212ed68[5]) {
            tile = 6;
        } else if (y > data_ov006_0212ed68[5] && y <= data_ov006_0212ed68[6]) {
            tile = 0x1a;
        } else if (y > data_ov006_0212ed68[6] && y <= data_ov006_0212ed68[7]) {
            tile = 9;
        }
    } else if (x > data_ov006_0212ed4c[2] && x <= data_ov006_0212ed4c[3]) {
        if (y > data_ov006_0212ed68[0] && y <= data_ov006_0212ed68[1]) {
            tile = 0xc;
        } else if (y > data_ov006_0212ed68[1] && y <= data_ov006_0212ed68[2]) {
            tile = 0x1d;
        } else if (y > data_ov006_0212ed68[2] && y <= data_ov006_0212ed68[3]) {
            tile = 0xe;
        } else if (y > data_ov006_0212ed68[3] && y <= data_ov006_0212ed68[4]) {
            tile = 0x1f;
        } else if (y > data_ov006_0212ed68[4] && y <= data_ov006_0212ed68[5]) {
            tile = 0x10;
        } else if (y > data_ov006_0212ed68[5] && y <= data_ov006_0212ed68[6]) {
            tile = 0x21;
        } else if (y > data_ov006_0212ed68[6] && y <= data_ov006_0212ed68[7]) {
            tile = 0x12;
        }
    } else if (x > data_ov006_0212ed4c[3] && x <= data_ov006_0212ed4c[4]) {
        if (y > data_ov006_0212ed68[0] && y <= data_ov006_0212ed68[1]) {
            tile = 1;
        } else if (y > data_ov006_0212ed68[1] && y <= data_ov006_0212ed68[2]) {
            tile = 0x15;
        } else if (y > data_ov006_0212ed68[2] && y <= data_ov006_0212ed68[3]) {
            tile = 4;
        } else if (y > data_ov006_0212ed68[3] && y <= data_ov006_0212ed68[4]) {
            tile = 0x18;
        } else if (y > data_ov006_0212ed68[4] && y <= data_ov006_0212ed68[5]) {
            tile = 7;
        } else if (y > data_ov006_0212ed68[5] && y <= data_ov006_0212ed68[6]) {
            tile = 0x1b;
        } else if (y > data_ov006_0212ed68[6] && y <= data_ov006_0212ed68[7]) {
            tile = 0xa;
        }
    } else if (x > data_ov006_0212ed4c[4] && x <= data_ov006_0212ed4c[5]) {
        if (y > data_ov006_0212ed68[0] && y <= data_ov006_0212ed68[1]) {
            tile = 0xd;
        } else if (y > data_ov006_0212ed68[1] && y <= data_ov006_0212ed68[2]) {
            tile = 0x1e;
        } else if (y > data_ov006_0212ed68[2] && y <= data_ov006_0212ed68[3]) {
            tile = 0xf;
        } else if (y > data_ov006_0212ed68[3] && y <= data_ov006_0212ed68[4]) {
            tile = 0x20;
        } else if (y > data_ov006_0212ed68[4] && y <= data_ov006_0212ed68[5]) {
            tile = 0x11;
        } else if (y > data_ov006_0212ed68[5] && y <= data_ov006_0212ed68[6]) {
            tile = 0x22;
        } else if (y > data_ov006_0212ed68[6] && y <= data_ov006_0212ed68[7]) {
            tile = 0x13;
        }
    } else if (x > data_ov006_0212ed4c[5] && x <= data_ov006_0212ed4c[6]) {
        if (y > data_ov006_0212ed68[0] && y <= data_ov006_0212ed68[1]) {
            tile = 2;
        } else if (y > data_ov006_0212ed68[1] && y <= data_ov006_0212ed68[2]) {
            tile = 0x16;
        } else if (y > data_ov006_0212ed68[2] && y <= data_ov006_0212ed68[3]) {
            tile = 5;
        } else if (y > data_ov006_0212ed68[3] && y <= data_ov006_0212ed68[4]) {
            tile = 0x19;
        } else if (y > data_ov006_0212ed68[4] && y <= data_ov006_0212ed68[5]) {
            tile = 8;
        } else if (y > data_ov006_0212ed68[5] && y <= data_ov006_0212ed68[6]) {
            tile = 0x1c;
        } else if (y > data_ov006_0212ed68[6] && y <= data_ov006_0212ed68[7]) {
            tile = 0xb;
        }
    }
    return tile;
}
}

// @symbol func_ov006_021085c0
extern "C" {
int func_ov006_021085c0(char *slider){
    *(void**)(slider + 8) = func_ov004_020adc68(0x21d);
    *(void**)(slider + 0xc) = func_ov004_020adc68(0x21e);
    Model::UpdateFileOffsets(**(BMD_File **)(slider + 8));
    Model::UpdateFileOffsets(**(BMD_File **)(slider + 0xc));
    if(((ModelBase *)(slider + 0x10))->SetFile(*(BMD_File **)(slider + 8), 1, -1) == 0)
        return 0;
    if(((ModelBase *)(slider + 0x60))->SetFile(*(BMD_File **)(slider + 0xc), 1, -1) == 0)
        return 0;
    *(short*)(slider + 0xcc) = 0;
    *(int*)(slider + 0xbc) = 0;
    return 1;
}
}

// @symbol func_ov006_0210858c
extern "C" {
int func_ov006_0210858c(ResPair *res)
{
    if (res->a)
        Ov004_Deallocate(res->a);
    if (res->b)
        Ov004_Deallocate(res->b);
    return 1;
}
}

// @symbol func_ov006_02108524
extern "C" {
void func_ov006_02108524(char *slider) {
    unsigned rand;
    *(int *)(slider + 0xb0) = 0x4200;
    *(int *)(slider + 0xb4) = 0;
    *(short *)(slider + 0xc0) = 0;
    *(short *)(slider + 0xc2) = 0;
    *(short *)(slider + 0xc6) = 0x400;
    *(short *)(slider + 0xc8) = -0x200;
    rand = (unsigned)RandomIntInternal(data_0209e650);
    *(int *)(slider + 0xb8) = ((rand & 0x7fffffff) >> 0x13) + 0x2000;
    *(int *)(slider + 0xd0) = 0;
    func_ov006_02108508(slider);
}
}

// @symbol func_ov006_02108508
extern "C" {
void func_ov006_02108508(char *slider) { *(Pair2W *)(slider + 0x0) = data_ov006_0213e2e8; }
}

// @symbol func_ov006_021082fc
extern "C" {
void func_ov006_021082fc(char *slider)
{
    int spin = *(int *)(slider + 0xb8);
    int pull = cstd::fdiv((int)(((s64)spin * spin + 0x800) >> 12), *(int *)(slider + 0xb0)) - 0xc00;

    spin = *(int *)(slider + 0xb8);
    int scaled = (int)(((s64)spin * 0x500 + 0x800) >> 12);
    int step = cstd::fdiv(scaled, *(int *)(slider + 0xb0));
    *(s16 *)(slider + 0xc6) = (s16)step;

    s16 *angleRef = (s16 *)(((int)slider + 0xc0));
    *angleRef = *angleRef + *(s16 *)(slider + 0xc6);

    *(int *)(slider + 0xb4) = (int)(((s64)*(int *)(slider + 0xb4) * 0xfe0 + 0x800) >> 12);

    *(int *)(((int)slider + 0xb4)) = *(int *)(((int)slider + 0xb4)) + pull;
    *(int *)(((int)slider + 0xb0)) = *(int *)(((int)slider + 0xb0)) + *(int *)(slider + 0xb4);

    int radius = *(int *)(slider + 0xb0);
    if (radius > 0x4200) {
        radius = 0x4200;
        *(int *)(slider + 0xb0) = radius;
        *(int *)(slider + 0xb4) = 0;
    } else if (radius < 0x2c00) {
        ApproachLinear(*(int *)(slider + 0xb8), 0xfffff667, 0x28);
        func_ov006_021082c4(slider);
    } else {
        if (*(s16 *)(slider + 0xcc) == 1) {
            *(int *)(slider + 0xbc) = 0;
            *(s16 *)(slider + 0xcc) = 2;
        }
    }

    if (*(int *)(slider + 0xd0) != 0) {
        ApproachLinear(*(int *)(slider + 0xb8), 0, 0x10);
    }

    u16 angle = *(u16 *)(slider + 0xc0);
    int dist = *(int *)(slider + 0xb0);
    int idx = angle >> 4;
    s16 val1 = data_02082214[idx * 2 + 1];
    s16 val0 = data_02082214[idx * 2];

    int x, y, z;
    struct Vec3 t;
    x = (int)(((s64)dist * val1 + 0x800) >> 12);
    y = (int)(((s64)dist * 0x400 + 0x800) >> 12) + 0x780;
    z = (int)(((s64)dist * val0 + 0x800) >> 12);
    ((int *)&t)[0] = x; ((int *)&t)[1] = y; ((int *)&t)[2] = z;
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, ((int *)&t)[0], ((int *)&t)[1], ((int *)&t)[2]);

    *(Matrix4x3 *)(slider + 0x7c) = data_020a0e68;
}
}

// @symbol func_ov006_021082c4
extern "C" {
void func_ov006_021082c4(char *slider)
{
    *(int *)(slider + 0xb8) += 0x140;
    *(short *)(slider + 0xc4) = -*(short *)(slider + 0xc2);
    *(struct Pair *)slider = data_ov006_0213e2f8;
}
}

// @symbol func_ov006_02107ea8
extern "C" {
void func_ov006_02107ea8(char *slider)
{
    int spinAbs;
    int pull;
    short sum;
    ShortTbl12 stops;
    short diff;
    short idx;
    int extra;
    int angidx, idx2;
    short t0, t1;
    int x, y, z;
    Vec3 t;

    spinAbs = *(int *)(slider + 0xb8) - 0x140;
    if (spinAbs < 0) spinAbs = -spinAbs;
    pull = cstd::fdiv((int)((((long long)spinAbs * spinAbs) + 0x800) >> 12), *(int *)(slider + 0xb0)) - 0xc00;

    *(short *)(slider + 0xc6) = cstd::fdiv(
        (int)((((long long)(*(int *)(slider + 0xb8)) * 0x500) + 0x800) >> 12),
        *(int *)(slider + 0xb0));

    *(short *)(((int)slider + 0xc0)) += *(short *)(slider + 0xc6);

    *(int *)(slider + 0xb4) = (int)((((long long)(*(int *)(slider + 0xb4)) * 0x600) + 0x800) >> 12);
    *(int *)(((int)slider + 0xb4)) += pull;
    *(int *)(((int)slider + 0xb0)) += *(int *)(slider + 0xb4);

    if (*(int *)(slider + 0xb0) > 0x2c00) {
        *(int *)(slider + 0xb0) = 0x2c00;
        *(int *)(slider + 0xb4) = 0;
    } else if (*(int *)(slider + 0xb0) < 0x1600) {
        *(int *)(slider + 0xb0) = 0x1600;
        *(int *)(slider + 0xb4) = 0;
    }

    sum = *(short *)(slider + 0xc0) + *(short *)(slider + 0xc4);

    stops = *(ShortTbl12 *)data_ov006_0213e334;

    for (idx = 0; idx < 12; idx++) {
        short ad;
        diff = sum - stops.v[idx];
        ad = diff < 0 ? (short)(-diff) : diff;
        if (ad < 0xaab) break;
    }

    *(short *)(slider + 0xca) = data_ov006_0212ed34[idx];

    extra = 0;
    if (*(int *)(slider + 0xb0) < 0x2000) {
        if (diff < -0x400 && (diff - *(short *)(slider + 0xc6)) >= -0x400) {
            *(short *)(slider + 0xc0) = (stops.v[idx] - 0x400) - *(short *)(slider + 0xc4);
            *(int *)(slider + 0xb8) = -(int)((((long long)(*(int *)(slider + 0xb8)) * 0xf00) + 0x800) >> 12);
            ApproachLinear(*(int *)(slider + 0xb8), 0xfffff667, 0x30);
            if (*(short *)(slider + 0xcc) != 0) {
                Sound::PlayBank2_2D(0x159);
                *(short *)(slider + 0xcc) = 0;
            }
        } else if (diff > 0x400 && (diff - *(short *)(slider + 0xc6)) <= 0x400) {
            *(short *)(slider + 0xc0) = (stops.v[idx] + 0x400) - *(short *)(slider + 0xc4);
            *(int *)(slider + 0xb8) = -(int)((((long long)(*(int *)(slider + 0xb8)) * 0xe00) + 0x800) >> 12);
            if (*(short *)(slider + 0xcc) != 0) {
                Sound::PlayBank2_2D(0x159);
                *(short *)(slider + 0xcc) = 0;
            }
        } else {
            if (diff < 0) diff = -diff;
            if (diff > 0x400) {
                *(int *)(slider + 0xb0) = 0x2000;
                *(int *)(slider + 0xb4) = 0;
            }
        }
        extra = (int)((((long long)(*(int *)(slider + 0xb0) - 0x2000) * 0x100) + 0x800) >> 12);
    }

    ApproachLinear(*(int *)(slider + 0xb8), 0, 0x30);

    angidx = ((unsigned short)(sum + *(short *)(slider + 0xc2))) >> 4;
    idx2 = angidx * 2;
    t0 = data_02082214[idx2 + 1];
    t1 = data_02082214[idx2];

    x = (int)((((long long)(*(int *)(slider + 0xb0)) * t0) + 0x800) >> 12);
    y = (int)((((long long)(*(int *)(slider + 0xb0)) * 0x400) + 0x800) >> 12) + 0x780 + extra;
    z = (int)((((long long)(*(int *)(slider + 0xb0)) * t1) + 0x800) >> 12);
    ((int *)&t)[0] = x; ((int *)&t)[1] = y; ((int *)&t)[2] = z;
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, ((int *)&t)[0], ((int *)&t)[1], ((int *)&t)[2]);

    *(Matrix4x3 *)(slider + 0x7c) = data_020a0e68;
}
}

// @symbol func_ov006_02107db8
extern "C" void func_ov006_02107db8(C *c)
{
    char *raw = (char *)c;
    short *p = (short *)(((int)raw + 0xc2));
    *p = (short)(*p + *(short *)(raw + 0xc8));
    *(short *)(*(int *)(raw + 0x20) + 0x82) = (short)(-*(short *)(raw + 0xc2));
    c->obj.v3();
    Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, 0);
    *(Matrix4x3 *)(raw + 0x2c) = *(Matrix4x3 *)&data_020a0e68;
    (c->*(c->pmf))();
    {
        short s = *(short *)(raw + 0xcc);
        if (s == 0)
            return;
        if (s == 2)
            *(int *)(raw + 0xbc) = Sound_PlayIfNotActive(*(int *)(raw + 0xbc), 2, 0x158, 0);
        else
            *(int *)(raw + 0xbc) = Sound_PlayIfNotActive(*(int *)(raw + 0xbc), 2, 0x157, 0);
    }
}

// @symbol func_ov006_02107d80
extern "C" void func_ov006_02107d80(char* slider){
    ((Sub*)(slider + 0x10))->m(0);
    ((Sub*)(slider + 0x60))->m(0);
}

// @symbol func_ov006_02107d58
extern "C" {
void func_ov006_02107d58(int *strip){
  int i;
  for(i=0;i<5;i++) strip[i]=0;
  *(short*)((char*)strip+0x14)=4;
  *(short*)((char*)strip+0x16)=0;
}
}

// @symbol func_ov006_02107d20
extern "C" {
void func_ov006_02107d20(int *strip, int racer)
{
    int i;
    *(short *)((char *)strip + 0x16) += 1;
    for (i = 0; i < 5; i++) {
        if (strip[i] == 0) {
            strip[i] = racer;
            return;
        }
    }
}
}

// @symbol func_ov006_02107c50
extern "C" {
void func_ov006_02107c50(short* strip, int y)
{
    int idx = GetGameLanguage();
    void* p = *(void**)((char*)data_ov006_0213e320[idx] + 0x50);
    Hud_RenderSprite(p, 0xc, y, -1, -1);

    func_ov004_020b2444(0x1e, y, data_ov006_0212ed14[*(short*)((char*)strip + 0x14)], 0, -1, 2, 0);

    {
        int b = y + 0x10;
        RenderOamBothScreens(data_ov006_02138c3c, 0x20, b, -1, 1, (void*)0);
        func_ov004_020b2444(0x2e, b, *(short*)((char*)strip + 0x16), 0, -1, 1, 0);
    }
}
}

// @symbol func_ov006_02107b94
extern "C" {
void func_ov006_02107b94(void **strip)
{
    int i;
    for (i = 0; i < 5; i++) {
        char *obj = (char *)strip[i];
        int m, idx, r0, a1, a2;
        if (obj == 0) return;
        *(int *)(obj + 0x28) = 0;
        r0 = *(short *)((char *)strip + 0x14);
        m = *(short *)(obj + 0x2c);
        idx = data_ov006_0212ed24[r0];
        if (data_020a0db0 & 8) idx++;
        if (m >= 0xc && m <= 0x13) idx += 2;
        a1 = ((TileXY *)data_ov006_02142ab4)[m].x >> 12;
        a2 = ((TileXY *)data_ov006_02142ab4)[m].y >> 12;
        if (r0 == 3) a1 += 0x58;
        Hud_RenderSprite(data_ov006_021376ec[idx], a1, a2, -1, -1);
    }
}
}

// @symbol func_ov006_02107b70
extern "C" {
void func_ov006_02107b70(char* p) {
    int i;
    for (i = 0; i < 4; i++) {
        data_ov006_02142aa4[i] = p;
        p += 0x18;
    }
}
}

// @symbol func_ov006_02107b14
extern "C" {
void func_ov006_02107b14(void) {
    int i;
    for (i = 0; i < 4; i++) {
        func_ov006_02107d58((int *)data_ov006_02142aa4[i]);
        *(short*)((char*)data_ov006_02142aa4[i] + 0x14) = (short)i;
    }
    data_ov006_0213e2dc = -1;
    data_ov006_021428d0 = 0;
}
}

// @symbol func_ov006_02107a6c
extern "C" int func_ov006_02107a6c(void)
{
    if (ApproachLinear(data_ov006_021428d0, 0, 1) != 0) {
        int idx;
        data_ov006_0213e2dc = data_ov006_0213e2dc + 1;
        idx = data_ov006_0213e2dc;
        data_ov006_021428d0 = 0x78;
        while (idx < 4) {
            if (*(short*)(data_ov006_02142aa4[idx] + 0x16) != 0) break;
            data_ov006_0213e2dc = data_ov006_0213e2dc + 1;
            idx = data_ov006_0213e2dc;
        }
        if (idx < 4) {
            func_02012790(0x26);
        }
    }
    return (data_ov006_0213e2dc >= 4) ? 1 : 0;
}

// @symbol func_ov006_021079cc
extern "C" {
void func_ov006_021079cc(void){
    int n = data_ov006_0213e2dc;
    int arg;
    int i;
    if (n < 0) return;
    n = n + 1;
    if (n > 4) n = 4;
    arg = 0x28;
    for (i = 0; i < n; i++) {
        int* o = (int *)data_ov006_02142aa4[i];
        if (*(short*)((char*)o + 0x16) > 0) {
            func_ov006_02107c50((short *)o, arg);
            arg += 0x24;
        }
    }
    if (data_ov006_0213e2dc >= 4) return;
    func_ov006_02107b94((void **)data_ov006_02142aa4[data_ov006_0213e2dc]);
}
}

// @symbol func_ov006_021079c8
extern "C" {
void func_ov006_021079c8(void)
{
}
}

/* The destructor pair, D1 at 0x0210788c and D0 at 0x02107920, is not
   defined here. It is declared inline as the class's first member in
   dScMgRoulette_c.h: that is the only form that makes the compiler emit D1
   below D0 with no D2, as the cartridge has them. */
