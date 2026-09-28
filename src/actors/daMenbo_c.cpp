//cpp
/* daMenbo_c -- Skeeter, the water strider on the lake.
 *
 * ov090 0x02130f00..0x02132654, twenty-four functions. daManta_c opens the
 * next run at 0x0213269c. The factory daMenbo_c_classInit stays in its own
 * file, and g_profile_MENBO stays out of this one. One out-of-line destructor
 * is the key function: it emits D1 then D0 and anchors the vtable.
 *
 * __sinit_ov090_02133ce8 copies the eight-byte PMF pairs at 0x021340e0 into
 * the four state nodes. Entry is the first pair, update the second:
 *   data_ov090_021344e4  idle         02131608 / 02131584   anim 02134498
 *   data_ov090_02134514  patrol       02131a74 / 02131648   anim 02134480
 *   data_ov090_02134504  water rest   02131b94 / 02131ac4   anim 02134490
 *   data_ov090_021344f4  water chase  02131db0 / 02131c48   anim 02134488
 * Idle and patrol hand off to water rest while unk_39c is set (the skeeter
 * is on the water line). Water chase hands back to idle when it clears.
 * Patrol itself speeds up when a player is close; that is not a fifth state.
 *
 * The tail the header still spells unk_ or pad_:
 *   unk_374..37c  anchor position
 *   unk_394       turn cooldown
 *   unk_396       post-turn hold
 *   unk_398       heading-jitter timer
 *   unk_39a       target heading
 *   unk_39c       on the water line this frame
 *   unk_3a1       death tumble (3 pitches down and yaws)
 *   unk_3a4       playback rate, copied into the frame controller
 *   unk_3a8       floor height from the ground probe
 *   unk_3ac       water line
 *   pad_380       four ripple-particle ids, then the anim-loop counter
 *   pad_39d       chase / far / wall / missed-probe latches
 *
 * common.h is included first so Matrix4x3 is the flat twelve words the two
 * matrix copies were matched under. defer_codegen is off, so the bodies are
 * ROM-ascending. opt_strength_reduction stays off for the file;
 * opt_common_subs is off except around the ground probe and Behavior.
 *
 * deslop leftovers:
 * - func_ov090_02131584, func_ov090_02131648 and func_ov090_02131c48: the
 *   anim-loop counter lives at 0x390, which is MenboRipples::loops. Adding
 *   through that member is `add this, #0x380` then `add #0x10`. The ROM adds
 *   #0x390 once, and each function grew 4 bytes. Zeroing the member and
 *   reloading it for the compare both match; only the increment stays
 *   `*(int *)((int)this + 0x390)`.
 * - func_ov090_02131ac4: `mStateTimer == 0` is ldrsh. The ROM is ldrh, one
 *   word. `*(unsigned short *)&mStateTimer` matches.
 * - func_ov090_02131608: ModelAnim::SetAnim with a Fix12<int> rate is 0x4c
 *   against 0x40 and emits 4 bytes of .rodata. The scalar SetAnim call
 *   matches. The patrol, water-rest and water-chase entries keep it.
 * - func_ov090_021310b4: dActor_c::SpawnCoins with a Fix12<int> spread is
 *   0x2cc against 0x2c4 and emits 4 bytes of .rodata. The same function's
 *   KillByInvincibleChar with a Fix12<int> argument is 0x2d0 against 0x2c4.
 *   Both scalar calls match. Behavior's coin spawn keeps the scalar call.
 * - InitResources: dCcAcPos_c::Init with two Fix12<int> arguments is 0x2d0
 *   against 0x2bc and emits two 4-byte .rodata words. The scalar call matches.
 */

#pragma defer_codegen off

#include "common.h"
#include "daMenbo_c.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_Lin.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "Animation.h"

bool ApproachLinear(short &value, short target, short step);

#pragma opt_strength_reduction off
#pragma opt_common_subs off

struct BMD_File;
struct BCA_File;

/* mState is void* on the class. The node it points at is two PMFs: the
 * entry Behavior does not call, then the update it does. */
struct MenboState;
typedef int (MenboState::*MenboStateFn)();
struct MenboState { char pad[0x370]; MenboStateFn *mState; };

class MenboSelf {};
typedef void (MenboSelf::*MenboSelfFn)();
struct MenboStateNode { char pad[8]; MenboSelfFn mRun; };

/* Fills pad_380 exactly: four ids, then the loop counter at 0x390. */
struct MenboRipples {
    u32 id[4];
    s32 loops;
};
/* Fills pad_39d exactly. */
struct MenboLatches {
    u8 chase;
    u8 far;
    u8 wall;
    u8 probe;
};

struct CoinVec3 { s32 x, y, z; };

/* By-value Fix12<int> in a mangled name; the calls pass a bare word. */
typedef int LocFix12;

