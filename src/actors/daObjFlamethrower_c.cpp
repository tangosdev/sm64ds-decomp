//cpp
/* daObjFlamethrower_c -- the flame jet (OBJ_FLAMETHROWER), ov095.
 *
 * A chain of up to twelve flame segments (nine when data_0209f2f8 is 0xc)
 * laid out along mTransform, one dCcPos_c each. It breathes: state 0 pulls
 * the flame in one segment a frame, state 1 pushes it out to full length,
 * and each lasts 60 frames. Every active segment burns a Mario it touches
 * (Player::Burn) and spawns its particles (one pair of emitters when param1's
 * low byte is not 0xff, the other when it is). In the 0xc mode the segment
 * heights follow a fixed sine curve along the chain.
 *
 * Identity is the cartridge's: ov095 holds _ZTS19daObjFlamethrower_c at
 * 0x021376b4 and _ZTI at 0x021376a8, and the 0x84-byte table at 0x021376f0
 * is the vtable daObjFlamethrower_c_classInit (0x02136ed4) installs.
 *
 * DO NOT "TIDY" THESE -- each one is load-bearing:
 *   Source is reverse ROM order (deferred codegen left on): InitResources,
 *   the highest address, comes first.
 *   The destructor has NO definition here: its inline body in the header
 *   emits exactly D1 then D0, ahead of every member, with no D2. Out of line
 *   it emits D2, D0, D1, and `tubuild.py linkcheck` refuses a TU whose
 *   licensed .text is not ROM-ascending; defer_codegen off plus an ascending
 *   rewrite leaves the pair LAST, which is worse. Both measured.
 *   Neither variant has a definition to hang a `// @symbol` marker on; both are
 *   scored through the inline lifecycle definition in the class header.
 *   Behavior's `volatile int part[3]` pins the y-bump interleave that the
 *   shards got from `#pragma opt_strength_reduction off`; the pragma is not
 *   carried and the TU matches 4/4 without it.
 *
 * Key-function side effect: InitResources is the key function, so the object
 * also emits _ZTV/_ZTI/_ZTS and the dActor_c/dBase_c/fBase_c RTTI chain. The
 * promotion is text-only; compiler_only_output externalizes all nine
 * records (deadstrip-data). Owning them as storage would need two intervals,
 * because g_profile_OBJ_FLAMETHROWER (0x021376cc) sits between the RTTI and
 * the vtable.
 *
 * Known limits:
 *   Both members reach the actor through `char *` offsets rather than the
 *   named fields in daObjFlamethrower_c.h (0x3a4 mFlamePositions, 0x434
 *   mTransform, 0x464 mState, 0x465 mActiveFlames, 0x466 mTimer, 0x468
 *   mSoundHandle). The named spelling is not yet proved byte-identical.
 *   MulVec3Mat4x3 and Vec3_Add keep the `int *` spelling; InitResources
 *   casts at its three call sites (type-only, codegen-neutral).
 *   Sound::PlayLong, dCc_c::Clear/Update, dActor_c::FindWithID,
 *   Player::Burn and dCcPos_c::Init stay mangled: no shared header declares
 *   them in a form that keeps these call shapes.
 */

#include "daObjFlamethrower_c.h"
#include "decl_common.h"

/* Layout-identical three-int stand-in for the vector the two math helpers
 * take; see Known limits in the header comment above. */
struct Vec3 { int x, y, z; };

