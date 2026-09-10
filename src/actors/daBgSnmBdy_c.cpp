//cpp
/* Reconstructed ov072/daBgSnmBdy_c translation unit -- the big rolling
 * snowman's body actor, under the name the cartridge's own RTTI gives it.
 *
 * The RTTI-backed class run is 0x0211f000..0x0211fedc (28 functions). The
 * registry-backed factory at 0x0211fedc allocates exactly
 * sizeof(daBgSnmBdy_c), installs this class's vtable, constructs its five
 * typed subobjects, and ends exactly at the next class's D1 at 0x0211ff34.
 * That direct registry/factory evidence supports testing the combined
 * 29-function TU; build/tu_map.json alone stops at 0x0211fedc and does not
 * prove the join.
 *
 * mwccarm emits ordinary function sections in reverse source order, so this
 * file is written highest-ROM-address first: the factory leads and the
 * destructor -- inline and declared last in daBgSnmBdy_c -- emits the retail
 * D1/D0 pair at the bottom of the run with no D2 body.
 *
 * The class-local operator new routes an ordinary `new daBgSnmBdy_c()` to the
 * retail fBase_c allocator, so the compiler emits and owns the vptr store; no
 * `_ZTV` address is spelled by hand anywhere in this file.
 */

#include "daBgSnmBdy_c.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"
#include "Player.h"

/* Shared ABI seams, kept above the first `// @symbol` marker so no member is
 * charged with their mangled spellings (notes/tu-promotion-conventions.md
 * sec 6). Each is the most complete of the spellings the retired one-function
 * shards carried. */
extern "C" {
int   Vec3_Dist(const void *a, const void *b);
int   Vec3_HorzDist(const void *a, const void *b);
short Vec3_HorzAngle(const void *a, const void *b);
void  Vec3_Asr(void *dst, const void *src, int shift);
int   _Z11UpdateAngleRssis(short *angle, short target, int shift, short maxStep);
void  _Z14ApproachLinearRiii(int *value, int target, int step);
unsigned short DecIfAbove0_Short(unsigned short *timer);
int   _ZN4cstd4fdivEii(int a, int b);
void  Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
void  func_0201267c(int id, void *pos);
void  func_0203568c(int *p, int v);
void  func_02035684(int *p, int v);
void  dBgCh_Actr_UpdateContinuous_Veneer(void *self);

dActor_c *_ZN8dActor_c10FindWithIDEj(u32 id);
void     *_ZN8dActor_c15FindWithActorIDEjPS_(u32 id, void *after);
void      _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void *self, int a, int b,
                                                    int c, int d);
void      _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *cc);
void      _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *self,
                                                       const Vector3 *pos,
                                                       int strength);
void      _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
              void *self, void *shadow, void *matrix, int radius, int height,
              u32 flags);

void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(dActor_c *player,
                                             const Vector3 *pos, u32 a,
                                             int fix, u32 b, u32 c, u32 d);
int  _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(int player, void *actor,
                                                    int msg, void *pos, int a,
                                                    int b);
int  _ZN6Player12GetTalkStateEv(void *player);

int  _ZNK7PathPtr7GetNodeER7Vector3j(void *self, void *out, u32 node);
int  _ZNK7PathPtr8NumNodesEv(void *self);
void _ZN7PathPtr6FromIDEj(void *self, u32 id);

int   _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
int   _ZNK10dBgCh_Actr13JustHitGroundEv(void *self);
char *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);
void  _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
          void *self, void *actor, int a, int b, void *v, int c);
void  _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *self, Vector3 *out);
void  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor, int a,
                                                int b, u32 c, u32 d);

void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int x, int y, int z);
u32  _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 handle, u32 a, u32 b,
                                       const Vector3 *pos, u32 e);
int  _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(
         void *clipper, void *matrix, void *pos, int radius, void *result);

/* ov072 and arm9 statics this TU reads but does not own. */
extern SharedFilePtr data_ov072_02122b20;
extern Vector3       data_ov072_02122b40;
extern Vector3       data_ov072_02122b58;
extern daBgSnmBdy_c::StateFunc data_ov072_02122b64[];
extern int           data_0209f43c[];
extern int           data_0209b3ec[];
}