extern "C" {

void  MulMat4x3Mat4x3(Matrix4x3 *a, Matrix4x3 *b, Matrix4x3 *out);
void  MulVec3Mat4x3(const Vector3 *v, const void *m, Vector3 *out);
void  Matrix4x3_FromRotationY(void *m, s16 angle);
void  Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX);
void  Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, s16 rx, s16 ry, s16 rz);
void  Vec3_Lsl(Vector3 *out, const Vector3 *in, int n);
void  Vec3_Asr(void *dst, void *src, int n);
int   Vec3_Dist(const Vector3 *a, const Vector3 *b);
int   Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
s16   Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
int   AngleDiff(int a, int b);
int   ApproachAngle(s16 *cur, int target, int divisor, int band, int maxStep);
void  _Z14ApproachLinearRiii(int &cur, int target, int step);
u16   DecIfAbove0_Short(u16 *p);
int   RandomIntInternal(int *seed);
void  func_02012694(int id, const Vector3 *pos);

int   _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
          unsigned int a0, unsigned int a1, int a2, int a3, int a4, int a5, int a6);
int   _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, BCA_File *f, int a, LocFix12 rate, unsigned int n);

void  _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, dActor_c *a, const Vector3 *v, LocFix12 r, LocFix12 h, unsigned int e, unsigned int g);
/* Not dBgCh_Actr::Init: the header method mangles with int, the ROM symbol with Fix12<int>. */
void  _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(dBgCh_Actr *self, int a, LocFix12 r, LocFix12 h, int p, int q);
int   SurfaceInfo_TestFlag0x20(const SurfaceInfo *p);
void  func_0203558c(void *self);
int   func_02035638(u8 *p);
void  func_02035684(int *p, int v);

void  _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *self, const void *v, u32 n, s32 fix, s16 s);

void  _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(void *self, void *v, void *player, LocFix12 a);
void  func_ov002_020aea30(void *self, void *actor, void *collision);
void  _ZN6Player6BounceE5Fix12IiE(void *p, LocFix12 f);
void  _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *p, void *v, int a, LocFix12 b, int d, int e, int f);

extern Matrix4x3     data_020a0e68;
extern int           data_02092138;
extern int           data_0209e650;
extern unsigned char data_0209f2d8;
extern signed char   data_0209f2f8;
extern int           data_0209f32c;

extern Vector3       data_ov090_0213412c;
extern SharedFilePtr data_ov090_02134480; /* patrol anim, file 0x3a1 */
extern SharedFilePtr data_ov090_02134488; /* water-chase anim, file 0x3a0 */
extern SharedFilePtr data_ov090_02134490; /* water-rest anim, file 0x3a2 */
extern SharedFilePtr data_ov090_02134498; /* idle anim, file 0x3a3 */
extern SharedFilePtr data_ov090_021344a0; /* model, file 0x39f */
/* decl_common.h already spells data_ov090_02134504 as char. The other three
 * nodes follow it; each call site casts to the PMF it actually is.
 * e4 idle, f4 water chase, 504 water rest, 514 patrol. */
extern char          data_ov090_021344e4;
extern char          data_ov090_021344f4;
extern char          data_ov090_02134504;
extern char          data_ov090_02134514;

void func_ov090_021310b4(daMenbo_c *c);
void func_ov090_02131378(daMenbo_c *c);
int  func_ov090_021314a0(daMenbo_c *c);
int  func_ov090_02131584(daMenbo_c *c);
int  func_ov090_02131608(daMenbo_c *c);
int  func_ov090_02131648(MenboState *c);
int  func_ov090_02131a74(daMenbo_c *c);
int  func_ov090_02131ac4(daMenbo_c *c);
int  func_ov090_02131b94(daMenbo_c *c);
int  func_ov090_02131c48(daMenbo_c *c);
int  func_ov090_02131db0(daMenbo_c *c);
int  func_ov090_02131e00(MenboState *c, MenboStateFn *p);
void func_ov090_02131e50(daMenbo_c *c);

}

/* SharedFilePtr.h has no fields. The BCA pointer SetAnim takes is the
 * second word of each anim handle. */
#define MENBO_BCA(handle) (((BCA_File **)&(handle))[1])
#define MENBO_RIPPLES(c) ((MenboRipples *)(c)->pad_380)
#define MENBO_LATCH(c)   ((MenboLatches *)(c)->pad_39d)

// @symbol _ZN9daMenbo_cD1Ev
// @symbol _ZN9daMenbo_cD0Ev
daMenbo_c::~daMenbo_c()
{
}

// @symbol func_ov090_02130f94
/* One ripple under each of four leg bones. Skipped while the actor is off
 * screen. Bone matrices are transforms[6], [9], [12] and [15], multiplied
 * by the model matrix; the ripple sits on the water line. */