extern "C" {
extern signed char data_0209f2f8;
extern short data_02082214[];
extern short data_ov095_02136f98[];
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 a, u32 b, u32 c, const Vector3 *v, s16 e);
extern void MulVec3Mat4x3(int *v, void *m, int *dst);
extern void Vec3_Add(int *out, int *a, int *b);
extern char *_ZN8dActor_c10FindWithIDEj(u32 id);
extern void _ZN6Player4BurnEv(char *thiz);
extern void func_02022774(Fix12i x, Fix12i y, Fix12i z, s16 ang1, int ang2);
extern void func_020226fc(Fix12i x, Fix12i y, Fix12i z, s16 ang1, s16 ang2);
extern void func_02022864(Fix12i x, Fix12i y, Fix12i z, s16 ang1, int ang2);
extern void func_020227ec(Fix12i x, Fix12i y, Fix12i z, s16 ang1, s16 ang2);
extern void _ZN5dCc_c5ClearEv(void *thiz);
extern void _ZN5dCc_c6UpdateEv(void *thiz);
extern void _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(void *self, struct Vec3 *pos, int fix, int t, unsigned int a, unsigned int b);
}

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

    count = 0xc;
    if (data_0209f2f8 == 0xc) count = 9;
    Matrix4x3_FromRotationXYZExt(((char *)this) + 0x434, mAngleX, mAngleY, mAngleZ);
    zero = 0;
    flag = 0;
    i = 0;
    if (i < count) {
        rowBase = ((char *)this);
        flamePos = ((char *)this) + 0x3a4;
        collider = ((char *)this) + 0xd4;
        do {
        im1 = i - 1;
        offset.x = zero;
        offset.y = zero;
        offset.z = zero;
        dst.x = zero;
        dst.y = zero;
        dst.z = zero;
        offset.z = data_ov095_02136fb0[i];
        MulVec3Mat4x3((int *)&offset, ((char *)this) + 0x434, (int *)&dst);
        if (im1 < 0) {
            Vec3_Add((int *)&firstPos, (int *)((char *)&mPosX), (int *)&dst);
            *(int *)(rowBase + 0x3a4) = firstPos.x;
            *(int *)(rowBase + 0x3a8) = firstPos.y;
            *(int *)(rowBase + 0x3ac) = firstPos.z;
        } else {
            Vec3_Add((int *)&nextPos, (int *)(((char *)this) + 0x3a4 + im1 * 0xc), (int *)&dst);
            *(int *)(rowBase + 0x3a4) = nextPos.x;
            *(int *)(rowBase + 0x3a8) = nextPos.y;
            *(int *)(rowBase + 0x3ac) = nextPos.z;
        }
        {
            short segmentSize = data_ov095_02136f80[i];
            _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(collider, (struct Vec3 *)flamePos, segmentSize * 0x14, segmentSize * 0x28, 0x200002, flag);
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
    /* The y-bump below is interleaved into the argument build in the
     * cartridge; `volatile` pins that interleave without the shards'
     * `#pragma opt_strength_reduction off`. Not a live value. */
    volatile int part[3];
    int sum[3];
    int sum2[3];
    int y;
    int hidden;
    u32 id;
    char *other;
    int isPlayer;
    s16 ang;
    int a2;
    u8 *p;

    self = (char *)this;
    count = 0xc;
    if (data_0209f2f8 == 0xc)
        count = 9;
    if (count <= 1)
        count = 1;
    if (count >= 0xc)
        count = 0xc;

    switch (*((u8 *)(self + 0x464))) {
    case 0:
        if (*((u8 *)(self + 0x465)) != 0) {
            p = (u8 *)((self + 0x400) + 0x65);
            *p = (u8)(*p - 1);
        }
        if (DecIfAbove0_Short((u16 *)(self + 0x466)) == 0) {
            *((u8 *)(self + 0x464)) = 1;
            *((u16 *)((self + 0x400) + 0x66)) = 0x3c;
        }
        break;
    case 1:
        if ((int)(*((u8 *)(self + 0x465))) < (count - 1)) {
            p = (u8 *)((self + 0x400) + 0x65);
            *p = (u8)(*p + 1);
        }
        if (DecIfAbove0_Short((u16 *)(self + 0x466)) == 0) {
            *((u8 *)(self + 0x464)) = 0;
            *((u16 *)((self + 0x400) + 0x66)) = 0x3c;
        }
        break;
    }

    if (*((u8 *)(self + 0x465)) != 0) {
        *((u32 *)(self + 0x468)) = _ZN5Sound8PlayLongEjjjRK7Vector3s(
            *((u32 *)(self + 0x468)), 3, 0x180, (const Vector3 *)(self + 0x74), 0);
    }

    if (*((u32 *)(self + 0xb0)) & 8)
        hidden = 1;
    else
        hidden = 0;
    if (hidden != 0)
        return 1;

    if (((*((u32 *)(self + 8)) & 0xff) != 0xff) || (data_0209f2f8 == 0xc)) {
        Matrix4x3_FromRotationXYZExt(self + 0x434,
            *((s16 *)(self + 0x8c)), *((s16 *)(self + 0x8e)), *((s16 *)(self + 0x90)));
    }

    if (*((u8 *)(self + 0x465)) != 0) {
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
                if (i == (int)(*((u8 *)(self + 0x465))))
                    break;
                if (((*((u32 *)(self + 8)) & 0xff) != 0xff) || (data_0209f2f8 == 0xc)) {
                    im1 = i - 1;
                    in[0] = zero;
                    in[1] = zero;
                    in[2] = zero;
                    dst[0] = zero;
                    dst[1] = zero;
                    dst[2] = zero;
                    in[2] = data_ov095_02136fb0[i];
                    MulVec3Mat4x3(in, self + 0x434, dst);
                    if (im1 < 0) {
                        Vec3_Add(sum, (int *)(self + 0x5c), dst);
                        *((int *)(ppos + 0x3a4)) = sum[0];
                        *((int *)(ppos + 0x3a8)) = sum[1];
                        *((int *)(ppos + 0x3ac)) = sum[2];
                    } else {
                        Vec3_Add(sum2, (int *)(self + 0x3a4 + im1 * 0xc), dst);
                        *((int *)(ppos + 0x3a4)) = sum2[0];
                        *((int *)(ppos + 0x3a8)) = sum2[1];
                        *((int *)(ppos + 0x3ac)) = sum2[2];
                    }
                    if (data_0209f2f8 == 0xc) {
                        s16 wave = data_02082214[(((u16)(s16)wavePhase) >> 4) * 2];
                        *((int *)(ppos + 0x3a8)) = (*((int *)(self + 0x60)))
                            + ((int)(((((long long)wave) * (-0xaa000)) + 0x800) >> 12));
                    }
                    *((int *)(pclsn + 0x104)) = *((int *)(ppos + 0x3a4));
                    *((int *)(pclsn + 0x108)) = *((int *)(ppos + 0x3a8));
                    *((int *)(pclsn + 0x10c)) = *((int *)(ppos + 0x3ac));
                }
                id = *((u32 *)(pclsn + 0xf8));
                if (id != 0) {
                    other = _ZN8dActor_c10FindWithIDEj(id);
                    if (other != 0) {
                        if (*((u16 *)(other + 0xc)) == 0xbf)
                            isPlayer = one;
                        else
                            isPlayer = no;
                        if (isPlayer != 0)
                            _ZN6Player4BurnEv(other);
                    }
                }
                part[0] = *((int *)(ppos + 0x3a4));
                y = *((int *)(ppos + 0x3a8));
                part[1] = y;
                y = y + 0x32000;
                part[2] = *((int *)(ppos + 0x3ac));
                part[1] = y;
                if ((*((u32 *)(self + 8)) & 0xff) != 0xff) {
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
        char *pc = self + 0xd4;
        do {
            _ZN5dCc_c5ClearEv(pc);
            if (j < (int)(*((u8 *)(self + 0x465))))
                _ZN5dCc_c6UpdateEv(pc);
            j++;
            pc += 0x3c;
        } while (j < count);
    }
    return 1;
}
