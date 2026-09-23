//cpp
/* The flower-petal minigame scene ("loves me, loves me not"): the player drags
 * petals off a flower with the stylus while a face watches. 9 functions.
 *
 * The pragmas carry the codegen; among them:
 *   - `#pragma defer_codegen off`: functions are emitted in source order and
 *     the destructor variants land where they are written. Do not reorder.
 *   - `#pragma opt_common_subs off` around Behavior only: with it on, the
 *     touch-sample loads get merged; turning it off file-wide breaks two
 *     other functions instead.
 * The destructor is defined out of line so D1 and D0 come out in cartridge
 * order; the extra D2 is dropped at link. The factory is in
 * src/d_s_mg_flower.cpp.
 *
 * Blocked: the 0x16-entry petal table at 0x4f38 is not typed in
 * dScMgFlower_c.h (mArray is a plain byte array), so the functions here
 * reach it by offset or through the local Petal view below. Several of those offset
 * spellings are codegen levers, noted where they occur.
 */

#include "dScMgFlower_c.h"
#include "types.h"
#include "decl_common.h"
#include "Sound.h"
#include "G2x.h"

#pragma defer_codegen off
#pragma opt_strength_reduction off

/* One 0x20-byte petal record, as the per-frame update reads it. */
typedef struct Petal {
    u8 active; u8 released; u8 held; u8 counted;
    s32 x; s32 y; s32 driftX; s32 fallSpeed;
    s32 grabX; s32 grabY; s32 angle;
} Petal;

/* The scene seen as the petal table plus the two fields the update needs. */
typedef struct PetalView {
    u8 pad0[0x4f38];
    Petal petals[22];
    u8 pad1[0x5fc8 - 0x51f8];
    s32 heldPetal;
    u8 pad2[1];
    u8 settled;
} PetalView;

/* The first two OAM attribute words, copied whole from the petal template. */
struct SpriteAttr { int a, b; };

/* Address launders. Each forces an address through an integer so mwccarm
 * rebuilds it at every use instead of sharing one register; removing any of
 * them changes the code. */
#define M(p) ((long long)(int)(p))
#define A1(off) ((int)M((int)M((char *)self + (int)M(i) * 32) + (off)))
#define A2(off) ((int)M((int)M((char *)self + (unsigned)i * 32) + (off)))
#define B1(off) (*(u8 *)A1(off))
#define W1(off) (*(s32 *)A1(off))
#define W2(off) (*(s32 *)A2(off))
#define LB(p) ((int)((int)(p)))
#define LA(p) ((int)(p))

extern "C" {
extern int RandomIntInternal(int *seed);
extern void func_0203d388(int *p, int angle);
extern void func_ov004_020b04d0(int n);
extern int data_0209d4b8;
extern s16 data_02082214[];
void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
void func_ov004_020af770(void *a0, int a1, int a2, int a3, int a4, int a5, u16 a6);
typedef struct V2 {
int x;
int z;
} V2;
typedef struct Cell {
int x;
int z;
char pad[0x18];
} Cell;
void Vec2_Sub(V2 *out, V2 *a, V2 *b);
int Vec2_Len(V2 *p);
void FreeGfxSlotsById(int a);
extern u8 data_020a0e40[];
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
void *func_ov004_020adc74(void *arg);
void DecompressLZ16(int src, void *dst);
void Ov004_Deallocate(void *ptr);
/* G2's calls stay mangled: decl_common.h declares a global named G2. */
char *_ZN2G213GetBG2CharPtrEv(void);
char *_ZN2G212GetBG2ScrPtrEv(void);
extern int data_0208ee44;
extern u8 data_0209d45c;
extern u8 data_0209d454;
}

namespace G2S  { void *GetBG2CharPtr(); char *GetBG2ScrPtr(); char *GetBG3ScrPtr(); }
namespace GX   { void LoadBGPltt(const void *src, u32 offset, u32 size); void LoadOBJPltt(const void *src, u32 offset, u32 size); }
namespace GXS  { void LoadBGPltt(const void *src, u32 offset, u32 size); void LoadOBJPltt(const void *src, u32 offset, u32 size); }
namespace CP15 { void FlushAndInvalidateDataCache(u32 address, u32 length); }