extern "C" void func_ov090_02130f94(daMenbo_c *c_)
{
    int zero;
    int sh;
    daMenbo_c *c;
    int i;
    Matrix4x3 *src;
    int idx;
    int b;
    unsigned int id;

    c = c_;
    b = (int)((c->mFlags & 8) != 0);
    if (b != 0) return;

    src = &c->mModelAnim.mat4x3;
    i = 0;
    idx = 6;
    zero = 0;
    sh = 3;
    id = 0xea;
    for (; i < 4; ++i) {
        Vector3 v;
        Vector3 r;

        data_020a0e68 = *src;
        MulMat4x3Mat4x3((Matrix4x3 *)c->mModelAnim.data.transforms + idx, &data_020a0e68, &data_020a0e68);
        v.x = *(int *)((char *)&data_020a0e68 + 0x24);
        v.y = *(int *)((char *)&data_020a0e68 + 0x28);
        v.z = *(int *)((char *)&data_020a0e68 + 0x2c);
        Vec3_Lsl(&r, &v, sh);
        v.x = r.x;
        v.y = r.y;
        v.z = r.z;
        v.y = c->unk_3ac;
        MENBO_RIPPLES(c)->id[i] =
            _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                MENBO_RIPPLES(c)->id[i], id, v.x, v.y, r.z, zero, zero);
        idx += 3;
    }
}

// @symbol func_ov090_021310b4
/* Contact response, in the order the tests run. A volatile read of the
 * cylinder offset keeps that copy as three loads. The 0xbf test is a
 * ternary widened through long long so it stays a moveq/movne pair. */
void func_ov090_021310b4(daMenbo_c *c)
{
    short hv[3];
    Vector3 sv;
    Vector3 cv;
    Vector3 hurt;
    int flags;
    dActor_c *p;
    int x, y, z;
    volatile Vector3 *src = &data_ov090_0213412c;

    x = src->x;
    y = src->y;
    z = src->z;
    sv.z = z;
    sv.x = x;
    sv.y = y;
    c->mdCcAcPos_c.SetPosRelativeToActor(sv);

    if (c->mdCcAcPos_c.otherOwner == 0) return;
    if ((p = dActor_c::FindWithID(c->mdCcAcPos_c.otherOwner)) == 0) return;
    flags = (int)((long long)*(int *)&c->mdCcAcPos_c.hitFlags);

    if (flags & 0x2400) {
        c->mDeathState = 2;
        func_ov002_020aea30(c, p, 0);
        return;
    }
    if (flags & 0x40000) {
        c->mDeathState = 4;
        func_ov002_020aea30(c, p, 0);
        return;
    }
    if (flags & 0x40) {
        c->mDeathState = 2;
        func_ov002_020aea30(c, p, 0);
        return;
    }
    if (c->JumpedOnByPlayer(*(dCc_c *)&c->mdCcAcPos_c, *(Player *)p)) {
        _ZN6Player6BounceE5Fix12IiE(p, 0x28000);
        c->mDeathState = 1;
        func_ov002_020aea30(c, p, 0);
        return;
    }
    if (flags & 0x20) {
        c->mVertAccel = 0;
        c->unk_0a4 = 0;
        c->mVertSpeed = 0;
        c->unk_0ac = 0;
        c->mDeathState = 1;
        func_ov002_020aea30(c, p, 0);
        return;
    }
    if (flags & 0x10) {
        hv[0] = 0x1000;
        hv[1] = 0;
        hv[2] = 0;
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, hv, p, 0);
        func_02012694(0x1d, (const Vector3 *)&c->mCamSpacePosX);
        return;
    }
    if (flags & 0x4380) {
        c->unk_3a1 = 3;
        c->mDeathState = 3;
        func_ov002_020aea30(c, p, 0);
        return;
    }
    {
        int b = (p->actorID == 0xbf) ? 1 : 0;
        if ((int)((long long)b) == 0) return;
    }
    if (((Player *)p)->mIsMetal == 1 || ((Player *)p)->IsOnShell() == 1) {
        cv.x = c->mPosX;
        cv.y = c->mPosY;
        cv.z = c->mPosZ;
        _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &cv, c->unk_10a + 1, 0xa000, 0);
        c->PoofDust();
        c->KillAndTrackInDeathTable();
        return;
    }
    hurt.x = c->mPosX;
    hurt.y = c->mPosY;
    hurt.z = c->mPosZ;
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, &hurt, 2, 0xc000, 1, 0, 1);
}

#pragma opt_common_subs on
// @symbol func_ov090_02131378
/* Ground probe. Mode 2 on course 0x12 skips it and only spawns ripples.
 * Otherwise a hit below the published water height (data_0209f32c) means
 * the skeeter is on the water: record that line, mark unk_39c, ripple.
 * A higher floor is the line instead, and either way Y is clamped up to it. */
