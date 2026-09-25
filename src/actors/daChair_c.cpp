//cpp
/**
 * daChair_c -- the haunted chair of Big Boo's Haunt (CHAIR 326), ov020.
 *
 * ov020 is BBH's flying furniture: BOOK_SHOT / CHAIR / BOOKEND /
 * BOOK_SHOT_SPAWNER. RTTI ov020:0x021149d8 names this class daChair_c.
 * State0 looks up actor ID 0xf9 (PIANO) and stores its uniqueID; the chair
 * then hunts that piano or the player, rattling in place (State1) before it
 * moves.
 *
 * DO NOT "TIDY" THESE -- each one is load-bearing:
 *
 *   common.h FIRST: InitResources assigns IDENTITY_MATRIX4X3 into
 *   mShadowMat. common.h's flat s32 m[12] is the ROM's twelve-word copy;
 *   math/Matrix.h's nested {Matrix3x3 r; Vector3 t;} scalarizes it (Vector3
 *   is non-POD).
 *
 *   State1's LAUNDER((int)p) and State2's `(int)c + 0x39e`. The named
 *   mStateTimer++ and &mPosX do not match.
 *
 *   State2's State300 overlay at this+0x300 is this class's own
 *   mStateTimer / mActionTimer (0x39e / 0x3a0), reached through the
 *   dBgCh_Actr interior at 0x300. Named fields do not match.
 *
 *   (Vector3 *)&mPosX: dActor_c has no Pos(). Player+0x6fb and Player+0xc
 *   (actorID == 0xbf PLAYER) stay offsets.
 *
 * WHY SOME CALLS ARE SPELLED AS MANGLED SYMBOLS:
 *   dCcAcPos_c::Init / dBgCh_Actr::Init / DropShadowRadHeight /
 *   Particle::System::NewSimple / Player::Hurt pass Fix12<int> by value
 *   (notes/mwccarm-codegen.md 6az); the header method form changes the code
 *   size. The dBgCh Init header's Fix12i mangles as int; the ROM's is
 *   Fix12<int>.
 *   GetWallResult / CopyNormalTo are not in dBgCh_Actr.h / SurfaceInfo.
 *
 * Known limits:
 *   func_0200f760 (cylinder flags vs Player+0x6fb) and func_0201267c (sound
 *   at mCamSpacePos) keep their linker names.
 *
 * NOT OWNED BY THIS TU: data_ov020_02114af0 is the BMD SharedFilePtr this
 * TU loads and releases; ov020's sinit constructs it as file ID 0x2d0. There
 * is no g_profile_CHAIR in this TU.
 */

#include "common.h"
#include "daChair_c.h"
#include "SharedFilePtr.h"

#pragma opt_common_subs off

extern SharedFilePtr data_ov020_02114af0;
extern Matrix4x3 IDENTITY_MATRIX4X3;

extern "C" {
void func_0200f760(char *self, char *cyl);
void func_0201267c(unsigned int id, void *pos);
void dBgCh_Actr_UpdateContinuous_Veneer(void *);
void Vec3_Sub(void *res, void *v0, void *v1);
s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
s32 Vec3_HorzLen(const Vector3 *v0);
void *_ZNK10dBgCh_Actr13GetWallResultEv(void *);
void AddVec3(void *, void *, void *);
int Vec3_Dist(const Vector3 *a, const Vector3 *b);
short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
int AngleDiff(int a, int b);
void Matrix4x3_FromRotationZXYExt(void *m, int x, int y, int z);

void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *self, dActor_c *a, int r, int h, Vector3_16 *rot, Vector3_16 *f);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void *self, dActor_c *a, const Vector3 *pos, int r, int h, u32 f1, u32 f2);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *sm, void *m, int rad, int height, unsigned int flags);
void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned, int, int, int);
int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
    void *, const void *, u32, int, u32, u32, u32);

extern s16 data_02082214[];
}

#define LAUNDER(p) (p)

typedef struct { s32 x, y, z; } PlainVector3;

typedef struct {
    u8 _pad[0x9e];
    u16 counter;
    u16 timer;
} State300;

#define A92 (*(u16 *)((char *)c + 0x92))
#define A94 (*(u16 *)((char *)c + 0x94))
#define ST  ((State300 *)((char *)c + 0x300))

