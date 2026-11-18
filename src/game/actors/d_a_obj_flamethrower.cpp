//cpp
/* ov095/daObjFlamethrower_c -- the flame jet (OBJ_FLAMETHROWER).
 *
 * A chain of flame segments along mTransform, one dCcPos_c each.
 * Twelve segments normally; nine in Big Boo's Haunt (LEVEL_ID 0xc),
 * where each segment's height is a sine bow under mPos.y. State 0
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
    kPhaseFrames = 60,
    kAltParam = 0xff,
    kOffScreen = 8,
    kPlayerActorId = 0xbf,
    kFlameSound = 0x180,
    kClsnFlags = 0x200002,
    kParticleLift = 0x32000,
    kRadiusMul = 20,
    kHeightMul = 40
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
    Vector3 offset;
    Vector3 dst;
    Vector3 firstPos;
    Vector3 nextPos;

    count = kSegments;
    if (gCurrentSublevel == kBigBoosHaunt)
        count = kBbhSegments;
    Matrix4x3_FromRotationXYZExt(&mTransform, mAngle.x, mAngle.y, mAngle.z);
    for (i = 0; i < count; i++) {
        im1 = i - 1;
        offset.x = 0;
        offset.y = 0;
        offset.z = 0;
        dst.x = 0;
        dst.y = 0;
        dst.z = 0;
        offset.z = data_ov095_02136fb0[i];
        MTX_MultVec43(&offset, &mTransform, &dst);
        if (im1 < 0) {
            Vec3_Add(&firstPos, &mPos, &dst);
            mFlamePositions[i] = firstPos;
        } else {
            Vec3_Add(&nextPos, &mFlamePositions[im1], &dst);
            mFlamePositions[i] = nextPos;
        }
        {
            short segmentSize = data_ov095_02136f80[i];
            mColliders[i].Init(mFlamePositions[i], segmentSize * kRadiusMul,
                               segmentSize * kHeightMul, kClsnFlags, 0);
        }
    }
    return 1;
}

int daObjFlamethrower_c::Behavior() {
    int count;
    int i;
    int im1;
    int j;
    Vector3 in;
    Vector3 dst;
    Vector3 part;
    Vector3 firstPos;
    Vector3 nextPos;
    int hidden;
    u32 id;
    dActor_c *other;
    int isPlayer;
    s16 ang;
    u8 *active;

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
        Matrix4x3_FromRotationXYZExt(&mTransform, mAngle.x, mAngle.y, mAngle.z);

    if (mActiveFlames != 0) {
        for (i = 0; i < count; i++) {
            if (i == (int)mActiveFlames)
                break;
            if (((param1 & 0xff) != kAltParam) || (gCurrentSublevel == kBigBoosHaunt)) {
                im1 = i - 1;
                in.x = 0;
                in.y = 0;
                in.z = 0;
                dst.x = 0;
                dst.y = 0;
                dst.z = 0;
                in.z = data_ov095_02136fb0[i];
                MTX_MultVec43(&in, &mTransform, &dst);
                if (im1 < 0) {
                    Vec3_Add(&firstPos, &mPos, &dst);
                    mFlamePositions[i] = firstPos;
                } else {
                    Vec3_Add(&nextPos, &mFlamePositions[im1], &dst);
                    mFlamePositions[i] = nextPos;
                }
                if (gCurrentSublevel == kBigBoosHaunt) {
                    s16 wave = FX_SinIdx(((u16)(s16)(i * 0x1000)) >> 4);
                    mFlamePositions[i].y = mPos.y + FX_MUL(wave, -0xaa000);
                }
                mColliders[i].pos = mFlamePositions[i];
            }
            id = mColliders[i].otherOwner;
            if (id != 0) {
                other = dActor_c::FindWithID(id);
                if (other != 0) {
                    if (other->actorID == kPlayerActorId)
                        isPlayer = 1;
                    else
                        isPlayer = 0;
                    if (isPlayer != 0)
                        ((daPly_c *)other)->Burn();
                }
            }
            part = mFlamePositions[i];
            part.y += kParticleLift;
            if ((param1 & 0xff) != kAltParam) {
                ang = data_ov095_02136f80[i];
                Particle::System::NewFlamethrowerBlueA(part.x, part.y, part.z, ang, 0);
                Particle::System::NewFlamethrowerBlueB(part.x, part.y, part.z, ang,
                                                       data_ov095_02136f98[i]);
            } else {
                ang = data_ov095_02136f80[i];
                Particle::System::NewFlamethrowerRedA(part.x, part.y, part.z, ang, 0);
                Particle::System::NewFlamethrowerRedB(part.x, part.y, part.z, ang,
                                                      data_ov095_02136f98[i]);
            }
        }
    }

    for (j = 0; j < count; j++) {
        mColliders[j].Clear();
        if (j < (int)mActiveFlames)
            mColliders[j].dCc_c::Update();
    }
    return 1;
}