void func_ov090_02131378(daMenbo_c *c)
{
    int b = (int)(data_0209f2d8 == 2);
    if (b != 0) {
        if (data_0209f2f8 == 0x12) {
            func_ov090_02130f94(c);
            return;
        }
    }
    dBgCh_Gnd rg;
    Vector3 v;
    int vz = c->mPosZ;
    int vy = c->mPosY + 0x32000;
    v.x = c->mPosX;
    v.y = vy;
    v.z = vz;
    c->unk_39c = 0;
    if (Vec3_HorzDist((Vector3 *)&c->mPosX, (Vector3 *)&c->mPrevPosX) != 0) {
        rg.SetObjAndPos(v, c);
        if (rg.DetectClsn() != 0)
            c->unk_3a8 = rg.clsnY;
    }
    if (c->unk_3a8 < data_0209f32c) {
        c->unk_3ac = data_0209f32c;
        if (c->mPosY <= c->unk_3ac) {
            c->unk_39c = 1;
            func_ov090_02130f94(c);
        }
    } else {
        c->unk_3ac = c->unk_3a8;
    }
    if (c->mPosY <= c->unk_3ac) {
        c->mPosY = c->unk_3ac;
        c->mVertSpeed = 0;
    }
}
#pragma opt_common_subs off

// @symbol func_ov090_021314a0
/* 1 while a player is close and within a quarter turn of the heading.
 * The held target latches; releasing it points the skeeter back at its
 * anchor and sets the patrol speed. */
int func_ov090_021314a0(daMenbo_c *c)
{
    Player *p = c->ClosestNonVanishPlayer();
    if (p == 0) goto out;
    if (AngleDiff(c->mPrevAngleY, c->HorzAngleToCPlayer()) >= 0x2000) goto out;
    {
        int *sv = &p->mPosX;
        int v[3];
        v[0] = sv[0];
        v[1] = sv[1];
        v[2] = sv[2];
        if (Vec3_Dist((const Vector3 *)&c->mPosX, (const Vector3 *)v) >= 0x3ac000) goto out;
        int w[3];
        w[0] = v[0];
        w[1] = v[1];
        w[2] = v[2];
        c->unk_39a = (u16)Vec3_HorzAngle((const Vector3 *)&c->mPosX, (const Vector3 *)w);
        MENBO_LATCH(c)->chase = 1;
        return 1;
    }
out:
    if (MENBO_LATCH(c)->chase == 1) {
        c->mHorzSpeed = 0x9000;
        MENBO_LATCH(c)->chase = 0;
        c->unk_39a = (u16)Vec3_HorzAngle((const Vector3 *)&c->mPosX, (const Vector3 *)&c->unk_374);
    }
    return 0;
}

// @symbol func_ov090_02131584
/* Idle. Speed stays zero. After a few loops of the idle anim, or as soon
 * as a player is in range, start patrolling. On the water, rest instead. */
int func_ov090_02131584(daMenbo_c *c)
{
    unsigned int v;
    c->mHorzSpeed = 0;
    v = ((unsigned int)static_cast<Animation &>(c->mModelAnim).currFrame << 4) >> 0x10;
    if (v >= 0x3b)
        *(int *)((int)c + 0x390) += 1;
    if (MENBO_RIPPLES(c)->loops > 2 || func_ov090_021314a0(c) == 1)
        func_ov090_02131e00((MenboState *)c, (MenboStateFn *)&data_ov090_02134514);
    if (c->unk_39c == 1)
        func_ov090_02131e00((MenboState *)c, (MenboStateFn *)&data_ov090_02134504);
    return 1;
}

// @symbol func_ov090_02131608
/* Idle entry. */
int func_ov090_02131608(daMenbo_c *c)
{
    MENBO_RIPPLES(c)->loops = 0;
    c->unk_3a4 = 0x1000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim, MENBO_BCA(data_ov090_02134498), 0, 0x1000, 0);
    return 1;
}

// @symbol func_ov090_02131648
/* Patrol. A line probe ahead, the distance back to the anchor, and a wall
 * each turn it around, and each latches so the turn only fires on the edge.
 * A turn reloads the three timers and snaps the position back to last
 * frame's. The heading then eases toward unk_39a. Close to a player, the
 * playback rate and the speed both double for a bit. */
