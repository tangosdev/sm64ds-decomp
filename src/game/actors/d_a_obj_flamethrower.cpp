//cpp
/* ov095/daObjFlamethrower_c -- the flame jet (OBJ_FLAMETHROWER).
 *
 * A chain of flame segments along mTransform, one dCcPos_c each.
 * Twelve segments normally; nine in Big Boo's Haunt (LEVEL_ID 0xc),
 * where each segment's height is a sine bow under mPosY. State 0
 * pulls the jet in one segment a frame and state 1 pushes it back
 * out; each phase lasts 60 frames. Every active segment burns Mario
 * and recycles a pair of flame particles. param1's low byte selects
 * the pair: not 0xff is effects 0x4f and 0x50 and the jet is re-aimed
 * every frame; 0xff is effects 0x51 and 0x52 and the jet stays where
 * InitResources put it, except in Big Boo's Haunt, which re-aims
 * anyway.
 *
 * InitResources is written above Behavior so deferred codegen emits
 * them in ROM order. The destructor stays the inline header body:
 * that is what places D1 then D0 ahead of both.
 *
 * daObjFlamethrower_c_classInit (0x02136ed4..0x02136f58, historical alias
 * Flamethrower_Spawn) sits at the very top of this file, ahead of
 * InitResources: under the default (non-off) deferred codegen this TU emits
 * .text in REVERSE of source order, so being written first is what puts it
 * LAST in .text -- ROM-ascending, after InitResources. The native factory
 * constructs the member arrays through their real class constructors. The
 * Vector3 array uses its empty default constructor at arm9 0x0203d384.
 *
 * Notes (each measured with mwccarm 2004/b56):
 * - InitResources: a Vector3* / dCcPos_c* walk was 0x16c against the
 *   ROM's 0x174. The cartridge keeps a pointer that starts at this
 *   and strides by 0xc, storing through +0x3a4, plus a second pointer
 *   already at +0x3a4 for dCcPos_c::Init. Folding the offset into the
 *   array pointer drops those 8 bytes.
 * - Behavior: mActiveFlames++ / -- encodes as ldrb [this, #0x465].
 *   The ROM read-modify-writes through a computed address, which
 *   `u8 *active = &mActiveFlames` reproduces. The segment loop is the
 *   same shape as InitResources (this-based, stores at +0x3a4 /
 *   +0x104 / +0xf8). Those two spellings together were 0x454 against
 *   0x470.
 * - InitResources: dCcPos_c::Init with by-value Fix12<int> was 0x18c
 *   against 0x174. The call is the scalar Fix12i method.
 */

#include "daObjFlamethrower_c.h"
#include "decl_common.h"
#include "daPly_c.h"
#include "Sound.h"
#include "nitro/fx/fx.h"
#include "math/CalcLib.h"
#include "Particle__System.h"
#include "nitro/fx/fx_trig.h"
#include "math/MtxStack.h"

/* LEVEL_ID 0xc is Big Boo's Haunt (its course overlay is ov020). */
enum {
    kBigBoosHaunt = 0xc,
    kSegments = 12,
    kBbhSegments = 9,
    kPhaseFrames = 0x3c,
    kAltParam = 0xff,
    kOffScreen = 8,
    kPlayerActorId = 0xbf,
    kFlameSound = 0x180,
    kClsnFlags = 0x200002,
    kParticleLift = 0x32000,
    kRadiusMul = 0x14,
    kHeightMul = 0x28
};

extern signed char gCurrentSublevel;
/* Second particle parameter per segment: 0xe61 + i * 0x689. */
extern short data_ov095_02136f98[];

extern "C" {
extern void Vec3_Add(Vector3 *out, const Vector3 *a, const Vector3 *b);
}

/* data_ov095_02136f80 (decl_common.h) is the per-segment scale,
 * 0x1000 + i * 0x745: collision radius and height are that times
 * 0x14 and 0x28, and the particle call takes it too.
 * data_ov095_02136fb0 is the local-Z placement, fx12, 0x28000 +
 * i * 0x122c8, rotated by mTransform and added to the previous
 * segment. */

/* The implicit constructor builds twelve collision cylinders and twelve
 * flame positions in member order. Vector3's empty default constructor
 * is the original array callback at arm9 0x0203d384. */