/* Written first so D1 and D0 are emitted first, as in the cartridge. Frees
 * the 0x51f8 object, then the petal table, in reverse of construction; the
 * rest of the teardown is the inlined base destructors. */
dScMgFlower_c::~dScMgFlower_c()
{
    func_ov006_020c3e70((char *)this + 0x51f8);
    __cxa_vec_cleanup(mArray, 0x16, 0x20, (void *)func_ov006_0212a650);
}

// @symbol func_ov006_0212a650
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0212a650(void)
{
}
}

// @symbol func_ov006_0212a654
extern "C" {  /* .c-derived member: C linkage for the whole block */
/* Per-frame petal update: released petals drift and fall, speeding up to
 * 0x4000, and go inactive once below the screen. `settled` stays 1 only if
 * no petal is still on the flower, held, or falling. */
void func_ov006_0212a654(char *p)
{
    PetalView *self = (PetalView *)p;
    int i;

    self->settled = 1;
    for (i = 0; i < 22; i++) {
        if (B1(0x4f38) == 0) continue;
        if (self->petals[i].held == 1) { self->settled = 0; continue; }
        if (self->petals[i].released == 0) { self->settled = 0; continue; }
        if (i == self->heldPetal) continue;
        W1(0x4f48) += 0x100;
        if (W1(0x4f48) >= 0x4000) W1(0x4f48) = 0x4000;
        W2(0x4f3c) += self->petals[i].driftX;
        W2(0x4f40) += W1(0x4f48);
        if (W2(0x4f40) >= 0x100000) B1(0x4f38) = 0;
        else self->settled = 0;
    }
}
}

// @symbol func_ov006_0212a764
/* Round setup, called by InitResources and by OnYoshiTryEat when a round
 * restarts. Clears the cursor, wipes all 0x16 petal records and rolls the
 * petal count (1-2 when the hold counter reads exactly 0x14, else a 1-in-10
 * chance of 20-21, otherwise 8-15). The petals are laid out on a circle
 * (angle = base + i * full turn / count, a (0, 3.0) offset rotated by that
 * angle and moved to (8.0, 6.0)), shuffled by swapping angle and position
 * with random partners, and the round state is reset.
 *
 * The petal fields stay raw offsets on purpose: only
 * `(unsigned int)(c + i * 0x20) + offset` keeps the offset out of the
 * addressing mode the way the ROM does; member access or named pointers
 * change the loop. pAngle and pX must stay block locals with pX declared
 * after the angle store, and the swap temp must be a Vec2 (its empty
 * destructor keeps its stack slot). */