int func_ov090_02131648(MenboState *c)
{
    daMenbo_c *self = (daMenbo_c *)c;
    int dist;
    u32 rnd;
    int selfY;
    Vector3 a, b, in, out;
    int angleSet;
    s16 *p39a;
    int sh;
    s16 ha;
    s32 *p390;

    rnd = (u32)RandomIntInternal(&data_0209e650) >> 8;
    dist = Vec3_Dist((Vector3 *)&self->mPosX, (Vector3 *)&self->unk_374);

    if (((((u32)static_cast<Animation &>(self->mModelAnim).currFrame) << 4) >> 0x10 & 0xf) == 0) {
        func_02012694(0xfc, (const Vector3 *)&self->mCamSpacePosX);
    }

    angleSet = 0;
    dBgCh_Lin line;

    a.x = 0; a.y = 0; a.z = 0;
    b.x = 0; b.y = 0; b.z = 0;
    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;

    a.x = self->mPosX;
    selfY = self->mPosY;
    a.y = selfY;
    a.z = self->mPosZ;
    a.y = selfY + 0x64000;
    in.y = 0x64000;
    in.z = 0x1f4000;

    Matrix4x3_FromRotationY(&data_020a0e68, self->mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, 0x2000);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);

    b.x = a.x;
    b.x = a.x + out.x;
    b.y = a.y;
    b.z = a.z;
    b.y = a.y + out.y;
    b.z = a.z + out.z;

    line.SetObjAndLine(a, b, self);

    if (!line.DetectClsn()) {
        if (MENBO_LATCH(self)->probe == 0) {
            ha = Vec3_HorzAngle((Vector3 *)&self->mPosX, (Vector3 *)&self->unk_374);
            sh = (rnd & 3) << 0xc;
            self->unk_39a = (u16)ha;
            p39a = (s16 *)&self->unk_39a;
            angleSet = 1;
            *p39a = *p39a + (0x1800 - sh);
            self->mPosX = self->mPrevPosX;
            self->mPosY = self->mPrevPosY;
            self->mPosZ = self->mPrevPosZ;
            MENBO_LATCH(self)->probe = 1;
        }
    } else {
        MENBO_LATCH(self)->probe = (u8)angleSet;
    }

    if (dist > 0x3c0000) {
        if (MENBO_LATCH(self)->far == 0 && MENBO_LATCH(self)->probe == 0) {
            ha = Vec3_HorzAngle((Vector3 *)&self->mPosX, (Vector3 *)&self->unk_374);
            self->unk_39a = (u16)ha;
            angleSet = 1;
            self->mPosX = self->mPrevPosX;
            self->mPosY = self->mPrevPosY;
            self->mPosZ = self->mPrevPosZ;
            MENBO_LATCH(self)->far = 1;
        }
    } else {
        MENBO_LATCH(self)->far = 0;
    }

    if (self->mWithMeshClsn.IsOnWall() != 0) {
        if (MENBO_LATCH(self)->wall == 0 && MENBO_LATCH(self)->far == 0 && MENBO_LATCH(self)->probe == 0) {
            ha = Vec3_HorzAngle((Vector3 *)&self->mPosX, (Vector3 *)&self->unk_374);
            sh = (rnd & 3) << 0xc;
            self->unk_39a = (u16)ha;
            p39a = (s16 *)&self->unk_39a;
            angleSet = 1;
            *p39a = *p39a + (0x1800 - sh);
            self->mPosX = self->mPrevPosX;
            self->mPosY = self->mPrevPosY;
            self->mPosZ = self->mPrevPosZ;
            MENBO_LATCH(self)->wall = 1;
        }
    } else {
        MENBO_LATCH(self)->wall = 0;
    }

    if (self->unk_398 == 0 && MENBO_LATCH(self)->wall == 0 && MENBO_LATCH(self)->far == 0 && MENBO_LATCH(self)->probe == 0) {
        self->unk_398 = (u16)((rnd + 0x32) & 0x3f);
        p39a = (s16 *)&self->unk_39a;
        *p39a = *p39a + (0x1800 - ((rnd & 3) << 0xc));
    }

    if (angleSet == 1) {
        self->unk_394 = 0xa;
        self->unk_398 = 0x32;
        self->unk_396 = 0x1e;
        self->mHorzSpeed = 0x9000;
        self->unk_3a4 = 0x1000;
    }

    ApproachLinear(self->mPrevAngleY, (s16)self->unk_39a, 0x500);

    if (self->unk_394 == 0 && self->unk_396 == 0) {
        self->unk_3a4 = 0x1000;
        self->mHorzSpeed = 0x9000;
        if (func_ov090_021314a0(self) == 1) {
            self->unk_396 = 0x1e;
            self->unk_398 = 0x1e;
            self->unk_3a4 = 0x2000;
            self->mHorzSpeed = 0xe000;
        }
    }

    if (((u32)(((u32)static_cast<Animation &>(self->mModelAnim).currFrame) << 4) >> 0x10) >= 0x10) {
        p390 = (s32 *)((int)self + 0x390);
        *p390 = *p390 + 1;
    }

    if (MENBO_RIPPLES(self)->loops > 0x1e && MENBO_LATCH(self)->far == 0) {
        func_ov090_02131e00(c, (MenboStateFn *)&data_ov090_021344e4);
    }

    if (self->unk_39c == 1) {
        func_ov090_02131e00(c, (MenboStateFn *)&data_ov090_02134504);
    }

    return 1;
}

// @symbol func_ov090_02131a74
/* Patrol entry. */
int func_ov090_02131a74(daMenbo_c *c)
{
    c->unk_3a4 = 0x1000;
    c->unk_396 = 0;
    c->unk_398 = 0x32;
    MENBO_RIPPLES(c)->loops = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim, MENBO_BCA(data_ov090_02134480), 0, 0x1000, 0);
    return 1;
}

