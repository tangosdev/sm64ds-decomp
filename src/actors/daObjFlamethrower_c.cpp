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
 * deslop leftovers:
 * - InitResources: a Vector3* / dCcPos_c* walk was 0x16c against the
 *   ROM's 0x174. The cartridge keeps a pointer that starts at this
 *   and strides by 0xc, storing through +0x3a4, plus a second pointer
 *   already at +0x3a4 for dCcPos_c::Init. Folding the offset into the
 *   array pointer drops those 8 bytes.
 * - Behavior: mActiveFlames++ / -- encodes as ldrb [this, #0x465].
 *   The ROM read-modify-writes through a pooled address,
 *   ((char *)this + 0x400) + 0x65. The segment loop is the same
 *   shape as InitResources (this-based, stores at +0x3a4 / +0x104 /
 *   +0xf8). Those two spellings together were 0x454 against 0x470.
 * - InitResources: dCcPos_c::Init as the header method, Fix12<int>
 *   passed by value, was 0x18c against 0x174. The call stays the
 *   mangled int-parameter extern.
 */

#include "daObjFlamethrower_c.h"
#include "decl_common.h"
#include "Player.h"
#include "Sound.h"

/* Three ints, so the transform helpers can take their address without
 * calling ~Vector3. */
struct Vec3 { int x, y, z; };

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

extern "C" {
extern signed char data_0209f2f8;
extern short data_02082214[];
/* Second particle parameter per segment: 0xe61 + i * 0x689. */
extern short data_ov095_02136f98[];
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void MulVec3Mat4x3(int *v, void *m, int *dst);
extern void Vec3_Add(int *out, int *a, int *b);
/* Flame particles. 0x4f / 0x50 when param1's low byte is not 0xff;
 * 0x51 / 0x52 when it is. No recovered names. */
extern void func_02022774(Fix12i x, Fix12i y, Fix12i z, s16 ang1, int ang2);
extern void func_020226fc(Fix12i x, Fix12i y, Fix12i z, s16 ang1, s16 ang2);
extern void func_02022864(Fix12i x, Fix12i y, Fix12i z, s16 ang1, int ang2);
extern void func_020227ec(Fix12i x, Fix12i y, Fix12i z, s16 ang1, s16 ang2);
extern void _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(
    void *self, struct Vec3 *pos, int radius, int height,
    unsigned int flags, unsigned int vuln);
}

/* data_ov095_02136f80 (decl_common.h) is the per-segment scale,
 * 0x1000 + i * 0x745: collision radius and height are that times
 * 0x14 and 0x28, and the particle call takes it too.
 * data_ov095_02136fb0 is the local-Z placement, fx12, 0x28000 +
 * i * 0x122c8, rotated by mTransform and added to the previous
 * segment. */

// @symbol _ZN19daObjFlamethrower_c13InitResourcesEv
int daObjFlamethrower_c::InitResources()
{
    int count;
    int i;
    int im1;
    int zero;
    int flag;
    char *rowBase;
    char *flamePos;
    char *collider;
    struct Vec3 offset;
    struct Vec3 dst;
    struct Vec3 firstPos;
    struct Vec3 nextPos;

    count = kSegments;
    if (data_0209f2f8 == kBigBoosHaunt)
        count = kBbhSegments;
    Matrix4x3_FromRotationXYZExt(&mTransform, mAngleX, mAngleY, mAngleZ);
    zero = 0;
    flag = 0;
    i = 0;
    if (i < count) {
        rowBase = (char *)this;
        flamePos = rowBase + 0x3a4;
        collider = rowBase + 0xd4;
        do {
            im1 = i - 1;
            offset.x = zero;
            offset.y = zero;
            offset.z = zero;
            dst.x = zero;
            dst.y = zero;
            dst.z = zero;
            offset.z = data_ov095_02136fb0[i];
            MulVec3Mat4x3((int *)&offset, &mTransform, (int *)&dst);
            if (im1 < 0) {
                Vec3_Add((int *)&firstPos, (int *)&mPosX, (int *)&dst);
                *(int *)(rowBase + 0x3a4) = firstPos.x;
                *(int *)(rowBase + 0x3a8) = firstPos.y;
                *(int *)(rowBase + 0x3ac) = firstPos.z;
            } else {
                Vec3_Add((int *)&nextPos, (int *)((char *)this + 0x3a4 + im1 * 0xc), (int *)&dst);
                *(int *)(rowBase + 0x3a4) = nextPos.x;
                *(int *)(rowBase + 0x3a8) = nextPos.y;
                *(int *)(rowBase + 0x3ac) = nextPos.z;
            }
            {
                short segmentSize = data_ov095_02136f80[i];
                _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(
                    collider, (struct Vec3 *)flamePos,
                    segmentSize * kRadiusMul, segmentSize * kHeightMul,
                    kClsnFlags, flag);
            }
            rowBase += 0xc;
            flamePos += 0xc;
            collider += 0x3c;
            i++;
        } while (i < count);
    }
    return 1;
}

// @symbol _ZN19daObjFlamethrower_c8BehaviorEv
int daObjFlamethrower_c::Behavior()
{
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
    int in[3];
    int dst[3];
    /* The y lift is interleaved into the argument build. volatile keeps
     * that store order; the array is not a live value. */
    volatile int part[3];
    int sum[3];
    int sum2[3];
    int y;
    int hidden;
    u32 id;
    dActor_c *other;
    int isPlayer;
    s16 ang;
    int a2;
    u8 *active;

    self = (char *)this;
    count = kSegments;
    if (data_0209f2f8 == kBigBoosHaunt)
        count = kBbhSegments;
    if (count <= 1)
        count = 1;
    if (count >= kSegments)
        count = kSegments;

    switch (mState) {
    case 0:
        if (mActiveFlames != 0) {
            /* Pooled address. mActiveFlames-- is ldrb [this, #0x465]. */
            active = (u8 *)((self + 0x400) + 0x65);
            *active = (u8)(*active - 1);
        }
        if (DecIfAbove0_Short(&mTimer) == 0) {
            mState = 1;
            mTimer = kPhaseFrames;
        }
        break;
    case 1:
        if ((int)mActiveFlames < (count - 1)) {
            active = (u8 *)((self + 0x400) + 0x65);
            *active = (u8)(*active + 1);
        }
        if (DecIfAbove0_Short(&mTimer) == 0) {
            mState = 0;
            mTimer = kPhaseFrames;
        }
        break;
    }

    if (mActiveFlames != 0) {
        mSoundHandle = Sound::PlayLong(
            mSoundHandle, 3, kFlameSound,
            *(const Vector3 *)&mCamSpacePosX, 0);
    }

    if ((mFlags & kOffScreen) != 0)
        hidden = 1;
    else
        hidden = 0;
    if (hidden != 0)
        return 1;

    if (((param1 & 0xff) != kAltParam) || (data_0209f2f8 == kBigBoosHaunt))
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
                if (((param1 & 0xff) != kAltParam) || (data_0209f2f8 == kBigBoosHaunt)) {
                    im1 = i - 1;
                    in[0] = zero;
                    in[1] = zero;
                    in[2] = zero;
                    dst[0] = zero;
                    dst[1] = zero;
                    dst[2] = zero;
                    in[2] = data_ov095_02136fb0[i];
                    MulVec3Mat4x3(in, &mTransform, dst);
                    if (im1 < 0) {
                        Vec3_Add(sum, (int *)&mPosX, dst);
                        *((int *)(ppos + 0x3a4)) = sum[0];
                        *((int *)(ppos + 0x3a8)) = sum[1];
                        *((int *)(ppos + 0x3ac)) = sum[2];
                    } else {
                        Vec3_Add(sum2, (int *)(self + 0x3a4 + im1 * 0xc), dst);
                        *((int *)(ppos + 0x3a4)) = sum2[0];
                        *((int *)(ppos + 0x3a8)) = sum2[1];
                        *((int *)(ppos + 0x3ac)) = sum2[2];
                    }
                    if (data_0209f2f8 == kBigBoosHaunt) {
                        s16 wave = data_02082214[(((u16)(s16)wavePhase) >> 4) * 2];
                        *((int *)(ppos + 0x3a8)) = mPosY
                            + (int)(((((long long)wave) * (-0xaa000)) + 0x800) >> 12);
                    }
                    *((int *)(pclsn + 0x104)) = *((int *)(ppos + 0x3a4));
                    *((int *)(pclsn + 0x108)) = *((int *)(ppos + 0x3a8));
                    *((int *)(pclsn + 0x10c)) = *((int *)(ppos + 0x3ac));
                }
                id = *((u32 *)(pclsn + 0xf8));
                if (id != 0) {
                    other = dActor_c::FindWithID(id);
                    if (other != 0) {
                        if (other->actorID == kPlayerActorId)
                            isPlayer = one;
                        else
                            isPlayer = no;
                        if (isPlayer != 0)
                            ((Player *)other)->Burn();
                    }
                }
                part[0] = *((int *)(ppos + 0x3a4));
                y = *((int *)(ppos + 0x3a8));
                part[1] = y;
                y = y + kParticleLift;
                part[2] = *((int *)(ppos + 0x3ac));
                part[1] = y;
                if ((param1 & 0xff) != kAltParam) {
                    ang = data_ov095_02136f80[i];
                    a2 = ang0;
                    func_02022774(part[0], part[1], part[2], ang, a2);
                    func_020226fc(part[0], part[1], part[2], ang, data_ov095_02136f98[i]);
                } else {
                    ang = data_ov095_02136f80[i];
                    a2 = ang1;
                    func_02022864(part[0], part[1], part[2], ang, a2);
                    func_020227ec(part[0], part[1], part[2], ang, data_ov095_02136f98[i]);
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