extern "C" void func_ov006_0212a764(void *p)
{
    dScMgFlower_c *self = (dScMgFlower_c *)p;
    char *c = (char *)self;
    unsigned int roll;
    int j;
    int i2;
    int angle;
    int *pPos;
    int i;
    int baseAngle;

    self->mCursorX = 0;
    self->mCursorY = 0;
    self->mPrevCursorX = self->mCursorX;
    self->mPrevCursorY = self->mCursorY;
    func_ov004_020b04d0(0x20);
    for (i = 0; i < 0x16; i++) {
        *(u8 *)(c + i * 0x20 + 0x4f38) = 0;
        *(u8 *)(c + i * 0x20 + 0x4f3a) = 0;
        *(u8 *)(c + i * 0x20 + 0x4f39) = 0;
        *(u8 *)(c + i * 0x20 + 0x4f3b) = 0;
        *(s32 *)(c + i * 0x20 + 0x4f3c) = 0;
        *(s32 *)(c + i * 0x20 + 0x4f40) = 0;
        *(s32 *)(c + i * 0x20 + 0x4f4c) = 0;
        *(s32 *)(c + i * 0x20 + 0x4f50) = 0;
        *(s32 *)(c + i * 0x20 + 0x4f44) = 0;
        *(s32 *)(c + i * 0x20 + 0x4f48) = 0;
    }
    if (self->mHoldTimer == 0x14) {
        roll = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        self->mPetalsLeft = ((roll * 2) >> 15) + 1;
    } else {
        int raw = RandomIntInternal(&data_0209d4b8);
        roll = ((unsigned int)raw >> 16) & 0x7fff;
        if (((roll * 10) >> 15) == 0) {
            roll = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            self->mPetalsLeft = ((roll * 2) >> 15) + 0x14;
        } else {
            roll = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            self->mPetalsLeft = ((roll * 8) >> 15) + 8;
        }
    }
    roll = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    baseAngle = (roll * 0x10000) >> 15;
    i2 = 0;
    if (self->mPetalsLeft > 0) {
        pPos = (int *)(c + 0x4f3c);
        angle = 0;
        do {
            s16 *pAngle = (s16 *)((unsigned int)(c + i2 * 0x20) + 0x4f54);
            *pAngle = baseAngle + angle / self->mPetalsLeft;
            {
                s32 *pX = (s32 *)((unsigned int)(c + i2 * 0x20) + 0x4f3c);
                *pX = 0;
                *(s32 *)((unsigned int)(c + i2 * 0x20) + 0x4f40) = 0x30000;
                func_0203d388(pPos, *pAngle);
                *pX += 0x80000;
                *(s32 *)((unsigned int)(c + i2 * 0x20) + 0x4f40) += 0x60000;
            }
            *(u8 *)((unsigned int)c + i2 * 0x20 + 0x4f38) = 1;
            i2++;
            angle += 0x10000;
            pPos += 8;
        } while (i2 < self->mPetalsLeft);
    }
    j = 0;
    if (self->mPetalsLeft > 0) {
        do {
            int k;
            roll = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            k = (self->mPetalsLeft * roll) >> 15;
            if (k != j) {
                char *ej = c + j * 0x20;
                char *ek = c + k * 0x20;
                u16 t16;
                dScMgFlower_c::Vec2 tmp;
                t16 = *(u16 *)((unsigned int)ej + 0x4f54);
                *(u16 *)((unsigned int)ej + 0x4f54) = *(u16 *)((unsigned int)ek + 0x4f54);
                *(u16 *)((unsigned int)ek + 0x4f54) = t16;
                tmp.x = *(s32 *)((unsigned int)ej + 0x4f3c);
                tmp.y = *(s32 *)((unsigned int)ej + 0x4f40);
                *(s32 *)((unsigned int)ej + 0x4f3c) = *(s32 *)((unsigned int)ek + 0x4f3c);
                *(s32 *)((unsigned int)ej + 0x4f40) = *(s32 *)((unsigned int)ek + 0x4f40);
                *(s32 *)((unsigned int)ek + 0x4f3c) = tmp.x;
                *(s32 *)((unsigned int)ek + 0x4f40) = tmp.y;
            }
            j++;
        } while (j < self->mPetalsLeft);
    }
    self->mHeldPetal = -1;
    self->mPetalToggle = 0;
    self->mState = 0;
    self->mHintTimer = 0;
    self->mResultTimer = 0;
    self->mFaceSprite = 2;
    self->unk_5fcd = 0;
    self->mBgScrollPhase = 0;
    *(volatile u16 *)0x4000050 = 0;
    G2x::SetBlendAlpha((volatile u16 *)0x4001050, 4, 8, 6, 0xa);
}

// @symbol _ZN13dScMgFlower_c13OnYoshiTryEatEi
/* Slot 18: bump the hold counter at 0x5fe4 (reset once it passes 0x14),
 * update the 0x51f8 object and restart the round. The counter goes through a
 * volatile pointer so the ROM's separate reload and branch come out. */