// @symbol func_ov090_02131ac4
/* Water rest. Bleed the speed off. Once it is gone, and the skeeter is not
 * on the water, drop the anchor here, pick a random heading and go idle.
 * When the rest timer expires, start the water chase. The heading keeps
 * easing either way. */
int func_ov090_02131ac4(daMenbo_c *c)
{
    _Z14ApproachLinearRiii(c->mHorzSpeed, 0, 0x1000);
    if (c->mHorzSpeed > 0) return 1;
    if (c->unk_39c == 0) {
        c->unk_374 = c->mPosX;
        c->unk_378 = c->mPosY;
        c->unk_37c = c->mPosZ;
        c->unk_39a = (u16)(((unsigned int)RandomIntInternal(&data_0209e650) >> 8) << 0xd);
        func_ov090_02131e00((MenboState *)c, (MenboStateFn *)&data_ov090_021344e4);
    }
    ApproachAngle(&c->mPrevAngleY, (s16)c->unk_39a, 1, 0x100, 0x100);
    if (*(unsigned short *)&c->mStateTimer == 0)
        func_ov090_02131e00((MenboState *)c, (MenboStateFn *)&data_ov090_021344f4);
    return 1;
}

// @symbol func_ov090_02131b94
/* Water-rest entry. Two random draws jitter the heading, a third sets how
 * long the rest lasts, then the rest anim starts. */
int func_ov090_02131b94(daMenbo_c *c)
{
    unsigned int r;
    short *s;
    r = (unsigned)RandomIntInternal(&data_0209e650);
    s = (short *)((unsigned int)c + 0x39a);
    *s = (short)(*s + ((int)(((r >> 8) & 3) << 0x1e) >> 16));
    r = (unsigned)RandomIntInternal(&data_0209e650);
    s = (short *)((char *)c + 0x39a);
    *s = (short)(*s + ((int)(((r >> 8) & 7) << 0x1d) >> 16));
    r = (unsigned)RandomIntInternal(&data_0209e650);
    c->mStateTimer = (short)(((r >> 8) & 0x1f) + 0x96);
    c->unk_3a4 = 0x1000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim, MENBO_BCA(data_ov090_02134490), 0, 0x1000, 0);
    return 1;
}

// @symbol func_ov090_02131c48
/* Water chase. Full speed after a few frames, and a wall close to the
 * heading kicks the target by a quarter turn. When the anim has looped
 * enough, go back to water rest. Off the water, drop the anchor and idle. */
int func_ov090_02131c48(daMenbo_c *c)
{
    if ((((unsigned)static_cast<Animation &>(c->mModelAnim).currFrame) << 4) >> 16 >= 4)
        c->mHorzSpeed = 0x19000;

    if ((((unsigned)static_cast<Animation &>(c->mModelAnim).currFrame) << 4) >> 16 == 4)
        func_02012694(0xfd, (const Vector3 *)&c->mCamSpacePosX);

    if (c->unk_394 == 0
        && c->mWithMeshClsn.IsOnWall()
        && AngleDiff(c->mPrevAngleY, (s16)c->unk_39a) < 0x200) {
        s16 *p = (s16 *)&c->unk_39a;
        *p = *p + 0x4000;
        c->unk_394 = 8;
    }

    if (c->unk_394 != 0) {
        ApproachAngle(&c->mPrevAngleY, (s16)c->unk_39a, 1, 0x1000, 0x1000);
    } else {
        if (static_cast<Animation &>(c->mModelAnim).Finished()) {
            s32 *q = (s32 *)((int)c + 0x390);
            *q = *q + 1;
            if (MENBO_RIPPLES(c)->loops > 0x14)
                func_ov090_02131e00((MenboState *)c, (MenboStateFn *)&data_ov090_02134504);
        }
    }

    if (c->unk_39c == 0) {
        c->unk_374 = c->mPosX;
        c->unk_378 = c->mPosY;
        c->unk_37c = c->mPosZ;
        c->unk_39a = (u16)(((unsigned)RandomIntInternal(&data_0209e650) >> 8) << 13);
        func_ov090_02131e00((MenboState *)c, (MenboStateFn *)&data_ov090_021344e4);
    }

    return 1;
}

// @symbol func_ov090_02131db0
/* Water-chase entry. The 0x40000000 flag is passed straight through. */
int func_ov090_02131db0(daMenbo_c *c)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&c->mModelAnim, MENBO_BCA(data_ov090_02134488), 0x40000000, 0x1000, 0);
    c->unk_3a4 = 0x1000;
    MENBO_RIPPLES(c)->loops = 0;
    return 1;
}