extern Matrix4x3 IDENTITY_MATRIX4X3;

/* The typed 0x1c actor profile: fBase_c reads the halfwords at +4/+6 as
 * behavior/render priorities. dActor_c reads actor flags at +8 and passes
 * the words at +0xc/+0x10/+0x14/+0x18 to SetRanges as clip offset Y, clip
 * radius, clip distance and far distance. These field names follow those
 * consumers; the existing widths, types and retail values are unchanged. */
struct SnmBdyProfile {
    daBgSnmBdy_c *(*classInit)();
    s16 behaviorPriority;
    s16 renderPriority;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    u32 clipDistance;
    u32 farDistance;
};

typedef char SnmBdyProfile_size_must_be_0x1c[
    sizeof(SnmBdyProfile) == 0x1c ? 1 : -1];

/* Reconstructed source-style names. SM64DS preserves this class's RTTI, the
 * BIG_SNOWMAN_BODY registry ID, the descriptor relationship and the factory's
 * behavior; it preserves neither identifier below. */
// @symbol daBgSnmBdy_c_classInit
extern "C" daBgSnmBdy_c *daBgSnmBdy_c_classInit()
{
    return new daBgSnmBdy_c();
}

extern "C" SnmBdyProfile g_profile_BIG_SNOWMAN_BODY = {
    daBgSnmBdy_c_classInit,
    0x0112,
    0x0085,
    2,
    0x82000,
    0x82000,
    0x01000000,
    0x01000000
};

// @symbol _ZN12daBgSnmBdy_c13InitResourcesEv
int daBgSnmBdy_c::InitResources()
{
    struct MatrixWords { int words[12]; };
    Vector3 pos;
    void *file = Model::LoadFile(data_ov072_02122b20);
    mModel.SetFile((BMD_File *)file, 1, 1);
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char *)this) + 0x14c,
        ((char *)this), 0x82000, 0x104000, 0x800004, 0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        ((char *)this) + 0x180, ((char *)this), 0x82000, 0x82000, 0, 0);
    {
        int p60;
        pos.x = mPosX;
        p60 = mPosY;
        pos.y = p60;
        pos.z = mPosZ;
        pos.y = p60 + 0x14000;
    }
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn())
        mPosY = ground.clsnY;
    else
        mPosY = pos.y;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mHomeAngleX = mAngleX;
    mHomeAngleY = mAngleY;
    mHomeAngleZ = mAngleZ;
    unk_3a4 = 0x5a;
    SetState(0);
    mTalkPlayer = 0;
    *(MatrixWords *)((char *)&mShadowMat) = *(MatrixWords *)&IDENTITY_MATRIX4X3;
    UpdateModel();
    _ZN7PathPtr6FromIDEj(((char *)this) + 0x380, param1 & 0xff);
    return 1;
}

/* dCc_c comes from the real dCcAc_c chain now that the header types
   mCylinder; Clear and Update are non-virtual there, so the direct bl is
   unchanged. */