void dScMgFlower_c::OnYoshiTryEat(int /* arg */)
{
    char *raw = (char *)this;

    if (mHoldTimer <= 0x14) {
        (*(volatile int *)(raw + 0x5fe4))++;
    } else {
        mHoldTimer = 0;
    }
    func_ov006_020c3bc8(raw + 0x51f8);
    func_ov006_0212a764(raw);
}

// @symbol _ZN13dScMgFlower_c6RenderEv
/* Slot 9. Scrolls the two sub-screen background layers off the sine table
 * (mBgScrollPhase steps 0xc0 a frame), draws the face, then one sprite per
 * live petal; the held petal gets a different palette. The row-array views
 * index the untyped petal table. */
s32 dScMgFlower_c::Render()
{
    char *self = (char *)this;
    int i;
    u8 (*barr)[0x20] = (u8 (*)[0x20])self;
    int (*iarr)[8] = (int (*)[8])self;
    u16 (*harr)[0x10] = (u16 (*)[0x10])self;

    mBgScrollPhase += 0xc0;
    {
        int sine = data_02082214[(mBgScrollPhase >> 4) << 1];
        int t = sine + 0x80;
        int off = (t + (int)((unsigned)(t >> 7) >> 24)) >> 8;
        SetSubBg2Offset(off, off + 8);
    }
    {
        int sine = data_02082214[(mBgScrollPhase >> 4) << 1];
        int t = 0x80 - sine;
        int off = (t + (int)((unsigned)(t >> 7) >> 24)) >> 8;
        SetSubBg3Offset(off, off);
    }
    func_ov004_020afdd0(data_ov006_0213ab94[mFaceSprite], 0x80, 0x60, -1, 1);

    for (i = 0; i < 0x16; i++) {
        if (barr[i][0x4f38] != 0) {
            struct SpriteAttr attr = *(struct SpriteAttr*)data_ov006_0213abe0;
            int palette = 1;
            attr.a = (attr.a & 0xc1fffcff) | 0x100;
            if (mHeldPetal == i)
                palette = 0;
            func_ov004_020af770(
                &attr,
                iarr[i][0x13cf] >> 12,
                iarr[i][0x13d0] >> 12,
                -1,
                palette,
                0x1000,
                (u16)(s16)(harr[i][0x27aa] + 0x8000));
        }
    }

    func_ov006_020c3b2c(self + 0x4660);
    func_ov006_020c3bf4(self + 0x51f8);
    return 1;
}

// @symbol _ZN13dScMgFlower_c8BehaviorEv
/* Slot 6, the whole minigame: pick up the petal nearest the stylus, drag it,
 * drop it, flip "loves me" and "loves me not", and run the end-of-round
 * results. LA and LB are launders, not casts (see above), and the pragma
 * around this function is required. */