// @symbol func_ov090_02131e00
/* Install a state and run its entry. A null entry PMF means there is none. */
int func_ov090_02131e00(MenboState *c, MenboStateFn *p)
{
    c->mState = p;
    MenboStateFn *q = c->mState;
    if (*q == 0) return 1;
    return (c->**q)();
}

// @symbol func_ov090_02131e50
/* Model matrix from position (shifted down 3, the renderer's scale) and
 * the three rotation halfwords. */
void func_ov090_02131e50(daMenbo_c *c)
{
    int src[3];
    int dst[3];
    src[0] = c->mPosX;
    src[1] = c->mPosY;
    src[2] = c->mPosZ;
    Vec3_Asr(dst, src, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, dst[0], dst[1], dst[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, c->mAngleX, c->mAngleY, c->mAngleZ);
    c->mModelAnim.mat4x3 = data_020a0e68;
}

// @symbol _ZN9daMenbo_c16CleanupResourcesEv
/* The model and the four anims, released in this order. */
int daMenbo_c::CleanupResources()
{
    data_ov090_021344a0.Release();
    data_ov090_02134488.Release();
    data_ov090_02134480.Release();
    data_ov090_02134490.Release();
    data_ov090_02134498.Release();
    return 1;
}

// @symbol _ZN9daMenbo_c16OnPendingDestroyEv
/* Empty. The override is what keeps the base from running. */
void daMenbo_c::OnPendingDestroy()
{
}

// @symbol _ZN9daMenbo_c6RenderEv
/* Skip the frame while the Yoshi-mouth flag is set. */
int daMenbo_c::Render()
{
    unsigned int f = mFlags;
    int b = ((f & 0x40000) != 0);
    if (b) return 1;
    mModelAnim.Render((const Vector3 *)&mScaleX);
    return 1;
}

#pragma opt_common_subs on
// @symbol _ZN9daMenbo_c8BehaviorEv
/* Yoshi's mouth, a killing blow, the death tumble, or one ordinary frame.
 * Course 0x15 area 1 treats a water hit on the mesh as a knock into the
 * water: motion zeroed, mDeathState = 1. Dying at or below the water line
 * pays the coins out. The cylinder stays quiet while the closest player
 * is vanished. */
int daMenbo_c::Behavior()
{
    char *c = (char *)this;

    if (UpdateYoshiEat(mWithMeshClsn)) {
        mdCcAcPos_c.Clear();
        if (mEatenByYoshi != 0 && unk_104 == 0)
            mdCcAcPos_c.Update();
        func_ov090_02131e50(this);
        return 1;
    }

    if (UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim, 3))
        return 1;

    if (mDeathState != 0) {
        func_02035684((int *)(&mWithMeshClsn), 0xd2000);
        UpdateWMClsn(mWithMeshClsn, 0);
        if (UpdateDeath(mWithMeshClsn))
            return 1;
        func_ov090_02131378(this);
        func_ov090_02131e50(this);
        if (mDeathState == 0)
            PoofDust();
        if (unk_3a1 == 3) {
            ApproachLinear(mAngleX, -32767, 0x500);
            if (AngleDiff(*&mAngleX, -32767) < 0x1000) {
                s16 *yaw = &mAngleY;
                *yaw += 0x1000;
            }
        }
        if (mDeathState != 1 && mPosY <= unk_3ac) {
            CoinVec3 v;
            v.x = mPosX;
            v.y = mPosY;
            v.z = mPosZ;
            _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &v, unk_10a + 1, 0xa000, 0);
            PoofDust();
            KillAndTrackInDeathTable();
        }
        return 1;
    }

    {
    int flag = (mFlags & 8) != 0;
    if (flag) {
        mHorzSpeed = 0;
        UpdatePos(&mdCcAcPos_c);
        func_ov090_02131378(this);
        if (data_0209f2f8 == 0x15 && mAreaId == 1) {
            UpdateWMClsn(mWithMeshClsn, 2);
            if (func_02035638((u8 *)(&mWithMeshClsn))) {
                mVertAccel = 0;
                unk_0a4 = 0;
                mVertSpeed = 0;
                unk_0ac = 0;
                mDeathState = 1;
                func_ov002_020aea30(c, ClosestPlayer(), 0);
                return 1;
            }
        }
        return 1;
    }
    }

    UpdatePos(&mdCcAcPos_c);
    func_ov090_02131378(this);
    DecIfAbove0_Short((u16 *)&mStateTimer);
    DecIfAbove0_Short(&unk_394);
    DecIfAbove0_Short(&unk_396);
    DecIfAbove0_Short(&unk_398);
    UpdateWMClsn(mWithMeshClsn, 2);
    if (mPosY <= unk_3ac)
        mPosY = unk_3ac;
    if (data_0209f2f8 == 0x15 && mAreaId == 1 && func_02035638((u8 *)(&mWithMeshClsn))) {
        mVertAccel = 0;
        unk_0a4 = 0;
        mVertSpeed = 0;
        unk_0ac = 0;
        mDeathState = 1;
        func_ov002_020aea30(c, ClosestPlayer(), 0);
        return 1;
    }

    {
        MenboStateNode *n = (MenboStateNode *)mState;
        if (n->mRun)
            (((MenboSelf *)c)->*(n->mRun))();
    }
    mAngleY = mPrevAngleY;
    static_cast<Animation &>(mModelAnim).speed = unk_3a4;
    static_cast<Animation &>(mModelAnim).Advance();
    func_ov090_02131e50(this);
    func_ov090_021310b4(this);
    mdCcAcPos_c.Clear();
    {
        Player *p = ClosestPlayer();
        if (p != 0 && p->mIsVanish == 0)
            mdCcAcPos_c.Update();
    }
    return 1;
}
#pragma opt_common_subs off