// @symbol _ZN12daBgSnmBdy_c8BehaviorEv
int daBgSnmBdy_c::Behavior()
{
    CallStateBehavior();
    mCylinder.Clear();
    mCylinder.Update();
    UpdateModel();
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c6RenderEv
int daBgSnmBdy_c::Render()
{
    mModel.Render((Vector3 *)&mScaleX);
    return 1;
}

/* Vtable slot 12. The ROM body is empty: the override exists only to occupy
 * the slot. */
// @symbol _ZN12daBgSnmBdy_c16OnPendingDestroyEv
void daBgSnmBdy_c::OnPendingDestroy()
{
}

/* Vtable slot 3. Releases the one shared file the class holds; it never
 * touches `this`. */
// @symbol _ZN12daBgSnmBdy_c16CleanupResourcesEv
int daBgSnmBdy_c::CleanupResources()
{
    data_ov072_02122b20.Release();
    return 1;
}

/* SetState and its int parameter are inferred spellings. The ROM proves
 * the table indexing, not the original identifier or int-versus-enum type. */
// @symbol _ZN12daBgSnmBdy_c8SetStateEi
void daBgSnmBdy_c::SetState(int state)
{
    mStateFuncs = data_ov072_02122b64 + state * 2;
    CallStateInit();
}

// @symbol _ZN12daBgSnmBdy_c13CallStateInitEv
void daBgSnmBdy_c::CallStateInit()
{
    StateFunc *func = mStateFuncs;
    (this->**func)();
}

// @symbol _ZN12daBgSnmBdy_c17CallStateBehaviorEv
void daBgSnmBdy_c::CallStateBehavior()
{
    StateFunc *func = mStateFuncs + 1;
    (this->**func)();
}

// @symbol _ZN12daBgSnmBdy_c10InitState0Ev
int daBgSnmBdy_c::InitState0()
{
    char *c = (char *)this;
    int *p;
    *(int *)(c + 0x9c) = 0;
    *(int *)(c + 0xa0) = 0;
    *(int *)(c + 0x80) = 0x800;
    *(int *)(c + 0x84) = 0x800;
    *(int *)(c + 0x88) = 0x800;
    *(int *)(c + 0x398) = (int)(((long long)(*(int *)(c + 0x80)) *
                                        0x82000 + 0x800) >> 12);
    *(int *)(c + 0x150) = *(int *)(c + 0x398);
    *(int *)(c + 0x154) = *(int *)(c + 0x398) << 1;
    func_0203568c((int *)(c + 0x180), *(int *)(c + 0x398));
    func_02035684((int *)(c + 0x180), *(int *)(c + 0x398));
    _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(
        c, *(int *)(c + 0x398), *(int *)(c + 0x398), 0x1000000, 0x1000000);
    p = (int *)(((int)c + 0xb0));
    *p = *p | 1;
    *(int *)(c + 0x394) = 0;
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c6State0Ev
int daBgSnmBdy_c::State0()
{
    char *c = (char *)this;
    Player *player = ClosestPlayer();
    char *p = (char *)player;
    if (Vec3_HorzDist(c + 0x5c, p + 0x5c) < 0x10e000) {
        if (((Player *)player)->StartTalk(*(fBase_c *)this, 1)) {
            *(Player **)(c + 0x390) = player;
            SetState(1);
        }
    }
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c10InitState1Ev
int daBgSnmBdy_c::InitState1()
{
    mSubstate = 0;
    mStateTimer = 0x15;
    mStateValue = 1;
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c6State1Ev
int daBgSnmBdy_c::State1()
{
    char *c = (char *)this;
    int v[3];
    unsigned char *state;
    v[0] = *(int *)(c + 0x5c);
    int y = *(int *)(c + 0x60);
    v[1] = y;
    v[2] = *(int *)(c + 0x64);
    v[1] = y + 0x96000;
    switch (*(unsigned char *)(c + 0x3a2)) {
    case 0:
        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
                *(int *)(c + 0x390), c, 0xb0, v, 0, 0) == 0)
            break;
        state = (unsigned char *)(((int)c + 0x3a2));
        *state = *state + 1;
        break;
    case 1:
        if (_ZN6Player12GetTalkStateEv((void *)*(int *)(c + 0x390)) != -1)
            break;
        state = (unsigned char *)(((int)c + 0x3a2));
        *state = *state + 1;
        break;
    case 2:
        if (DecIfAbove0_Short((unsigned short *)(c + 0x3a0)) == 0)
            SetState(2);
        break;
    }
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c10InitState2Ev
int daBgSnmBdy_c::InitState2()
{
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mFlags &= ~1;
    mPathNode = 0;
    mHorzSpeed = 0;
    mStateValue = 2;
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c6State2Ev
int daBgSnmBdy_c::State2()
{
    char *c = (char *)this;
    int t;
    int v;
    _Z14ApproachLinearRiii((int *)(c + 0x98), 0x28000, 0x400);
    if (DecIfAbove0_Short((unsigned short *)(c + 0x3a0)) == 0) {
        t = *(int *)(c + 0x80);
        _Z14ApproachLinearRiii(&t, 0x1800, 0x11);
        v = t;
        *(int *)(c + 0x80) = v;
        *(int *)(c + 0x84) = v;
        *(int *)(c + 0x88) = v;
        *(int *)(c + 0x398) =
            (int)(((long long)t * 0x82000 + 0x800) >> 12);
        *(int *)(c + 0x150) = *(int *)(c + 0x398);
        *(int *)(c + 0x154) = *(int *)(c + 0x398) << 1;
        func_0203568c((int *)(c + 0x180), *(int *)(c + 0x398));
        func_02035684((int *)(c + 0x180), *(int *)(c + 0x398));
        _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(
            c, *(int *)(c + 0x398), *(int *)(c + 0x398),
            0x1000000, 0x1000000);
    }
    if (AdvancePath() != 0) {
        if (*(unsigned char *)(c + 0x3a3) != 0 &&
            IsPlayerNearCenter() != 0)
            SetState(3);
        else
            SetState(4);
    }
    _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(
        *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
    *(int *)(c + 0x39c) = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        *(unsigned *)(c + 0x39c), 3, 0x8a,
        (const Vector3 *)(c + 0x74), 0);
    UpdateRollAngle();
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x14c);
    UpdateGroundCollision((dBgCh_Actr *)(c + 0x180));
    HurtPlayer();
    if (*(unsigned char *)(c + 0x3a3) == 0) {
        if (Vec3_Dist(&data_ov072_02122b40,
                      *(char **)(c + 0x390) + 0x5c) < 0x300000)
            *(unsigned char *)(c + 0x3a3) = 1;
    }
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c10InitState3Ev
int daBgSnmBdy_c::InitState3()
{
    mSubstate = 0;
    mStateValue = 3;
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c6State3Ev
int daBgSnmBdy_c::State3()
{
    unsigned char *thiz = (unsigned char *)this;
    unsigned char *state;
    switch (thiz[0x3a2]) {
    case 0:
        {
            int d = Vec3_HorzDist(&data_ov072_02122b58, thiz + 0x5c);
            _Z11UpdateAngleRssis((short *)(thiz + 0x8e),
                Vec3_HorzAngle(thiz + 0x5c, &data_ov072_02122b58),
                2, 0x600);
            *(short *)(thiz + 0x94) = *(short *)(thiz + 0x8e);
            _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(
                *(int *)(thiz + 0x5c), *(int *)(thiz + 0x60),
                *(int *)(thiz + 0x64));
            *(unsigned int *)(thiz + 0x39c) =
                _ZN5Sound8PlayLongEjjjRK7Vector3s(
                    *(unsigned int *)(thiz + 0x39c), 3, 0x8a,
                    (const Vector3 *)(thiz + 0x74), 0);
            if (d < 0x17c000) {
                void *actor = _ZN8dActor_c15FindWithActorIDEjPS_(0x111, 0);
                *(unsigned char *)((unsigned char *)actor + 0x336) = 1;
                func_0201267c(0x114, thiz + 0x74);
                *(int *)(thiz + 0xa8) = 0x1d000;
                *(int *)(thiz + 0x98) = 0xe000;
                state = (unsigned char *)(((int)thiz + 0x3a2));
                *state = *state + 1;
            }
        }
        break;
    case 1:
        if (_ZNK10dBgCh_Actr13JustHitGroundEv(thiz + 0x180) != 0) {
            Vector3 v;
            v.x = *(int *)(thiz + 0x5c);
            v.y = *(int *)(thiz + 0x60);
            v.z = *(int *)(thiz + 0x64);
            _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(
                thiz, &v, 0x5dc000);
            *(int *)(thiz + 0x5c) = data_ov072_02122b58.x;
            *(int *)(thiz + 0x60) = data_ov072_02122b58.y;
            *(int *)(thiz + 0x64) = data_ov072_02122b58.z;
            *(short *)(thiz + 0x8c) = 0;
            *(short *)(thiz + 0x8e) = (short)-0x4000;
            *(int *)(thiz + 0x98) = 0;
            state = (unsigned char *)(((int)thiz + 0x3a2));
            *state = *state + 1;
        }
        break;
    case 2:
    default:
        break;
    }

    UpdateRollAngle();
    _ZN8dActor_c9UpdatePosEP5dCc_c(thiz, thiz + 0x14c);
    UpdateGroundCollision((dBgCh_Actr *)(thiz + 0x180));
    HurtPlayer();
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c10InitState4Ev
int daBgSnmBdy_c::InitState4()
{
    mFlags &= ~1;
    mStateValue = 4;
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c6State4Ev
int daBgSnmBdy_c::State4()
{
    char *c = (char *)this;
    _Z14ApproachLinearRiii((int *)(c + 0x98), 0x28000, 0x400);
    _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(
        *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
    *(int *)(c + 0x39c) = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        *(unsigned int *)(c + 0x39c), 3, 0x8a,
        (const Vector3 *)(c + 0x74), 0);
    UpdateRollAngle();
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x14c);
    UpdateGroundCollision((dBgCh_Actr *)(c + 0x180));
    HurtPlayer();
    if (*(int *)(c + 0x60) < (int)0xfe363c80)
        SetState(5);
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c10InitState5Ev
int daBgSnmBdy_c::InitState5()
{
    mFlags &= ~1;
    mStateValue = 5;
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c6State5Ev
int daBgSnmBdy_c::State5()
{
    char *c = (char *)this;
    int b = (int)((*(int *)(c + 0xb0) & 8) != 0);
    if (b == 0) return 1;
    int sp4[3];
    int v[3];
    Vec3_Asr(v, c + 0x33c, 3);
    if (_ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(
            data_0209f43c, data_0209b3ec, v, 0x1f400, sp4) < 0x1194000)
        return 1;
    *(int *)(c + 0x5c) = *(int *)(c + 0x33c);
    *(int *)(c + 0x60) = *(int *)(c + 0x340);
    *(int *)(c + 0x64) = *(int *)(c + 0x344);
    *(short *)(c + 0x8c) = *(short *)(c + 0x348);
    *(short *)(c + 0x8e) = *(short *)(c + 0x34a);
    *(short *)(c + 0x90) = *(short *)(c + 0x34c);
    *(short *)(c + 0x92) = *(short *)(c + 0x8c);
    *(short *)(c + 0x94) = *(short *)(c + 0x8e);
    *(short *)(c + 0x96) = *(short *)(c + 0x90);
    *(int *)(c + 0x390) = 0;
    SetState(0);
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c11UpdateModelEv
void daBgSnmBdy_c::UpdateModel()
{
    char *c = (char *)this;
    Matrix4x3_FromRotationXYZExt(c + 0xf0, *(s16 *)(c + 0x8c),
                                *(s16 *)(c + 0x8e), *(s16 *)(c + 0x90));
    *(int *)(c + 0x114) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x118) = (*(int *)(c + 0x60) + *(int *)(c + 0x398)) >> 3;
    *(int *)(c + 0x11c) = *(int *)(c + 0x64) >> 3;
    *(int *)(c + 0x374) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x378) = (*(int *)(c + 0x60) + *(int *)(c + 0x398)) >> 3;
    *(int *)(c + 0x37c) = *(int *)(c + 0x64) >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x124, c + 0x350, *(int *)(c + 0x398) << 1,
        *(int *)(c + 0x398) << 1, 0xf);
}

// @symbol _ZN12daBgSnmBdy_c21UpdateGroundCollisionEP10dBgCh_Actr
// The P10dBgCh_Actr in that spelling asserts a POINTER parameter. The bytes
// cannot distinguish a pointer from a reference here -- R10dBgCh_Actr would have
// matched equally well -- so the parameter type is a disclosed guess.
void daBgSnmBdy_c::UpdateGroundCollision(dBgCh_Actr *mc)
{
    char *c = (char *)this;
    Vector3 n;
    char *fr;
    dBgCh_Actr_UpdateContinuous_Veneer(mc);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(mc) == 0) return;
    fr = _ZNK10dBgCh_Actr14GetFloorResultEv(mc);
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(fr + 4, &n);
    if (n.y == 0) return;
    {
        int a = (int)(((long long)n.x * *(int *)(c + 0xa4) + 0x800) >> 12);
        int b = (int)(((long long)n.z * *(int *)(c + 0xac) + 0x800) >> 12);
        *(int *)(c + 0xa8) = -(_ZN4cstd4fdivEii(a + b, n.y) + 0x8000);
    }
}

// @symbol _ZN12daBgSnmBdy_c10HurtPlayerEv
int daBgSnmBdy_c::HurtPlayer()
{
    char *self = (char *)this;
    dActor_c *actor;
    u32 id;
    int t;
    Vector3 pos;

    id = *(u32 *)(self + 0x170);
    if (id == 0) return 0;
    actor = _ZN8dActor_c10FindWithIDEj(id);
    if (actor == 0) goto fail;
    t = (int)(*(u16 *)((char *)actor + 0xc) == 0xbf);
    if (t != 0) goto body;
fail:
    return 0;
body:
    if (*(u32 *)(self + 0x98) != 0) {
        pos.x = *(int *)(self + 0x5c);
        pos.y = *(int *)(self + 0x60);
        pos.z = *(int *)(self + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
            actor, &pos, 2, 0xc000, 1, 0, 1);
    }
    return 1;
}

// @symbol _ZN12daBgSnmBdy_c11AdvancePathEv
int daBgSnmBdy_c::AdvancePath()
{
    char *c = (char *)this;
    int v[3];
    int *idx;
    int n;
    _ZNK7PathPtr7GetNodeER7Vector3j(c + 0x380, v, *(int *)(c + 0x388));
    int d = Vec3_HorzDist(c + 0x5c, v);
    _Z11UpdateAngleRssis((short *)(c + 0x8e),
                        Vec3_HorzAngle(c + 0x5c, v), 2, 0x600);
    *(short *)(c + 0x94) = *(short *)(c + 0x8e);
    if (d < *(int *)(c + 0x398)) {
        n = _ZNK7PathPtr8NumNodesEv(c + 0x380);
        idx = (int *)(((int)c + 0x388));
        *idx = *idx + 1;
        if (*(int *)(c + 0x388) >= n - 1) return 1;
    }
    return 0;
}

// @symbol _ZN12daBgSnmBdy_c15UpdateRollAngleEv
void daBgSnmBdy_c::UpdateRollAngle()
{
    char *c = (char *)this;
    int d = (int)(((long long)(*(int *)(c + 0x398) << 1) *
                       0x3243F6A89LL + 0x80000000LL) >> 32);
    Fix12i q = _ZN4cstd4fdivEii(*(int *)(c + 0x98), d);
    *(short *)(c + 0x8c) = (short)(*(short *)(c + 0x8c) +
        (int)(((long long)q * 0xffff + 0x800) >> 12));
}

// @symbol _ZN12daBgSnmBdy_c18IsPlayerNearCenterEv
int daBgSnmBdy_c::IsPlayerNearCenter()
{
    char *c = (char *)this;
    int d0 = Vec3_Dist(&data_ov072_02122b58, c + 0x5c);
    int d1 = Vec3_Dist(&data_ov072_02122b58,
                      (char *)(*(int *)(c + 0x390)) + 0x5c);
    int a0 = Vec3_HorzAngle(&data_ov072_02122b58, c + 0x5c);
    int a1 = Vec3_HorzAngle(&data_ov072_02122b58,
                           (char *)(*(int *)(c + 0x390)) + 0x5c);
    int sub = a0 - a1;
    if (d1 < d0) {
        short diff = (short)sub;
        if (diff < 0) diff = -diff;
        if (diff < 0x700) goto ret1;
    }
    if (Vec3_Dist(&data_ov072_02122b58,
                  (char *)(*(int *)(c + 0x390)) + 0x5c) >= 0x300000)
        goto ret0;
ret1:
    return 1;
ret0:
    return 0;
}