extern int _ZTV9daChair_c[];

// @symbol daChair_c_classInit
/* CHAIR registry factory. `return new daChair_c()` MATCHES; the synthesized
   ctor stores `_ZTV9daChair_c + 2` because this TU defines the vtable.
   classInit is reconstructed, not a preserved identifier. */
extern "C" daChair_c *daChair_c_classInit(void)
{
    return new daChair_c();
}

// @symbol _ZN9daChair_c13InitResourcesEv
s32 daChair_c::InitResources()
{
    BMD_File *f = (BMD_File *)Model::LoadFile(data_ov020_02114af0);
    mModel.SetFile(f, 1, -1);
    mShadowModel.InitCylinder();
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x32000, 0x64000, 0, 0);
    mClsnOffset.x = 0;
    mClsnOffset.y = 0;
    mClsnOffset.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mCylinder, this, &mClsnOffset, 0x32000, 0x64000, 0x200004, 0);
    mState = 0;
    mHomePos.x = mPosX;
    mHomePos.y = mPosY;
    mHomePos.z = mPosZ;
    *(Matrix4x3 *)(void *)&mShadowMat = IDENTITY_MATRIX4X3;
    return 1;
}

// @symbol _ZN9daChair_c8BehaviorEv
s32 daChair_c::Behavior()
{
    func_0200f760((char *)this, (char *)&mCylinder);
    switch (mState) {
    case 0: State0(); break;
    case 1: State1(); break;
    case 2: State2(); break;
    case 3: State3(); break;
    }
    UpdateModel();
    mCylinder.Clear();
    mCylinder.SetPosRelativeToActor(mClsnOffset);
    mCylinder.Update();
    return 1;
}

// @symbol _ZN9daChair_c6RenderEv
s32 daChair_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN9daChair_c16CleanupResourcesEv
s32 daChair_c::CleanupResources()
{
    data_ov020_02114af0.Release();
    return 1;
}

// @symbol _ZN9daChair_c11UpdateModelEv
void daChair_c::UpdateModel()
{
    char *c = (char *)this;
    Matrix4x3_FromRotationZXYExt(c + 0xf0, mAngleX,
                                 mAngleY, mAngleZ);
    *(int *)(c + 0x114) = mPosX >> 3;
    *(int *)(c + 0x118) = mPosY >> 3;
    *(int *)(c + 0x11c) = mPosZ >> 3;
    *(int *)(c + 0x170) = mPosX >> 3;
    *(int *)(c + 0x174) = *(int *)(c + 0x384) >> 3;
    *(int *)(c + 0x178) = mPosZ >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x124, c + 0x14c, 0x32000, 0x1e000, 0xf);
}

// @symbol _ZN9daChair_c5BreakEv
void daChair_c::Break()
{
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x25, mPosX, mPosY, mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x26, mPosX, mPosY, mPosZ);
    PoofDust();
    MarkForDestruction();
}

// @symbol _ZN9daChair_c18ApproachStateValueEPsS0_isis
int daChair_c::ApproachStateValue(s16 *pos, s16 *vel, s32 target,
                                  s16 thresh, s32 accel, s16 mult)
{
    short old = pos[0];
    pos[0] = old + vel[0];
    short now = pos[0];
    if (now == target
        || ((now - target) * (old - target) < 0
            && vel[0] > -thresh && vel[0] < thresh)) {
        pos[0] = target;
        vel[0] = 0;
        return 1;
    }
    if (now >= target)
        accel = (short)-accel;
    if ((short)vel[0] * (short)accel < 0)
        accel = (short)accel * (short)mult;
    vel[0] = vel[0] + accel;
    return 0;
}

// @symbol _ZN9daChair_c6State0Ev
void daChair_c::State0()
{
    char *c = (char *)this;
    mState = 1;
    mActionTimer = 1;
    mTargetAngle = 0;
    mStateValue0 = 0;
    mStateValue1 = 0;
    mStateValue2 = 0;
    /* 0xf9 is PIANO (249). This chair arms itself off the Mad Piano. */
    void *found = dActor_c::FindWithActorID(0xf9, 0);
    mTargetID = 0;
    if (!found)
        return;
    Fix12i d = Vec3_Dist((Vector3 *)(c + 0x5c),
                         (Vector3 *)((char *)found + 0x5c));
    if (d < 0x12c000) {
        mTargetID = *(int *)((char *)found + 4);
        mActionTimer = 0;
    }
}