// @symbol _ZN9daMenbo_c13InitResourcesEv
/* Model, four anims, both collision volumes, then the water line.
 * Mode 2 on course 0x12 skips the raycast and starts at water rest with
 * the line equal to the spawn height. A surface whose 0x20 flag is set
 * records the hit as the line only and also starts at water rest. Anything
 * else snaps Y to the line, randomises the heading and starts idle. */
int daMenbo_c::InitResources()
{
    BMD_File *f;
    int r;
    Vector3 pos;
    Vector3 v;

    f = (BMD_File *)Model::LoadFile(data_ov090_021344a0);
    mModelAnim.SetFile(f, 1, -1);
    Animation::LoadFile(data_ov090_02134488);
    Animation::LoadFile(data_ov090_02134480);
    Animation::LoadFile(data_ov090_02134490);
    Animation::LoadFile(data_ov090_02134498);

    mTerminalVelocity = -0x3c000;

    v.x = data_ov090_0213412c.x;
    v.y = data_ov090_0213412c.y;
    v.z = data_ov090_0213412c.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &v, 0x5a000, 0x5a000, 0x200000, 0x7eff0);

    mAngleY = mPrevAngleY;
    unk_3a4 = 0x1000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, (int)this, 0xc8000, 0, 0, 0);
    func_0203558c(&mWithMeshClsn);

    unk_108 = 1;
    unk_10a = 2;
    mVertAccel = -0x3000;

    {
        int b = 1;
        if (data_0209f2d8 != 2) b = 0;
        if (b != 0 && data_0209f2f8 == 0x12) {
            unk_3ac = mPosY;
            unk_374 = mPosX;
            unk_378 = mPosY;
            unk_37c = mPosZ;
            func_ov090_02131e00((MenboState *)this, (MenboStateFn *)&data_ov090_021344f4);
            return 1;
        }
    }

    {
        dBgCh_Gnd ground;
        ground.mProbeHeight = 0xbb8000;
        ground.StartDetectingWater();
        {
            int py = mPosY;
            int pz = mPosZ;
            int px = mPosX;
            int ip = py + 0x32000;
            pos.x = px;
            pos.y = ip;
            pos.z = pz;
        }
        ground.SetObjAndPos(pos, this);
        unk_3a8 = data_02092138;
        if (ground.DetectClsn() != 0) {
            if (SurfaceInfo_TestFlag0x20(&ground.surface) != 0) {
                unk_39c = 1;
                unk_3ac = ground.clsnY;
            } else {
                unk_3a8 = ground.clsnY;
                unk_3ac = ground.clsnY;
            }
        }

        mPosY = unk_3ac;
        unk_374 = mPosX;
        unk_378 = mPosY;
        unk_37c = mPosZ;

        if (unk_39c != 0) {
            func_ov090_02131e00((MenboState *)this, (MenboStateFn *)&data_ov090_021344f4);
            return 1;
        }

        {
            r = RandomIntInternal(&data_0209e650);
            short ang = (short)((((unsigned int)r >> 8) & 0xf) << 12);
            *(short *)&unk_39a = ang;
            mPrevAngleY = *(short *)&unk_39a;
            mAngleY = mPrevAngleY;
        }
        func_ov090_02131e00((MenboState *)this, (MenboStateFn *)&data_ov090_021344e4);
    }

    return 1;
}

// @symbol _ZN9daMenbo_c16OnAimedAtWithEggEv
/* How far Yoshi's aim leads this target. */
s32 daMenbo_c::OnAimedAtWithEgg()
{
    return 0x20000;
}

// @symbol _ZN9daMenbo_c13OnTurnIntoEggER6Player
void daMenbo_c::OnTurnIntoEgg(Player &player)
{
    GivePlayerCoins(player, (unsigned char)(unk_10a + 1), 0);
    KillAndTrackInDeathTable();
}

// @symbol _ZN9daMenbo_c13OnYoshiTryEatEv
/* What Yoshi turns this into when he swallows it. */
s32 daMenbo_c::OnYoshiTryEat()
{
    return 4;
}
