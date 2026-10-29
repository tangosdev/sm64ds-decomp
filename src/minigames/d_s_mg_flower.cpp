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
 *  - DealPetals' layout and shuffle loops reach the petal fields through
 *    c + i * 0x20 cast back to the scene, ((dScMgFlower_c *)p)->mPetals[0]:
 *    indexing mPetals[i] there changes the code (see its doc comment).
 *  - Behavior keeps one launder, LB, on the dragged petal's y.
 */

#include "dScMgFlower_c.h"
#include "types.h"
#include "decl_common.h"
#include "Sound.h"
#include "G2x.h"
#include "PlayerInput.h"
#include "G2.h"
#include "CP15.h"
#include "GX.h"

#pragma defer_codegen off
#pragma opt_strength_reduction off

/* The first two OAM attribute words, copied whole from the petal template. */
struct SpriteAttr {
    int a, b;
};

/* Address launder: forces an address through an integer so mwccarm
 * rebuilds it at its use instead of sharing one register; removing it
 * changes the code. */
#define LB(p) ((int)((int)(p)))

extern "C" {
extern int RandomIntInternal(int *seed);
extern void func_0203d388(int *p, int angle);
extern void Mg_SetTopScreenYOffset(int n);
extern int data_0209d4b8;
extern s16 data_02082214[];
void Mg_RenderSpriteBoth(void *a0, int a1, int a2, int a3, int a4);
void Mg_RenderSubSpriteScaled(void *a0, int a1, int a2, int a3, int a4, int a5, u16 a6);
/* Plain screen-space vectors used at the scalar C ABI boundary. */
typedef struct V2 {
    int x;
    int y;
} V2;
/* Position-only view retaining the petal stride from a +0x04 base. */
typedef struct Cell {
    int x;
    int y;
    char pad[0x18];
} Cell;
void Vec2_Sub(V2 *out, V2 *a, V2 *b);
int Vec2_Len(V2 *p);
void FreeGfxSlotsById(int a);
void *Ov004_LoadFileByPath(void *arg);
void DecompressLZ16(int src, void *dst);
void Ov004_Deallocate(void *ptr);
extern int data_0208ee44;
extern u8 data_0209d45c;
extern u8 data_0209d454;
}

/* Written first so D1 and D0 are emitted first, as in the cartridge. The
 * body is empty: the compiler destroys mFace, then the petal table, in
 * reverse of construction; the rest of the teardown is the inlined base
 * destructors. */
dScMgFlower_c::~dScMgFlower_c() {}

// @symbol _ZN13dScMgFlower_c5PetalD1Ev
/* The petal record's destructor, which __cxa_vec_cleanup runs on each of
 * mPetals. Empty, like its constructor. */
dScMgFlower_c::Petal::~Petal() {}

// @symbol _ZN13dScMgFlower_c12UpdatePetalsEv
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

// @symbol _ZN13dScMgFlower_c10DealPetalsEv
/* Round setup, called by InitResources and by StartRound when a round
 * restarts. Clears the cursor, wipes all 0x16 petal records and rolls the
 * petal count (1-2 when the hold counter reads exactly 0x14, else a 1-in-10
 * chance of 20-21, otherwise 8-15). The petals are laid out on a circle
 * (angle = base + i * full turn / count, a (0, 3.0) offset rotated by that
 * angle and moved to (8.0, 6.0)), shuffled by swapping angle and position
 * with random partners, and the round state is reset.
 *
 * The layout and shuffle loops step a petal pointer, c + i * 0x20, and
 * cast it back to the scene: &mPetals[i].angle makes the function 0xc
 * bytes longer, mPetals[i].y 4 bytes longer, and &mPetals[i].x and the
 * indexed swap reorder registers. pAngle and pX must stay block locals with
 * pX declared after the angle store, and the swap temp must be a Vec2 (its
 * empty destructor keeps its stack slot). */