#pragma opt_common_subs off
s32 dScMgFlower_c::Behavior()
{
    char *c = (char *)this;
    struct dScMgFlower_c *self = this;
    int i;
    int k;
    int t;
    V2 toCursor;
    V2 toCursor2;
    V2 drag;

    if (self->mPromptBlinkCount == 0) {
        self->mPromptEnabled = 1;
        self->mPromptBlinkCount = 1;
        self->mPromptBlinkTimer = 0;
    }
    if (self->mHintTimer > 0) {
        if (self->mHeldPetal == -1) {
            *(int *)LA(c + 0x5fd0) -= 1;
            if (self->mHintTimer <= 0) {
                if (self->mPetalsLeft >= 1)
                    self->mFaceSprite = 2;
            }
        } else {
            self->mHintTimer = 0x3c;
        }
    }
    switch (self->mState) {
    case 0:
        self->mPrevCursorX = self->mCursorX;
        self->mPrevCursorY = self->mCursorY;
        k = data_020a0e40[0];
        if (data_020a0de8[k * 4] != 0) {
            int b = data_020a0deb[k * 4];
            int a = data_020a0dea[k * 4];
            self->mCursorX = (b ? a : a) << 12;
            self->mCursorY = b << 12;
        }
        if (self->mHoldTimer > 0x14) {
            if (self->mPetalsLeft > 0) {
                for (i = 0; i < self->mPetalsLeft; i++) {
                    *(u8 *)(c + i * 0x20 + 0x4f39) = 1;
                    *(u8 *)(c + i * 0x20 + 0x4f3b) = 1;
                }
                self->mPetalsLeft = 0;
                Sound::PlayBank2_2D(0x10d);
                FreeGfxSlotsById(0x1d);
            }
        } else if (self->mHeldPetal < 0) {
            int t;
            if (data_020a0de8[k * 4] != 0 && data_020a0de9[k * 4] != 0)
                t = 1;
            else
                t = 0;
            if (t != 0) {
                int ii;
                V2 *petalPos = (V2 *)(c + 0x4f3c);
                for (ii = 0; ii < 0x16; ii++) {
                    if (*(u8 *)(c + ii * 0x20 + 0x4f38) != 0 &&
                        *(u8 *)(c + ii * 0x20 + 0x4f3a) != 1 &&
                        *(u8 *)(c + ii * 0x20 + 0x4f39) != 1) {
                        int near;
                        Vec2_Sub(&toCursor, petalPos, (V2 *)(c + 0x5fb8));
                        near = Vec2_Len(&toCursor) < 0x18000 ? 1 : 0;
                        if (near != 0) {
                            Sound::PlayBank2_2D(0x109);
                            *(u8 *)(c + ii * 0x20 + 0x4f3a) = 1;
                            *(int *)(c + ii * 0x20 + 0x4f44) = 0;
                            *(int *)(c + ii * 0x20 + 0x4f48) = 0;
                            self->mHeldPetal = ii;
                            *(int *)(c + ii * 0x20 + 0x4f4c) = self->mCursorX;
                            *(int *)(c + ii * 0x20 + 0x4f50) = self->mCursorY;
                            FreeGfxSlotsById(0x1d);
                            self->mFaceSprite = 2;
                            break;
                        }
                    }
                    petalPos = (V2 *)((char *)petalPos + 0x20);
                }
                if (self->mHeldPetal < 0) {
                    int i2;
                    V2 *petalPos2 = (V2 *)(c + 0x4f3c);
                    for (i2 = 0; i2 < 0x16; i2++) {
                        if (*(u8 *)(c + i2 * 0x20 + 0x4f38) != 0 &&
                            *(u8 *)(c + i2 * 0x20 + 0x4f3a) != 1) {
                            int near2;
                            Vec2_Sub(&toCursor2, petalPos2, (V2 *)(c + 0x5fb8));
                            near2 = Vec2_Len(&toCursor2) < 0x18000 ? 1 : 0;
                            if (near2 != 0) {
                                *(u8 *)(c + i2 * 0x20 + 0x4f3a) = 1;
                                *(int *)(c + i2 * 0x20 + 0x4f44) = 0;
                                *(int *)(c + i2 * 0x20 + 0x4f48) = 0;
                                self->mHeldPetal = i2;
                                *(int *)(c + i2 * 0x20 + 0x4f4c) = self->mCursorX;
                                *(int *)(c + i2 * 0x20 + 0x4f50) = self->mCursorY;
                                break;
                            }
                        }
                        petalPos2 = (V2 *)((char *)petalPos2 + 0x20);
                    }
                }
            }
        } else {
            t = 0;
            if (data_020a0de8[k * 4] != 0) {
                Cell *cells;
                int j;
                Vec2_Sub(&drag, (V2 *)(c + 0x5fb8), (V2 *)(c + 0x5fc0));
                cells = (Cell *)(c + 0x4f3c);
                j = self->mHeldPetal;
                cells[j].x += drag.x;
                *(int *)LB((char *)cells + j * 0x20 + 4) += drag.z;
            } else {
                if (*(u8 *)(c + self->mHeldPetal * 0x20 + 0x4f39) == 0) {
                    *(u8 *)(c + self->mHeldPetal * 0x20 + 0x4f39) = 1;
                    if (*(u8 *)(c + self->mHeldPetal * 0x20 + 0x4f3b) == 0) {
                        t = 1;
                        *(u8 *)(c + self->mHeldPetal * 0x20 + 0x4f3b) = 1;
                        func_ov006_020c3990(c + 0x51f8);
                    }
                }
                *(u8 *)(c + self->mHeldPetal * 0x20 + 0x4f3a) = 0;
                self->mHeldPetal = -1;
            }
            if (t != 0) {
                *(int *)LA(c + 0x5fd8) -= 1;
                if (self->mPetalToggle == 1) {
                    self->mPetalToggle = 0;
                    if (self->mPetalsLeft >= 1) {
                        FreeGfxSlotsById(0x1d);
                        func_ov004_020b0cac(0x13, 0x80, 0x18, 0, -1, 0xd);
                        self->mFaceSprite = 3;
                        Sound::PlayBank2_2D(0x104);
                    }
                } else {
                    self->mPetalToggle = 1;
                    if (self->mPetalsLeft >= 1) {
                        FreeGfxSlotsById(0x1d);
                        func_ov004_020b0cac(0x10, 0x80, 0x18, 0, -1, 0xd);
                        self->mFaceSprite = 1;
                        Sound::PlayBank2_2D(0x103);
                    }
                }
                self->mHintTimer = 0x3c;
            }
        }
        if (self->mPetalsLeft <= 0 && self->unk_5fcd == 1) {
            if (self->mResultTimer > 0) {
                *(int *)LA(c + 0x5fd4) -= 1;
                if (self->mResultTimer <= 0) {
                    self->mState = 1;
                    func_ov004_020b0a54(0xc);
                }
                if (func_ov006_020c3b80(c + 0x51f8) != 0) {
                    if (self->mPetalToggle == 1)
                        func_ov006_020c3908(c + 0x51f8);
                    else
                        func_ov006_020c38b0(c + 0x51f8);
                }
            } else {
                FreeGfxSlotsById(0x1d);
                if (self->mHoldTimer > 0x14) {
                    if (self->unk_5fcd == 1) {
                        Sound::PlayBank2_2D(0x106);
                        self->mFaceSprite = 4;
                        self->mResultTimer = 0x3c;
                    }
                } else {
                if (self->mPetalToggle == 1) {
                    self->mFaceSprite = 0;
                    *(int *)LA(c + 0x5fdc) += 1;
                    self->mLoseStreak = 0;
                    if (self->mWinStreak >= 3) {
                        func_ov004_020b0cac(0x12, 0x80, 0x18, 0, -1, 0xd);
                        Sound::PlayBank2_2D(0x107);
                        *(int *)LA(c + 0x5ff0) += 3;
                        if (self->mScore > 0x270f)
                            self->mScore = 0x270f;
                    } else {
                        func_ov004_020b0cac(0x10, 0x80, 0x18, 0, -1, 0xd);
                        Sound::PlayBank2_2D(0x108);
                        *(int *)LA(c + 0x5ff0) += 1;
                        if (self->mScore > 0x270f)
                            self->mScore = 0x270f;
                    }
                } else {
                    self->mFaceSprite = 4;
                    *(int *)LA(c + 0x5fe0) += 1;
                    self->mWinStreak = 0;
                    if (self->mLoseStreak >= 3) {
                        func_ov004_020b0cac(0x11, 0x80, 0x18, 0, -1, 0xd);
                        Sound::PlayBank2_2D(0x105);
                    } else {
                        func_ov004_020b0cac(0x13, 0x80, 0x18, 0, -1, 0xd);
                        Sound::PlayBank2_2D(0x106);
                    }
                }
                self->mResultTimer = 0x3c;
                }
            }
        }
        func_ov006_0212a654(c);
        break;
    case 1:
        if (func_ov006_020c3b80(c + 0x51f8) != 0) {
            if (self->mPetalToggle == 1)
                func_ov006_020c3908(c + 0x51f8);
            else
                func_ov006_020c38b0(c + 0x51f8);
        }
        self->mPromptEnabled = 0;
        func_ov006_0212a654(c);
        break;
    default:
        break;
    }
    func_ov006_020c3d18(c + 0x51f8);
    return 1;
}
#pragma opt_common_subs on

