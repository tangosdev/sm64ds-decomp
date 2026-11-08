//cpp
/* The flower-petal minigame scene ("loves me, loves me not"): the player drags
 * petals off a flower with the stylus while a face watches. 11 functions
 * (.text 0x0212a554..0x0212b890, to the end of ov006's code), closing with
 * the factory and the petal record's empty constructor.
 *
 * The pragmas carry the codegen; among them:
 *   - `#pragma defer_codegen off`: functions are emitted in source order and
 *     the destructor variants land where they are written. Do not reorder.
 *   - `#pragma opt_common_subs off` around Behavior only: with it on, the
 *     touch-sample loads get merged; turning it off file-wide breaks two
 *     other functions instead.
 * The destructor is defined out of line so D1 and D0 come out in cartridge
 * order; the extra D2 is dropped at link. The factory's `new` expands the
 * inline constructor, so it also emits dScMgSingle3DBase_c's D2, likewise
 * dropped.
 *
 * The 0x16-entry petal table at 0x4f38 is an array of the Petal record from
 * dScMgFlower_c.h, and mFace is a FlowerFace member: the compiler builds and
 * tears both down. Petal's empty constructor and destructor are out of line
 * so each lands at its own cartridge address; their extra C2/D2 are dropped
 * at link.
 *
 * comment leftovers:
 *  - The FlowerFace_* helpers are free functions over a char* record in
 *    src/minigames/unit020bfec0.cpp, so mFace is passed by address.
 *  - Behavior keeps one launder: the touch sample's `touchY ? touchX : touchX`,
 *    which loads both bytes before the first cursor store.
 */

#include "dScMgFlower_c.h"
#include "nitro/hw/registers.h"
#include "types.h"
#include "decl_common.h"
#include "LevelRng.h"
#include "Sound.h"
#include "G2x.h"
#include "PlayerInput.h"
#include "Vector2.h"
#include "Vector2Math.h"
#include "G2.h"
#include "CP15.h"
#include "GX.h"

#pragma defer_codegen off

/* The first two OAM attribute words, copied whole from the petal template. */
struct SpriteAttr {
    int a, b;
};

extern "C" {
extern int RandomIntInternal(int *seed);
extern void func_0203d388(int *p, int angle);
extern void Mg_SetTopScreenYOffset(int n);
extern s16 data_02082214[];
void Mg_RenderSpriteBoth(void *a0, int a1, int a2, int a3, int a4);
int Mg_RenderSubSpriteScaled(void *a0, int a1, int a2, int a3, int a4, int a5, u16 a6);
/* The dragged petal's two adjacent fixed-point coordinates. */
typedef struct V2 {
    int x;
    int y;
} V2;
void Vec2_Sub(int *out, int *a, int *b);
void FreeGfxSlotsById(int a);
void *Ov004_LoadFileByPath(void *arg);
void DecompressLZ16(void *src, void *dst);
void Ov004_Deallocate(void *ptr);
extern int data_0208ee44;
extern u8 gMainScreenLayers;
extern u8 gSubScreenLayers;
}

/* Written first so D1 and D0 are emitted first, as in the cartridge. The
 * body is empty: the compiler destroys mFace, then the petal table, in
 * reverse of construction; the rest of the teardown is the inlined base
 * destructors. */
dScMgFlower_c::~dScMgFlower_c() {}

/* The petal record's destructor, which __cxa_vec_cleanup runs on each of
 * mPetals. Empty, like its constructor. */
dScMgFlower_c::Petal::~Petal() {}

/* Per-frame petal update: released petals drift and fall, speeding up to
 * 0x4000, and go inactive once below the screen. mPetalsSettled stays 1 only if
 * no petal is still on the flower, held, or falling. */