void dScMgFlower_c::DealPetals() {
    dScMgFlower_c *self = this;
    char *c = (char *)self;
    unsigned int roll;
    int shuffleIndex;
    int petalIndex;
    int angle;
    int *pPos;
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
    petalIndex = 0;
    if (self->mPetalsLeft > 0) {
        pPos = &self->mPetals[0].x;
        angle = 0;
        do {
            s16 *pAngle = &((dScMgFlower_c *)(c + petalIndex * 0x20))->mPetals[0].angle;
            *pAngle = baseAngle + angle / self->mPetalsLeft;
            {
                s32 *pX = &((dScMgFlower_c *)(c + petalIndex * 0x20))->mPetals[0].x;
                *pX = 0;
                ((dScMgFlower_c *)(c + petalIndex * 0x20))->mPetals[0].y = 0x30000;
                func_0203d388(pPos, *pAngle);
                *pX += 0x80000;
                ((dScMgFlower_c *)(c + petalIndex * 0x20))->mPetals[0].y += 0x60000;
            }
            self->mPetals[petalIndex].active = 1;
            petalIndex++;
            angle += 0x10000;
            pPos += 8;
        } while (petalIndex < self->mPetalsLeft);
    }
    shuffleIndex = 0;
    if (self->mPetalsLeft > 0) {
        do {
            int swapIndex;
            roll = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            swapIndex = (self->mPetalsLeft * roll) >> 15;
            if (swapIndex != shuffleIndex) {
                char *shuffleBase = c + shuffleIndex * 0x20;
                char *swapBase = c + swapIndex * 0x20;
                u16 savedAngle;
                dScMgFlower_c::Vec2 savedPosition;
                savedAngle = *(u16 *)&((dScMgFlower_c *)shuffleBase)->mPetals[0].angle;
                *(u16 *)&((dScMgFlower_c *)shuffleBase)->mPetals[0].angle =
                    *(u16 *)&((dScMgFlower_c *)swapBase)->mPetals[0].angle;
                *(u16 *)&((dScMgFlower_c *)swapBase)->mPetals[0].angle = savedAngle;
                savedPosition.x = ((dScMgFlower_c *)shuffleBase)->mPetals[0].x;
                savedPosition.y = ((dScMgFlower_c *)shuffleBase)->mPetals[0].y;
                ((dScMgFlower_c *)shuffleBase)->mPetals[0].x =
                    ((dScMgFlower_c *)swapBase)->mPetals[0].x;
                ((dScMgFlower_c *)shuffleBase)->mPetals[0].y =
                    ((dScMgFlower_c *)swapBase)->mPetals[0].y;
                ((dScMgFlower_c *)swapBase)->mPetals[0].x = savedPosition.x;
                ((dScMgFlower_c *)swapBase)->mPetals[0].y = savedPosition.y;
            }
            shuffleIndex++;
        } while (shuffleIndex < self->mPetalsLeft);
    }
    self->mHeldPetal = -1;
    self->mPetalToggle = 0;
    self->mState = 0;
    self->mHintTimer = 0;
    self->mResultTimer = 0;
    self->mFaceSprite = 2;
    self->mPetalsSettled = 0;
    self->mBgScrollPhase = 0;
    *(volatile u16 *)0x4000050 = 0;
    G2x::SetBlendAlpha((volatile u16 *)0x4001050, 4, 8, 6, 0xa);
}

// @symbol _ZN13dScMgFlower_c10StartRoundEi
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

// @symbol _ZN13dScMgFlower_c6RenderEv
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
                                     0x1000, (u16)(s16)((u16)mPetals[i].angle + 0x8000));
        }
    }

    Camera_InitFlowerView(pad_4660);
    FlowerFace_Draw(&mFace);
    return 1;
}

// @symbol _ZN13dScMgFlower_c8BehaviorEv
/* Slot 6, the whole minigame: pick up a petal within the stylus radius, drag it,
 * drop it, flip "loves me" and "loves me not", and run the end-of-round
 * results. LB is a launder, not a cast (see above), and the pragma
 * around this function is required. */