// @symbol _ZN9daChair_c6State1Ev
void daChair_c::State1()
{
    char *self = (char *)this;
    int targetPos[4];
    void *target;
    short d;
    void *player;
    int wobble;
    int *av;
    unsigned short *p39e;

    target = dActor_c::FindWithID(mTargetID);
    if (target != 0) {
        av = (int *)(int)LAUNDER((char *)target + 0x5c);
        targetPos[0] = *av;
        targetPos[1] = av[1];
        targetPos[2] = av[2];
        if (Vec3_Dist((const Vector3 *)(self + 0x5c), (const Vector3 *)targetPos) >= 0xfa000)
            return;
        d = (short)(Vec3_HorzAngle((const Vector3 *)(self + 0x5c), (const Vector3 *)targetPos)
                    - mAngleY + 0x2000);
        if (d & 0x4000) {
            *(char **)(self + 0x3a4) = self + 0x90;
            if (d > 0)
                mTargetAngle = 0x4000;
            else
                mTargetAngle = (short)-0x4000;
        } else {
            *(char **)(self + 0x3a4) = self + 0x8c;
            if (d < 0)
                mTargetAngle = 0x5800;
            else
                mTargetAngle = (short)-0x4000;
        }
        if (mTargetAngle < 0)
            mStateValue0 = (short)0xfa24;
        else
            mStateValue0 = 0x5dc;
        mState = 3;
        return;
    }

    if (mActionTimer != 0) {
        player = ClosestPlayer();
        if (player != 0) {
            if (Vec3_Dist((const Vector3 *)(self + 0x5c),
                          (const Vector3 *)((char *)player + 0x5c)) < 0x1f4000)
                mActionTimer = 0;
        }
        mStateTimer = 0;
        return;
    }

    p39e = (unsigned short *)(int)LAUNDER(self + 0x39e);
    *p39e = (unsigned short)(*p39e + 1);
    if (mStateTimer & 8) {
        if (mAngleX >= 0) {
            wobble = -4;
        } else {
            func_0201267c(0x5f, self + 0x74);
            wobble = 4;
        }
        {
            int *px = (int *)(int)LAUNDER(self + 0x5c);
            *px = *px - (wobble << 12);
        }
        {
            int *pz = (int *)(int)LAUNDER(self + 0x64);
            *pz = *pz - (wobble << 12);
        }
        mAngleZ = (short)(wobble * 0x32);
        mAngleX = mAngleZ;
    } else {
        mAngleZ = 0;
        mAngleX = mAngleZ;
    }

    if (mStateTimer < 0x1e)
        return;
    mState = 2;
    mStateValue0 = 0;
    mStateValue1 = 0;
    mStateValue2 = 0xc8;
    mActionTimer = 0x28;
    mStateTimer = 0;
}