void dScMgFlower_c::UpdatePetals() {
    dScMgFlower_c *self = this;
    int i;

    self->mPetalsSettled = 1;
    for (i = 0; i < 22; i++) {
        if (self->mPetals[i].active == 0)
            continue;
        if (self->mPetals[i].held == 1) {
            self->mPetalsSettled = 0;
            continue;
        }
        if (self->mPetals[i].released == 0) {
            self->mPetalsSettled = 0;
            continue;
        }
        if (i == self->mHeldPetal)
            continue;
        self->mPetals[i].fallSpeed += 0x100;
        if (self->mPetals[i].fallSpeed >= 0x4000)
            self->mPetals[i].fallSpeed = 0x4000;
        self->mPetals[i].x += self->mPetals[i].driftX;
        self->mPetals[i].y += self->mPetals[i].fallSpeed;
        if (self->mPetals[i].y >= 0x100000)
            self->mPetals[i].active = 0;
        else
            self->mPetalsSettled = 0;
    }
}

/* Round setup, called by InitResources and by StartRound when a round
 * restarts. Clears the cursor, wipes all 0x16 petal records and rolls the
 * petal count (1-2 when the hold counter reads exactly 0x14, else a 1-in-10
 * chance of 20-21, otherwise 8-15). The petals are laid out on a circle
 * (angle = base + i * full turn / count, a (0, 3.0) offset rotated by that
 * angle and moved to (8.0, 6.0)), shuffled by swapping angle and position
 * with random partners, and the round state is reset. The rotation takes
 * the angle signed; the swap moves it as the stored u16. */