#pragma opt_common_subs off
s32 dScMgFlower_c::Behavior() {
    struct dScMgFlower_c *self = this;
    int releaseIndex;
    int playerSlot;
    int newlyCounted;
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
            self->mHintTimer -= 1;
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
        playerSlot = gActivePlayerSlot;
        if (gTouchHeld[playerSlot * 4] != 0) {
            int touchY = gTouchY[playerSlot * 4];
            int touchX = gTouchX[playerSlot * 4];
            self->mCursorX = (touchY ? touchX : touchX) << 12;
            self->mCursorY = touchY << 12;
        }
        if (self->mHoldTimer > 0x14) {
            if (self->mPetalsLeft > 0) {
                for (releaseIndex = 0; releaseIndex < self->mPetalsLeft; releaseIndex++) {
                    self->mPetals[releaseIndex].released = 1;
                    self->mPetals[releaseIndex].counted = 1;
                }
                self->mPetalsLeft = 0;
                Sound::PlayBank2_2D(0x10d);
                FreeGfxSlotsById(0x1d);
            }
        } else if (self->mHeldPetal < 0) {
            int touchPressed;
            if (gTouchHeld[playerSlot * 4] != 0 && gTouchEdge[playerSlot * 4] != 0)
                touchPressed = 1;
            else
                touchPressed = 0;
            /* Prefer attached petals; then allow re-grabbing a falling petal. */
            if (touchPressed != 0) {
                int attachedIndex;
                V2 *petalPos = (V2 *)&self->mPetals[0].x;
                for (attachedIndex = 0; attachedIndex < 0x16; attachedIndex++) {
                    if (self->mPetals[attachedIndex].active != 0 &&
                        self->mPetals[attachedIndex].held != 1 &&
                        self->mPetals[attachedIndex].released != 1) {
                        int withinGrabRadius;
                        Vec2_Sub(&toCursor, petalPos, (V2 *)&self->mCursorX);
                        withinGrabRadius = Vec2_Len(&toCursor) < 0x18000 ? 1 : 0;
                        if (withinGrabRadius != 0) {
                            Sound::PlayBank2_2D(0x109);
                            self->mPetals[attachedIndex].held = 1;
                            self->mPetals[attachedIndex].driftX = 0;
                            self->mPetals[attachedIndex].fallSpeed = 0;
                            self->mHeldPetal = attachedIndex;
                            self->mPetals[attachedIndex].touchStartX = self->mCursorX;
                            self->mPetals[attachedIndex].touchStartY = self->mCursorY;
                            FreeGfxSlotsById(0x1d);
                            self->mFaceSprite = 2;
                            break;
                        }
                    }
                    petalPos = (V2 *)((char *)petalPos + sizeof(dScMgFlower_c::Petal));
                }
                if (self->mHeldPetal < 0) {
                    int freeIndex;
                    V2 *petalPos2 = (V2 *)&self->mPetals[0].x;
                    for (freeIndex = 0; freeIndex < 0x16; freeIndex++) {
                        if (self->mPetals[freeIndex].active != 0 &&
                            self->mPetals[freeIndex].held != 1) {
                            int withinGrabRadius2;
                            Vec2_Sub(&toCursor2, petalPos2, (V2 *)&self->mCursorX);
                            withinGrabRadius2 = Vec2_Len(&toCursor2) < 0x18000 ? 1 : 0;
                            if (withinGrabRadius2 != 0) {
                                self->mPetals[freeIndex].held = 1;
                                self->mPetals[freeIndex].driftX = 0;
                                self->mPetals[freeIndex].fallSpeed = 0;
                                self->mHeldPetal = freeIndex;
                                self->mPetals[freeIndex].touchStartX = self->mCursorX;
                                self->mPetals[freeIndex].touchStartY = self->mCursorY;
                                break;
                            }
                        }
                        petalPos2 = (V2 *)((char *)petalPos2 + sizeof(dScMgFlower_c::Petal));
                    }
                }
            }
        } else {
            newlyCounted = 0;
            if (gTouchHeld[playerSlot * 4] != 0) {
                Cell *positions;
                int heldIndex;
                Vec2_Sub(&drag, (V2 *)&self->mCursorX, (V2 *)&self->mPrevCursorX);
                positions = (Cell *)&self->mPetals[0].x;
                heldIndex = self->mHeldPetal;
                positions[heldIndex].x += drag.x;
                *(int *)LB(&positions[heldIndex].y) += drag.y;
            } else {
                if (self->mPetals[self->mHeldPetal].released == 0) {
                    self->mPetals[self->mHeldPetal].released = 1;
                    if (self->mPetals[self->mHeldPetal].counted == 0) {
                        newlyCounted = 1;
                        self->mPetals[self->mHeldPetal].counted = 1;
                        FlowerFace_PlayPetalPulled((char *)&self->mFace);
                    }
                }
                self->mPetals[self->mHeldPetal].held = 0;
                self->mHeldPetal = -1;
            }
            if (newlyCounted != 0) {
                self->mPetalsLeft -= 1;
                if (self->mPetalToggle == 1) {
                    self->mPetalToggle = 0;
                    if (self->mPetalsLeft >= 1) {
                        FreeGfxSlotsById(0x1d);
                        Mg_ShowBanner(0x13, 0x80, 0x18, 0, -1, 0xd);
                        self->mFaceSprite = 3;
                        Sound::PlayBank2_2D(0x104);
                    }
                } else {
                    self->mPetalToggle = 1;
                    if (self->mPetalsLeft >= 1) {
                        FreeGfxSlotsById(0x1d);
                        Mg_ShowBanner(0x10, 0x80, 0x18, 0, -1, 0xd);
                        self->mFaceSprite = 1;
                        Sound::PlayBank2_2D(0x103);
                    }
                }
                self->mHintTimer = 0x3c;
            }
        }
        if (self->mPetalsLeft <= 0 && self->mPetalsSettled == 1) {
            if (self->mResultTimer > 0) {
                self->mResultTimer -= 1;
                if (self->mResultTimer <= 0) {
                    self->mState = 1;
                    Mg_SetSharedState(MG_STATE_RETRY_PROMPT_WIDE);
                }
                if (FlowerFace_IsIdle((char *)&self->mFace) != 0) {
                    if (self->mPetalToggle == 1)
                        FlowerFace_PlayLike((char *)&self->mFace);
                    else
                        FlowerFace_PlayHate((char *)&self->mFace);
                }
            } else {
                FreeGfxSlotsById(0x1d);
                if (self->mHoldTimer > 0x14) {
                    if (self->mPetalsSettled == 1) {
                        Sound::PlayBank2_2D(0x106);
                        self->mFaceSprite = 4;
                        self->mResultTimer = 0x3c;
                    }
                } else {
                    if (self->mPetalToggle == 1) {
                        self->mFaceSprite = 0;
                        self->mWinStreak += 1;
                        self->mLoseStreak = 0;
                        if (self->mWinStreak >= 3) {
                            Mg_ShowBanner(0x12, 0x80, 0x18, 0, -1, 0xd);
                            Sound::PlayBank2_2D(0x107);
                            self->mScore += 3;
                            if (self->mScore > 0x270f)
                                self->mScore = 0x270f;
                        } else {
                            Mg_ShowBanner(0x10, 0x80, 0x18, 0, -1, 0xd);
                            Sound::PlayBank2_2D(0x108);
                            self->mScore += 1;
                            if (self->mScore > 0x270f)
                                self->mScore = 0x270f;
                        }
                    } else {
                        self->mFaceSprite = 4;
                        self->mLoseStreak += 1;
                        self->mWinStreak = 0;
                        if (self->mLoseStreak >= 3) {
                            Mg_ShowBanner(0x11, 0x80, 0x18, 0, -1, 0xd);
                            Sound::PlayBank2_2D(0x105);
                        } else {
                            Mg_ShowBanner(0x13, 0x80, 0x18, 0, -1, 0xd);
                            Sound::PlayBank2_2D(0x106);
                        }
                    }
                    self->mResultTimer = 0x3c;
                }
            }
        }
        UpdatePetals();
        break;
    case 1:
        if (FlowerFace_IsIdle((char *)&self->mFace) != 0) {
            if (self->mPetalToggle == 1)
                FlowerFace_PlayLike((char *)&self->mFace);
            else
                FlowerFace_PlayHate((char *)&self->mFace);
        }
        self->mPromptEnabled = 0;
        UpdatePetals();
        break;
    default:
        break;
    }
    FlowerFace_Update((char *)&self->mFace);
    return 1;
}
#pragma opt_common_subs on