daObjFlamethrower_c *daObjFlamethrower_c_classInit() {
    return new daObjFlamethrower_c;
}

int daObjFlamethrower_c::InitResources() {
    int count;
    int i;
    int im1;
    int zero;
    int flag;
    char *rowBase;
    Vector3 *flamePos;
    dCcPos_c *collider;
    Vector3 offset;
    Vector3 dst;
    Vector3 firstPos;
    Vector3 nextPos;

    count = kSegments;
    if (gCurrentSublevel == kBigBoosHaunt)
        count = kBbhSegments;
    Matrix4x3_FromRotationXYZExt(&mTransform, mAngleX, mAngleY, mAngleZ);
    zero = 0;
    flag = 0;
    i = 0;
    if (i < count) {
        rowBase = (char *)this;
        flamePos = mFlamePositions;
        collider = mColliders;
        do {
            im1 = i - 1;
            offset.x = zero;
            offset.y = zero;
            offset.z = zero;
            dst.x = zero;
            dst.y = zero;
            dst.z = zero;
            offset.z = data_ov095_02136fb0[i];
            MTX_MultVec43(&offset, &mTransform, &dst);
            if (im1 < 0) {
                Vec3_Add(&firstPos, &mPos, &dst);
                ((daObjFlamethrower_c *)rowBase)->mFlamePositions[0].x = firstPos.x;
                ((daObjFlamethrower_c *)rowBase)->mFlamePositions[0].y = firstPos.y;
                ((daObjFlamethrower_c *)rowBase)->mFlamePositions[0].z = firstPos.z;
            } else {
                Vec3_Add(&nextPos, &mFlamePositions[im1], &dst);
                ((daObjFlamethrower_c *)rowBase)->mFlamePositions[0].x = nextPos.x;
                ((daObjFlamethrower_c *)rowBase)->mFlamePositions[0].y = nextPos.y;
                ((daObjFlamethrower_c *)rowBase)->mFlamePositions[0].z = nextPos.z;
            }
            {
                short segmentSize = data_ov095_02136f80[i];
                collider->Init(*flamePos, segmentSize * kRadiusMul, segmentSize * kHeightMul,
                               kClsnFlags, flag);
            }
            rowBase += 0xc;
            flamePos++;
            collider++;
            i++;
        } while (i < count);
    }
    return 1;
}