void dScMgFlower_c::DealPetals() {
    dScMgFlower_c *self = this;
    unsigned int roll;
    int shuffleIndex;
    int petalIndex;
    int clearIndex;
    int baseAngle;

    self->mCursorX = 0;
    self->mCursorY = 0;
    self->mPrevCursorX = self->mCursorX;
    self->mPrevCursorY = self->mCursorY;
    Mg_SetTopScreenYOffset(0x20);
    for (clearIndex = 0; clearIndex < 0x16; clearIndex++) {
        self->mPetals[clearIndex].active = 0;
        self->mPetals[clearIndex].held = 0;
        self->mPetals[clearIndex].released = 0;
        self->mPetals[clearIndex].counted = 0;
        self->mPetals[clearIndex].x = 0;
        self->mPetals[clearIndex].y = 0;
        self->mPetals[clearIndex].touchStartX = 0;
        self->mPetals[clearIndex].touchStartY = 0;
        self->mPetals[clearIndex].driftX = 0;
        self->mPetals[clearIndex].fallSpeed = 0;
    }
    if (self->mHoldTimer == 0x14) {
        roll = ((unsigned int)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff;
        self->mPetalsLeft = ((roll * 2) >> 15) + 1;
    } else {
        int raw = RandomIntInternal(&data_0209d4b8.value);
        roll = ((unsigned int)raw >> 16) & 0x7fff;
        if (((roll * 10) >> 15) == 0) {
            roll = ((unsigned int)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff;
            self->mPetalsLeft = ((roll * 2) >> 15) + 0x14;
        } else {
            roll = ((unsigned int)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff;
            self->mPetalsLeft = ((roll * 8) >> 15) + 8;
        }
    }
    roll = ((unsigned int)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff;
    baseAngle = (roll * 0x10000) >> 15;
    for (petalIndex = 0; petalIndex < self->mPetalsLeft; petalIndex++) {
        self->mPetals[petalIndex].angle = baseAngle + petalIndex * 0x10000 / self->mPetalsLeft;
        self->mPetals[petalIndex].x = 0;
        self->mPetals[petalIndex].y = 0x30000;
        func_0203d388(&self->mPetals[petalIndex].x, (s16)self->mPetals[petalIndex].angle);
        self->mPetals[petalIndex].x += 0x80000;
        self->mPetals[petalIndex].y += 0x60000;
        self->mPetals[petalIndex].active = 1;
    }
    for (shuffleIndex = 0; shuffleIndex < self->mPetalsLeft; shuffleIndex++) {
        int swapIndex;
        roll = ((unsigned int)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff;
        swapIndex = (self->mPetalsLeft * roll) >> 15;
        if (swapIndex != shuffleIndex) {
            u16 savedAngle;
            dScMgFlower_c::Vec2 savedPosition;
            savedAngle = self->mPetals[shuffleIndex].angle;
            self->mPetals[shuffleIndex].angle = self->mPetals[swapIndex].angle;
            self->mPetals[swapIndex].angle = savedAngle;
            savedPosition.x = self->mPetals[shuffleIndex].x;
            savedPosition.y = self->mPetals[shuffleIndex].y;
            self->mPetals[shuffleIndex].x = self->mPetals[swapIndex].x;
            self->mPetals[shuffleIndex].y = self->mPetals[swapIndex].y;
            self->mPetals[swapIndex].x = savedPosition.x;
            self->mPetals[swapIndex].y = savedPosition.y;
        }
    }
    self->mHeldPetal = -1;
    self->mPetalToggle = 0;
    self->mState = 0;
    self->mHintTimer = 0;
    self->mResultTimer = 0;
    self->mFaceSprite = 2;
    self->mPetalsSettled = 0;
    self->mBgScrollPhase = 0;
    REG_BLDCNT = 0;
    G2x::SetBlendAlpha(&REG_DB_BLDCNT, 4, 8, 6, 0xa);
}

/* Slot 18: bump mHoldTimer (reset once it passes 0x14), reset the face
 * object mFace and restart the round. */
void dScMgFlower_c::StartRound(int /* arg */) {
    if (mHoldTimer <= 0x14) {
        mHoldTimer++;
    } else {
        mHoldTimer = 0;
    }
    FlowerFace_Reset(&mFace);
    DealPetals();
}

/* Slot 9. Scrolls the two sub-screen background layers off the sine table
 * (mBgScrollPhase steps 0xc0 a frame), draws the face, then one sprite per
 * live petal; the held petal gets a different palette. */
s32 dScMgFlower_c::Render() {
    int i;

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
    Mg_RenderSpriteBoth(sFlowerFaceSprites[mFaceSprite], 0x80, 0x60, -1, 1);

    for (i = 0; i < 0x16; i++) {
        if (mPetals[i].active != 0) {
            struct SpriteAttr attr = *(struct SpriteAttr *)data_ov006_0213abe0;
            int palette = 1;
            attr.a = (attr.a & 0xc1fffcff) | 0x100;
            if (mHeldPetal == i)
                palette = 0;
            Mg_RenderSubSpriteScaled(&attr, mPetals[i].x >> 12, mPetals[i].y >> 12, -1, palette,
                                     0x1000, (s16)(mPetals[i].angle + 0x8000));
        }
    }

    Camera_InitFlowerView(&mCamera);
    FlowerFace_Draw(&mFace);
    return 1;
}

/* Slot 6, the whole minigame: pick up a petal within the stylus radius, drag it,
 * drop it, flip "loves me" and "loves me not", and run the end-of-round
 * results. */
/* Kept: the ROM re-reads gTouchHeld[playerSlot * 4] before the grab test; CSE reuses the first read. */
#pragma opt_common_subs off
s32 dScMgFlower_c::Behavior() {
    int releaseIndex;
    int playerSlot;
    int newlyCounted;
    Vector2 toCursor;
    Vector2 toCursor2;
    Vector2 drag;

    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }
    if (mHintTimer > 0) {
        if (mHeldPetal == -1) {
            mHintTimer -= 1;
            if (mHintTimer <= 0) {
                if (mPetalsLeft >= 1)
                    mFaceSprite = 2;
            }
        } else {
            mHintTimer = 0x3c;
        }
    }
    switch (mState) {
    case 0:
        mPrevCursorX = mCursorX;
        mPrevCursorY = mCursorY;
        playerSlot = gActivePlayerSlot;
        if (gTouchHeld[playerSlot * 4] != 0) {
            int touchY = gTouchY[playerSlot * 4];
            int touchX = gTouchX[playerSlot * 4];
            mCursorX = (touchY ? touchX : touchX) << 12;
            mCursorY = touchY << 12;
        }
        if (mHoldTimer > 0x14) {
            if (mPetalsLeft > 0) {
                for (releaseIndex = 0; releaseIndex < mPetalsLeft; releaseIndex++) {
                    mPetals[releaseIndex].released = 1;
                    mPetals[releaseIndex].counted = 1;
                }
                mPetalsLeft = 0;
                Sound::PlayBank2_2D(0x10d);
                FreeGfxSlotsById(0x1d);
            }
        } else if (mHeldPetal < 0) {
            int touchPressed;
            if (gTouchHeld[playerSlot * 4] != 0 && gTouchEdge[playerSlot * 4] != 0)
                touchPressed = 1;
            else
                touchPressed = 0;
            /* Prefer attached petals; then allow re-grabbing a falling petal. */
            if (touchPressed != 0) {
                int attachedIndex;
                for (attachedIndex = 0; attachedIndex < 0x16; attachedIndex++) {
                    if (mPetals[attachedIndex].active != 0 && mPetals[attachedIndex].held != 1 &&
                        mPetals[attachedIndex].released != 1) {
                        int withinGrabRadius;
                        Vec2_Sub(&toCursor.x, &mPetals[attachedIndex].x, &mCursorX);
                        withinGrabRadius = Vec2_Len(&toCursor) < 0x18000 ? 1 : 0;
                        if (withinGrabRadius != 0) {
                            Sound::PlayBank2_2D(0x109);
                            mPetals[attachedIndex].held = 1;
                            mPetals[attachedIndex].driftX = 0;
                            mPetals[attachedIndex].fallSpeed = 0;
                            mHeldPetal = attachedIndex;
                            mPetals[attachedIndex].touchStartX = mCursorX;
                            mPetals[attachedIndex].touchStartY = mCursorY;
                            FreeGfxSlotsById(0x1d);
                            mFaceSprite = 2;
                            break;
                        }
                    }
                }
                if (mHeldPetal < 0) {
                    int freeIndex;
                    for (freeIndex = 0; freeIndex < 0x16; freeIndex++) {
                        if (mPetals[freeIndex].active != 0 && mPetals[freeIndex].held != 1) {
                            int withinGrabRadius2;
                            Vec2_Sub(&toCursor2.x, &mPetals[freeIndex].x, &mCursorX);
                            withinGrabRadius2 = Vec2_Len(&toCursor2) < 0x18000 ? 1 : 0;
                            if (withinGrabRadius2 != 0) {
                                mPetals[freeIndex].held = 1;
                                mPetals[freeIndex].driftX = 0;
                                mPetals[freeIndex].fallSpeed = 0;
                                mHeldPetal = freeIndex;
                                mPetals[freeIndex].touchStartX = mCursorX;
                                mPetals[freeIndex].touchStartY = mCursorY;
                                break;
                            }
                        }
                    }
                }
            }
        } else {
            newlyCounted = 0;
            if (gTouchHeld[playerSlot * 4] != 0) {
                Vec2_Sub(&drag.x, &mCursorX, &mPrevCursorX);
                V2 *pos = (V2 *)&mPetals[mHeldPetal].x;
                pos->x += drag.x;
                pos->y += drag.y;
            } else {
                if (mPetals[mHeldPetal].released == 0) {
                    mPetals[mHeldPetal].released = 1;
                    if (mPetals[mHeldPetal].counted == 0) {
                        newlyCounted = 1;
                        mPetals[mHeldPetal].counted = 1;
                        FlowerFace_PlayPetalPulled((char *)&mFace);
                    }
                }
                mPetals[mHeldPetal].held = 0;
                mHeldPetal = -1;
            }
            if (newlyCounted != 0) {
                mPetalsLeft -= 1;
                if (mPetalToggle == 1) {
                    mPetalToggle = 0;
                    if (mPetalsLeft >= 1) {
                        FreeGfxSlotsById(0x1d);
                        Mg_ShowBanner(0x13, 0x80, 0x18, 0, -1, 0xd);
                        mFaceSprite = 3;
                        Sound::PlayBank2_2D(0x104);
                    }
                } else {
                    mPetalToggle = 1;
                    if (mPetalsLeft >= 1) {
                        FreeGfxSlotsById(0x1d);
                        Mg_ShowBanner(0x10, 0x80, 0x18, 0, -1, 0xd);
                        mFaceSprite = 1;
                        Sound::PlayBank2_2D(0x103);
                    }
                }
                mHintTimer = 0x3c;
            }
        }
        if (mPetalsLeft <= 0 && mPetalsSettled == 1) {
            if (mResultTimer > 0) {
                mResultTimer -= 1;
                if (mResultTimer <= 0) {
                    mState = 1;
                    Mg_SetSharedState(MG_STATE_RETRY_PROMPT_WIDE);
                }
                if (FlowerFace_IsIdle((char *)&mFace) != 0) {
                    if (mPetalToggle == 1)
                        FlowerFace_PlayLike((char *)&mFace);
                    else
                        FlowerFace_PlayHate((char *)&mFace);
                }
            } else {
                FreeGfxSlotsById(0x1d);
                if (mHoldTimer > 0x14) {
                    if (mPetalsSettled == 1) {
                        Sound::PlayBank2_2D(0x106);
                        mFaceSprite = 4;
                        mResultTimer = 0x3c;
                    }
                } else {
                    if (mPetalToggle == 1) {
                        mFaceSprite = 0;
                        mWinStreak += 1;
                        mLoseStreak = 0;
                        if (mWinStreak >= 3) {
                            Mg_ShowBanner(0x12, 0x80, 0x18, 0, -1, 0xd);
                            Sound::PlayBank2_2D(0x107);
                            mScore += 3;
                            if (mScore > 0x270f)
                                mScore = 0x270f;
                        } else {
                            Mg_ShowBanner(0x10, 0x80, 0x18, 0, -1, 0xd);
                            Sound::PlayBank2_2D(0x108);
                            mScore += 1;
                            if (mScore > 0x270f)
                                mScore = 0x270f;
                        }
                    } else {
                        mFaceSprite = 4;
                        mLoseStreak += 1;
                        mWinStreak = 0;
                        if (mLoseStreak >= 3) {
                            Mg_ShowBanner(0x11, 0x80, 0x18, 0, -1, 0xd);
                            Sound::PlayBank2_2D(0x105);
                        } else {
                            Mg_ShowBanner(0x13, 0x80, 0x18, 0, -1, 0xd);
                            Sound::PlayBank2_2D(0x106);
                        }
                    }
                    mResultTimer = 0x3c;
                }
            }
        }
        UpdatePetals();
        break;
    case 1:
        if (FlowerFace_IsIdle((char *)&mFace) != 0) {
            if (mPetalToggle == 1)
                FlowerFace_PlayLike((char *)&mFace);
            else
                FlowerFace_PlayHate((char *)&mFace);
        }
        mPromptEnabled = 0;
        UpdatePetals();
        break;
    default:
        break;
    }
    FlowerFace_Update((char *)&mFace);
    return 1;
}
#pragma opt_common_subs on

/* Slot 0. Sets up BG2 and BG3 on both screens, decompresses their tiles and
 * maps, loads the palettes and the OBJ tiles, then starts the first round.
 * 0x06400000 and 0x06600000 are the two OBJ VRAM banks. */
s32 dScMgFlower_c::InitResources() {
    void *fileData;

    mScore = Mg_GetSavedRecord0();

    data_0208ee44 = 1;

    REG_BG2CNT = (REG_BG2CNT & 0x43) | 0x1414;
    REG_BG2CNT &= ~BG_CONTROL_MOSAIC;
    *(volatile u32 *)0x4000018 = 0; /* BG2OFS */
    REG_BG2CNT = (REG_BG2CNT & ~3) | 3;

    REG_DB_BG2CNT = (REG_DB_BG2CNT & 0x43) | 0x1414;
    REG_DB_BG2CNT &= ~BG_CONTROL_MOSAIC;
    *(volatile u32 *)0x4001018 = 0; /* DB_BG2OFS */
    REG_DB_BG2CNT = (REG_DB_BG2CNT & ~3) | 3;

    REG_DB_BG3CNT = (REG_DB_BG3CNT & 0x43) | 0x1414;
    REG_DB_BG3CNT &= ~BG_CONTROL_MOSAIC;
    *(volatile u32 *)0x400101c = 0; /* DB_BG3OFS */
    REG_DB_BG3CNT = (REG_DB_BG3CNT & ~3) | 3;

    fileData = Ov004_LoadFileByPath(&sFlowerBgUpNcgPath);
    DecompressLZ16(fileData, G2::GetBG2CharPtr());
    Ov004_Deallocate(fileData);

    fileData = Ov004_LoadFileByPath(&sFlowerBgDownNcgPath);
    DecompressLZ16(fileData, G2S::GetBG2CharPtr());
    Ov004_Deallocate(fileData);

    fileData = Ov004_LoadFileByPath(&sFlowerBgUpNscPath);
    DecompressLZ16(fileData, G2::GetBG2ScrPtr());
    Ov004_Deallocate(fileData);

    fileData = Ov004_LoadFileByPath(&sFlowerBgDownNscPath);
    DecompressLZ16(fileData, G2S::GetBG2ScrPtr());
    DecompressLZ16(fileData, G2S::GetBG3ScrPtr());
    Ov004_Deallocate(fileData);

    fileData = Ov004_LoadFileByPath(&sFlowerBgUpNclPath);
    CP15::FlushAndInvalidateDataCache((u32)fileData, 0x1a0);
    GX::LoadBGPltt(fileData, 0x60, 0x1a0);
    Ov004_Deallocate(fileData);

    fileData = Ov004_LoadFileByPath(&sFlowerBgDownNclPath);
    CP15::FlushAndInvalidateDataCache((u32)fileData, 0x1a0);
    GXS::LoadBGPltt(fileData, 0x60, 0x1a0);
    Ov004_Deallocate(fileData);

    REG_BLDCNT = 0;
    G2x::SetBlendAlpha(&REG_DB_BLDCNT, 4, 8, 6, 0xa);

    gMainScreenLayers |= DISPLAY_LAYER_BG2;
    gSubScreenLayers |= 0xc;

    fileData = Ov004_LoadFileByPath(&sFlowerNcgPath);
    DecompressLZ16(fileData, (void *)0x6400000);
    DecompressLZ16(fileData, (void *)0x6600000);
    Ov004_Deallocate(fileData);

    fileData = Ov004_LoadFileByPath(&sFlowerNclPath);
    CP15::FlushAndInvalidateDataCache((u32)fileData, 0x100);
    GX::LoadOBJPltt(fileData, 0, 0x100);
    GXS::LoadOBJPltt(fileData, 0, 0x100);
    Ov004_Deallocate(fileData);

    sMgPlayAgainX = 0x40;
    sMgPlayAgainY = 0xa0;
    sMgQuitX = 0xc0;
    sMgQuitY = 0xa0;

    mWinStreak = 0;
    mLoseStreak = 0;
    mHoldTimer = 0;
    DealPetals();

    gMainScreenLayers |= DISPLAY_LAYER_BG0;
    REG_BG0CNT = (REG_BG0CNT & ~3) | 1;
    FlowerFace_Load((char *)&mFace);
    Camera_InitFlowerView(&mCamera);

    return 1;
}

/* The factory unit: the factory and the empty constructor of the petal
 * records, which follows it in the ROM. */
/* The body is empty: the compiler builds the petal table (through
 * __cxa_vec_ctor with Petal's own constructor and destructor), then mFace;
 * ~dScMgFlower_c() above tears them down in the reverse order. Everything
 * before them (dScMgBase_c's C2, dScMgSingle3DBase_c's vptr and mSysTracker,
 * then this class's own vptr) is the compiler's inlining of the base
 * constructors. */
inline dScMgFlower_c::dScMgFlower_c() {}

/* Reconstructed source-style name: SM64DS proves dScMgFlower_c through RTTI,
 * allocation size, vtable identity, and the MG_FLOWER registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: func_ov006_0212b7f8. */
extern "C" void *dScMgFlower_c_classInit() {
    return new dScMgFlower_c;
}

/* The petal record's constructor, which __cxa_vec_ctor runs on each of
 * mPetals. Empty, like its destructor. */
dScMgFlower_c::Petal::Petal() {}