// @symbol _ZN13dScMgFlower_c13InitResourcesEv
/* Slot 0. Sets up BG2 and BG3 on both screens, decompresses their tiles and
 * maps, loads the palettes and the OBJ tiles, then starts the first round.
 * The bare 0x040000xx stores are the BGxCNT and BGxOFS registers, and
 * 0x06400000 and 0x06600000 are the two OBJ VRAM banks. */
s32 dScMgFlower_c::InitResources()
{
    char *raw = (char *)this;
    int h;

    mScore = func_ov004_020ad8b8();

    data_0208ee44 = 1;

    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x1414;
    *(volatile u16 *)0x400000c &= ~0x40;
    *(volatile u32 *)0x4000018 = 0;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 3;

    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x1414;
    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile u32 *)0x4001018 = 0;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 3;

    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x1414;
    *(volatile u16 *)0x400100e &= ~0x40;
    *(volatile u32 *)0x400101c = 0;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 3;

    h = (int)func_ov004_020adc74(&data_ov006_021401d0);
    DecompressLZ16(h, _ZN2G213GetBG2CharPtrEv());
    Ov004_Deallocate((void *)h);

    h = (int)func_ov004_020adc74(&data_ov006_021401f4);
    DecompressLZ16(h, G2S::GetBG2CharPtr());
    Ov004_Deallocate((void *)h);

    h = (int)func_ov004_020adc74(&data_ov006_02140218);
    DecompressLZ16(h, _ZN2G212GetBG2ScrPtrEv());
    Ov004_Deallocate((void *)h);

    h = (int)func_ov004_020adc74(&data_ov006_0214023c);
    DecompressLZ16(h, G2S::GetBG2ScrPtr());
    DecompressLZ16(h, G2S::GetBG3ScrPtr());
    Ov004_Deallocate((void *)h);

    h = (int)func_ov004_020adc74(&data_ov006_02140260);
    CP15::FlushAndInvalidateDataCache(h, 0x1a0);
    GX::LoadBGPltt((const void *)h, 0x60, 0x1a0);
    Ov004_Deallocate((void *)h);

    h = (int)func_ov004_020adc74(&data_ov006_02140284);
    CP15::FlushAndInvalidateDataCache(h, 0x1a0);
    GXS::LoadBGPltt((const void *)h, 0x60, 0x1a0);
    Ov004_Deallocate((void *)h);

    *(volatile u16 *)0x4000050 = 0;
    G2x::SetBlendAlpha((volatile u16 *)0x4001050, 4, 8, 6, 0xa);

    data_0209d45c |= 4;
    data_0209d454 |= 0xc;

    h = (int)func_ov004_020adc74(&data_ov006_021402a8);
    DecompressLZ16(h, (void *)0x6400000);
    DecompressLZ16(h, (void *)0x6600000);
    Ov004_Deallocate((void *)h);

    h = (int)func_ov004_020adc74(&data_ov006_021402c4);
    CP15::FlushAndInvalidateDataCache(h, 0x100);
    GX::LoadOBJPltt((const void *)h, 0, 0x100);
    GXS::LoadOBJPltt((const void *)h, 0, 0x100);
    Ov004_Deallocate((void *)h);

    data_ov004_020bc8a8 = 0x40;
    data_ov004_020bc898 = 0xa0;
    data_ov004_020bc86c = 0xc0;
    data_ov004_020bc8a4 = 0xa0;

    mWinStreak = 0;
    mLoseStreak = 0;
    mHoldTimer = 0;
    func_ov006_0212a764(raw);

    data_0209d45c |= 1;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & ~3) | 1;
    func_ov006_020c3d88(raw + 0x51f8);
    func_ov006_020c3b2c(raw + 0x4660);

    return 1;
}