// @symbol _ZN9daChair_c6State2Ev
void daChair_c::State2()
{
    char *c = (char *)this;
    dBgCh_Actr_UpdateContinuous_Veneer(c + 0x1bc);
    (*(u16 *)(((int)c + 0x39e)))++;

    if (ST->counter <= 0x46) {
        if (ST->counter < 0x32)
            *(s32 *)(c + 0xa8) = 0x6000;
        else
            *(s32 *)(c + 0xa8) = 0;
        ApproachStateValue((short *)(c + 0x8c), (short *)(c + 0x398),
                           -0xfa0, 0xc8, 0x14, 2);
        ApproachStateValue((short *)(c + 0x90), (short *)(c + 0x39c),
                           0, 0, 0x14, 1);
        if (ST->counter == 0x46) {
            func_0201267c(0x5c, c + 0x74);
        }
    } else {
        if (ST->timer != 0) {
            (*(u16 *)(((int)c + 0x3a0)))--;
            if (ST->timer == 0) {
                void *player;
                func_0201267c(0x5d, c + 0x74);
                player = ClosestPlayer();
                if (player == 0)
                    return;
                {
                    PlainVector3 *pp = (PlainVector3 *)(((int)player + 0x5c));
                    PlainVector3 playerPos;
                    PlainVector3 diff;
                    s32 r;
                    playerPos.x = pp->x;
                    playerPos.y = pp->y;
                    playerPos.z = pp->z;
                    Vec3_Sub(&diff, &playerPos, (PlainVector3 *)(c + 0x5c));
                    mPrevAngleY = _ZN4cstd5atan2E5Fix12IiES1_(diff.x, diff.z);
                    mPrevAngleX = _ZN4cstd5atan2E5Fix12IiES1_(diff.y, Vec3_HorzLen((const Vector3 *)&diff)) * -1;
                    r = (s32)(((long long)data_02082214[(A92 >> 4) * 2 + 1] * 0x32000 + 0x800) >> 12);
                    *(s32 *)(c + 0xa4) = (s32)(((long long)r * data_02082214[(A94 >> 4) * 2] + 0x800) >> 12);
                    *(s32 *)(c + 0xa8) = (s32)(((long long)data_02082214[(A92 >> 4) * 2] * -0x32000 + 0x800) >> 12);
                    *(s32 *)(c + 0xac) = (s32)(((long long)r * data_02082214[(A94 >> 4) * 2 + 1] + 0x800) >> 12);
                }
            } else {
                if (ST->timer > 0x14) {
                    *(s16 *)(((int)c + 0x8e)) += 0x2710;
                }
            }
        } else {
            if (mWithMeshClsn.IsOnGround() != 0) {
                Break();
            } else if (mWithMeshClsn.IsOnWall() != 0) {
                void *wr = _ZNK10dBgCh_Actr13GetWallResultEv(c + 0x1bc);
                PlainVector3 normal;
                ((SurfaceInfo *)((char *)wr + 4))->CopyNormalTo(*(Vector3 *)&normal);
                if (GetSubtraction(mPrevAngleY,
                        _ZN4cstd5atan2E5Fix12IiES1_(normal.x, normal.z)) > 0x4000) {
                    Break();
                }
            }
        }
    }

    AddVec3(c + 0x5c, c + 0xa4, c + 0x5c);

    {
        u32 id = *(u32 *)(c + 0x1a0);
        void *a;
        int b;
        if (id == 0)
            return;
        a = dActor_c::FindWithID(id);
        if (a == 0)
            return;
        /* 0xbf is PLAYER (191). */
        b = (int)(*(u16 *)((char *)a + 0xc) == 0xbf);
        if (b == 0)
            return;
        b = *(u8 *)((char *)a + 0x6fb);
        if (b != 0)
            return;
        {
            PlainVector3 pos;
            pos.x = mPosX;
            pos.y = mPosY;
            pos.z = mPosZ;
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &pos, 2, 0xc000, 1, 0, 1);
        }
    }
}

// @symbol _ZN9daChair_c6State3Ev
void daChair_c::State3()
{
    char *c = (char *)this;
    int *p3a4 = *(int **)(c + 0x3a4);
    ApproachStateValue((short *)p3a4, (short *)(c + 0x398),
                       mTargetAngle, 0xfa0, 0x14, 2);
    {
        int a = AngleDiff(*(short *)*(int **)(c + 0x3a4), 0);
        int *ip = *(int **)(c + 0x3a4);
        short t = data_02082214[((unsigned short)*(unsigned short *)ip >> 4) * 2];
        short sa = (short)a;
        int m = (short)t * (short)0x50;
        if (ip != (int *)(c + 0x8c)) {
            *(int *)(c + 0x38c) = -m;
            *(int *)(c + 0x390) = 0;
            *(int *)(c + 0x394) = 0;
        } else {
            *(int *)(c + 0x38c) = 0;
            *(int *)(c + 0x390) = 0;
            *(int *)(c + 0x394) = m;
            goto cont;
        }
cont: ;
        if (sa >= 0x4000) {
            short d = (short)(sa - 0x4000);
            short t2 = data_02082214[((unsigned short)d >> 4) * 2];
            mPosY = (short)t2 * (short)0x28 + (*(int *)(c + 0x384) + 0x28000);
        } else {
            short t3 = data_02082214[((unsigned short)sa >> 4) * 2];
            mPosY = (short)t3 * (short)0x28 + *(int *)(c + 0x384);
        }
    }
}