// @symbol _ZN13dScMgFlower_c13InitResourcesEv
/* Slot 0. Sets up BG2 and BG3 on both screens, decompresses their tiles and
 * maps, loads the palettes and the OBJ tiles, then starts the first round.
 * The bare 0x040000xx stores are the BGxCNT and BGxOFS registers, and
 * 0x06400000 and 0x06600000 are the two OBJ VRAM banks. */
s32 dScMgFlower_c::InitResources() {
    int fileData;

    mScore = Mg_GetSavedRecord0();

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

    fileData = (int)Ov004_LoadFileByPath(&sFlowerBgUpNcgPath);
    DecompressLZ16(fileData, G2::GetBG2CharPtr());
    Ov004_Deallocate((void *)fileData);

    fileData = (int)Ov004_LoadFileByPath(&sFlowerBgDownNcgPath);
    DecompressLZ16(fileData, G2S::GetBG2CharPtr());
    Ov004_Deallocate((void *)fileData);

    fileData = (int)Ov004_LoadFileByPath(&sFlowerBgUpNscPath);
    DecompressLZ16(fileData, G2::GetBG2ScrPtr());
    Ov004_Deallocate((void *)fileData);

    fileData = (int)Ov004_LoadFileByPath(&sFlowerBgDownNscPath);
    DecompressLZ16(fileData, G2S::GetBG2ScrPtr());
    DecompressLZ16(fileData, G2S::GetBG3ScrPtr());
    Ov004_Deallocate((void *)fileData);

    fileData = (int)Ov004_LoadFileByPath(&sFlowerBgUpNclPath);
    CP15::FlushAndInvalidateDataCache(fileData, 0x1a0);
    GX::LoadBGPltt((const void *)fileData, 0x60, 0x1a0);
    Ov004_Deallocate((void *)fileData);

    fileData = (int)Ov004_LoadFileByPath(&sFlowerBgDownNclPath);
    CP15::FlushAndInvalidateDataCache(fileData, 0x1a0);
    GXS::LoadBGPltt((const void *)fileData, 0x60, 0x1a0);
    Ov004_Deallocate((void *)fileData);

    *(volatile u16 *)0x4000050 = 0;
    G2x::SetBlendAlpha((volatile u16 *)0x4001050, 4, 8, 6, 0xa);

    data_0209d45c |= 4;
    data_0209d454 |= 0xc;

    fileData = (int)Ov004_LoadFileByPath(&sFlowerNcgPath);
    DecompressLZ16(fileData, (void *)0x6400000);
    DecompressLZ16(fileData, (void *)0x6600000);
    Ov004_Deallocate((void *)fileData);

    fileData = (int)Ov004_LoadFileByPath(&sFlowerNclPath);
    CP15::FlushAndInvalidateDataCache(fileData, 0x100);
    GX::LoadOBJPltt((const void *)fileData, 0, 0x100);
    GXS::LoadOBJPltt((const void *)fileData, 0, 0x100);
    Ov004_Deallocate((void *)fileData);

    sMgPlayAgainX = 0x40;
    sMgPlayAgainY = 0xa0;
    sMgQuitX = 0xc0;
    sMgQuitY = 0xa0;

    mWinStreak = 0;
    mLoseStreak = 0;
    mHoldTimer = 0;
    DealPetals();

    data_0209d45c |= 1;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & ~3) | 1;
    FlowerFace_Load((char *)&mFace);
    Camera_InitFlowerView(pad_4660);

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

// @symbol dScMgFlower_c_classInit
/* Reconstructed source-style name: SM64DS proves dScMgFlower_c through RTTI,
 * allocation size, vtable identity, and the MG_FLOWER registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: func_ov006_0212b7f8. */
extern "C" void *dScMgFlower_c_classInit() {
    return new dScMgFlower_c;
}

// @symbol _ZN13dScMgFlower_c5PetalC1Ev
/* The petal record's constructor, which __cxa_vec_ctor runs on each of
 * mPetals. Empty, like its destructor. */
dScMgFlower_c::Petal::Petal() {}