int daObjFlamethrower_c::Behavior() {
    char *self;
    int count;
    int i;
    int im1;
    int zero;
    int one;
    int no;
    int ang0;
    int ang1;
    char *ppos;
    int wavePhase;
    char *pclsn;
    int j;
    Vector3 in;
    Vector3 dst;
    Vector3 part;
    Vector3 sum;
    int sum2[3];
    int hidden;
    u32 id;
    dActor_c *other;
    int isPlayer;
    s16 ang;
    int a2;
    u8 *active;

    self = (char *)this;
    count = kSegments;
    if (gCurrentSublevel == kBigBoosHaunt)
        count = kBbhSegments;
    if (count <= 1)
        count = 1;
    if (count >= kSegments)
        count = kSegments;

    switch (mState) {
    case 0:
        if (mActiveFlames != 0) {
            /* Through its address. mActiveFlames-- is ldrb [this, #0x465]. */
            active = &mActiveFlames;
            *active = (*active - 1);
        }
        if (DecIfAbove0_Short(&mTimer) == 0) {
            mState = 1;
            mTimer = kPhaseFrames;
        }
        break;
    case 1:
        if ((int)mActiveFlames < (count - 1)) {
            active = &mActiveFlames;
            *active = (*active + 1);
        }
        if (DecIfAbove0_Short(&mTimer) == 0) {
            mState = 0;
            mTimer = kPhaseFrames;
        }
        break;
    }

    if (mActiveFlames != 0) {
        mSoundHandle = Sound::PlayLong(mSoundHandle, 3, kFlameSound, mCamSpacePos, 0);
    }

    if ((mFlags & kOffScreen) != 0)
        hidden = 1;
    else
        hidden = 0;
    if (hidden != 0)
        return 1;

    if (((param1 & 0xff) != kAltParam) || (gCurrentSublevel == kBigBoosHaunt))
        Matrix4x3_FromRotationXYZExt(&mTransform, mAngleX, mAngleY, mAngleZ);

    if (mActiveFlames != 0) {
        zero = 0;
        one = 1;
        no = 0;
        ang0 = 0;
        ang1 = 0;
        i = 0;
        if (i < count) {
            ppos = self;
            wavePhase = i;
            pclsn = self;
            do {
                if (i == (int)mActiveFlames)
                    break;
                if (((param1 & 0xff) != kAltParam) || (gCurrentSublevel == kBigBoosHaunt)) {
                    im1 = i - 1;
                    in.x = zero;
                    in.y = zero;
                    in.z = zero;
                    dst.x = zero;
                    dst.y = zero;
                    dst.z = zero;
                    in.z = data_ov095_02136fb0[i];
                    MTX_MultVec43(&in, &mTransform, &dst);
                    if (im1 < 0) {
                        Vec3_Add(&sum, &mPos, &dst);
                        ((daObjFlamethrower_c *)ppos)->mFlamePositions[0].x = sum.x;
                        ((daObjFlamethrower_c *)ppos)->mFlamePositions[0].y = sum.y;
                        ((daObjFlamethrower_c *)ppos)->mFlamePositions[0].z = sum.z;
                    } else {
                        Vec3_Add((Vector3 *)sum2, &mFlamePositions[im1], &dst);
                        ((daObjFlamethrower_c *)ppos)->mFlamePositions[0].x = sum2[0];
                        ((daObjFlamethrower_c *)ppos)->mFlamePositions[0].y = sum2[1];
                        ((daObjFlamethrower_c *)ppos)->mFlamePositions[0].z = sum2[2];
                    }
                    if (gCurrentSublevel == kBigBoosHaunt) {
                        s16 wave = FX_SinIdx(((u16)(s16)wavePhase) >> 4);
                        ((daObjFlamethrower_c *)ppos)->mFlamePositions[0].y =
                            mPosY + FX_MUL(wave, -0xaa000);
                    }
                    ((daObjFlamethrower_c *)pclsn)->mColliders[0].pos.x =
                        ((daObjFlamethrower_c *)ppos)->mFlamePositions[0].x;
                    ((daObjFlamethrower_c *)pclsn)->mColliders[0].pos.y =
                        ((daObjFlamethrower_c *)ppos)->mFlamePositions[0].y;
                    ((daObjFlamethrower_c *)pclsn)->mColliders[0].pos.z =
                        ((daObjFlamethrower_c *)ppos)->mFlamePositions[0].z;
                }
                id = ((daObjFlamethrower_c *)pclsn)->mColliders[0].otherOwner;
                if (id != 0) {
                    other = dActor_c::FindWithID(id);
                    if (other != 0) {
                        if (other->actorID == kPlayerActorId)
                            isPlayer = one;
                        else
                            isPlayer = no;
                        if (isPlayer != 0)
                            ((daPly_c *)other)->Burn();
                    }
                }
                part = ((daObjFlamethrower_c *)ppos)->mFlamePositions[0];
                part.y += kParticleLift;
                if ((param1 & 0xff) != kAltParam) {
                    ang = data_ov095_02136f80[i];
                    a2 = ang0;
                    Particle::System::NewFlamethrowerBlueA(part.x, part.y, part.z, ang, a2);
                    Particle::System::NewFlamethrowerBlueB(part.x, part.y, part.z, ang,
                                                           data_ov095_02136f98[i]);
                } else {
                    ang = data_ov095_02136f80[i];
                    a2 = ang1;
                    Particle::System::NewFlamethrowerRedA(part.x, part.y, part.z, ang, a2);
                    Particle::System::NewFlamethrowerRedB(part.x, part.y, part.z, ang,
                                                          data_ov095_02136f98[i]);
                }
                ppos += 0xc;
                wavePhase += 0x1000;
                pclsn += 0x3c;
                i++;
            } while (i < count);
        }
    }

    j = 0;
    if (count > 0) {
        dCcPos_c *pc = mColliders;
        do {
            pc->Clear();
            if (j < (int)mActiveFlames)
                pc->dCc_c::Update();
            j++;
            pc++;
        } while (j < count);
    }
    return 1;
}
