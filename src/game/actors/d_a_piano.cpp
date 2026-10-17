//cpp
/* The Mad Piano (PIANO), ov063/daPiano_c in Big Boo's Haunt.
 * ROM RTTI names daPiano_c; g_profile_PIANO selects its factory, which
 * constructs the existing 0x6e4-byte class and its render/collision members.
 * Two PMF states search for a moving player and then lunge and bite.
 *
 * This promoted TU owns seventeen text functions, its initializer, ctor
 * word, four PMF descriptors, resource handles, destructor nodes and state
 * table. The manifest records their exact extents and external RTTI homes.
 * The inline destructor preserves the retail D1/D0 order.
 *
 * Remaining reconstruction (issue #2720):
 * - SetAnim, collider initialization, mesh SetFile and DropShadow calls
 *   retain their matched scalar ABI declarations. Native Fix12 interfaces
 *   remain separate work; prior exact probes are in piano-2712-*.md.
 * - common.h supplies the flat Matrix4x3 representation. Existing views
 *   across dActor_c scalar position fields remain; real local Vector3
 *   objects do not reconstruct those shared fields as aggregates.
 * - Address-named free helpers retain current configured linker names;
 *   original source names are unknown. Radius/height store veneers remain.
 * - Volatile position/angle reads and sine-table accesses retain their
 *   measured order. The CLPS payload and external g_profile_PIANO are not
 *   reconstructed here.
 * - func_0201267c is an enrolled Sound.cpp forwarder to Sound::Play;
 *   this TU retains its existing free-function call.
 */
#include "daPiano_c.h"
#include "daPiano_c_resources.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "nitro/fx/fx.h"

bool ApproachLinear(short &value, short target, short step);

/* The TU's three resource handles, defined at file end so mwcc emits their
 * constructor calls, destructor registrations, and the PMF-table copy into
 * __sinit_d_a_piano.cpp. Each flavor binds the constructor/destructor veneer
 * the cartridge uses for its asset kind (model / mesh collision / animation). */
struct PianoModelFilePtr : SharedFilePtr {
    u32 words[2];
    PianoModelFilePtr(u32 fileID);
    ~PianoModelFilePtr();
};
struct PianoCollisionFilePtr : SharedFilePtr {
    u32 words[2];
    PianoCollisionFilePtr(u32 fileID);
    ~PianoCollisionFilePtr();
};
struct PianoAnimationFilePtr : SharedFilePtr {
    u32 words[2];
    PianoAnimationFilePtr(u32 fileID);
    ~PianoAnimationFilePtr();
};

extern PianoModelFilePtr gPianoModelFile;
extern PianoCollisionFilePtr gPianoCollisionFile;
extern PianoAnimationFilePtr gPianoAttackAnimationFile;
extern char data_ov063_0211ecb8;

/* One file-scope extern "C" region: the union of the five legacy shards'
 * declarations, deduplicated. C++-named members cannot carry block-scope
 * linkage specifications, so everything they call lives here. */
struct Vec3;
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *, BCA_File *, int, int, unsigned int);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(dBgCh_Actr *, dActor_c *, Fix12i, Fix12i, Vector3_16 *, Vector3_16 *);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(dCcAcPos_c *, dActor_c *, const Vector3 *, Fix12i, Fix12i, unsigned int, unsigned int);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, void *, const Matrix4x3 *, int, short, void *);
extern void func_ov063_0211d88c(daPiano_c *self);
extern void func_ov063_0211d828(daPiano_c *self);
extern void func_ov063_0211d5f4(daPiano_c *self);
extern int func_ov063_0211dd84(int unused, dActor_c *cand);
extern void func_ov063_0211ddac(daPiano_c *c, int state);
extern void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
extern int LenVec3(Vec3 *v);
extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern u8 DecIfAbove0_Byte(u8 *p);
extern void func_0201267c(unsigned int id, const Vector3 *v);
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(Player *self, void *a, unsigned int b, int dmg, unsigned char arg4, unsigned char arg5, unsigned char arg6);
int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
void dBgCh_Actr_UpdateContinuous_Veneer(dBgCh_Actr *clsn);
void func_0203568c(int *clsn, int radius);
void func_02035684(int *clsn, int height);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *, int, int);
void func_ov063_0211ddf4(daPiano_c *self);
}

/* Reconstructed registry name; the class identity is backed by retail RTTI.
 * Historical alias: daPiano_c_Spawn. The inherited allocator and native
 * construction establish the base, vtable and member lifetimes. */
// @symbol daPiano_c_classInit
extern "C" daPiano_c *daPiano_c_classInit()
{
    return new daPiano_c();
}

/* Load the piano's three shared assets, initialize its typed render/collision
 * members, and snapshot its home position. Fix12-by-value APIs keep their
 * explicit ABI declarations because 2004/b56 homes the member form differently. */
// @symbol _ZN9daPiano_c13InitResourcesEv
int daPiano_c::InitResources()
{
    int i;
    dCcAcPos_c *cylinder;
    void *f;

    f = Model::LoadFile(gPianoModelFile);
    mModelAnim.SetFile((BMD_File *)f, 1, -1);
    mShadowModel1.InitCuboid();
    mShadowModel2.InitCuboid();
    mShadowModel3.InitCylinder();
    f = dExtFrameCtrl_c::LoadFile(gPianoAttackAnimationFile);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (BCA_File *)f, 0, 0x1000, 0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHomePos.x = mPosX;
    mHomePos.y = mPosY;
    mHomePos.z = mPosZ;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x159000, 0x159000, 0, 0);
    for (i = 0, cylinder = mCylinderClsn; i < 2; i++) {
        /* Sensor cylinders, 155.0 radius by 250.0 tall, tracking mPos.
         * 0x200004 is enemy | char-projectile: what can hit the piano.
         * Players are Hurt manually (func_ov063_0211d8cc), not via dCc. */
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(cylinder, this, &mPos, 0x9b000,
                                                                0xfa000, 0x200004, 0);
        cylinder++;
    }
    f = dBgW_Kc::LoadFile(gPianoCollisionFile);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, f, &mClsnMat, 0x199, mAngleY, &data_ov063_0211ecb8);
    func_ov063_0211d88c(this);
    func_ov063_0211d828(this);
    func_ov063_0211d5f4(this);
    return 1;
}

/* Advance the piano and its collision, constrain it to its home area, and
 * disable the inherited moving mesh while the nearest player is vanished. */
// @symbol _ZN9daPiano_c8BehaviorEv
int daPiano_c::Behavior()
{
    func_ov063_0211ddf4(this);
    UpdatePos(0);
    if (Vec3_HorzDist(&mHomePos, &mPos) > 0x180000) {
        mPosX = mPrevPosX;
        mPosY = mPrevPosY;
        mPosZ = mPrevPosZ;
    }
    if (mPosY <= mMinPosY)
        mPosY = mMinPosY;
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    func_0203568c((int *)&mWithMeshClsn, 0x159000);
    func_02035684((int *)&mWithMeshClsn, 0x159000);
    if (ClosestPlayer()->mIsVanish != 0) {
        if (mMeshCollider.IsEnabled())
            mMeshCollider.Disable();
    } else {
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x1f4000, 0);
    }
    func_ov063_0211d88c(this);
    func_ov063_0211d828(this);
    func_ov063_0211d5f4(this);
    return 1;
}

/* ModelAnim's real virtual API expresses the same slot-5 dispatch as the ROM. */
// @symbol _ZN9daPiano_c6RenderEv
int daPiano_c::Render()
{
    mModelAnim.Render(0);
    return 1;
}

/* Release the three shared assets after removing the inherited moving mesh
 * collider from the collision world. */
// @symbol _ZN9daPiano_c16CleanupResourcesEv
int daPiano_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    gPianoModelFile.Release();
    gPianoAttackAnimationFile.Release();
    gPianoCollisionFile.Release();
    return 1;
}

/* The piano's two-state dispatch table (data_ov063_0211efbc, filled by
 * __sinit_d_a_piano.cpp from the four ROM {ptr, adj} records): per state,
 * an entry routine and a body routine. Bound to daPiano_c itself. An earlier
 * draft routed this through a non-polymorphic shadow layout on the theory
 * that a PMF of the real class does not carry the ROM's plain {ptr, adj}
 * representation; that is false for this TU -- under 2004/b56 the real class
 * gives byte-identical bodies and relocation records for all 17 functions. */
typedef void (daPiano_c::*PianoPMF)();
struct PianoStateEntry { PianoPMF pmf[2]; };
extern PianoStateEntry data_ov063_0211efbc[2];
/* State 0 searches (body func_ov063_0211dbb8, entry func_ov063_0211dd78);
 * state 1 attacks (body func_ov063_0211d8cc, entry func_ov063_0211dba4).
 * Proven by who calls ddac with what and which routine runs per-frame. */
enum PianoState { kStateSearch = 0, kStateAttack = 1 };

extern "C" {
// @symbol func_ov063_0211ddf4
/* Run the current state's body routine (pmf[1]) every frame. */
void func_ov063_0211ddf4(daPiano_c *self)
{
    int cur = self->mStateIdx;
    (self->*data_ov063_0211efbc[cur].pmf[1])();
}
}

extern "C" {
// @symbol func_ov063_0211ddac
/* Switch to `state` and run its entry routine (pmf[0]) once. The idx
 * re-read after the store is ROM-true. */
void func_ov063_0211ddac(daPiano_c *self, int state)
{
    self->mStateIdx = state;
    int cur = self->mStateIdx;
    (self->*data_ov063_0211efbc[cur].pmf[0])();
}
}

extern "C" {
// @symbol func_ov063_0211dd84
/* True when the candidate target is moving (either speed above 20.0). The
 * ROM passes self as the first arg; it is ignored. */
int func_ov063_0211dd84(int unused, dActor_c *cand) {
    if (cand->mVertSpeed > 0x14000 || cand->mHorzSpeed > 0x14000) return 1;
    return 0;
}
}

// @symbol _ZN9daPiano_c19func_ov063_0211dd78Ev
/* State-0 entry, reached via the PMF table: stop dead. */
void daPiano_c::func_ov063_0211dd78()
{
    this->mHorzSpeed = 0;
}

#pragma opt_strength_reduction off
// @symbol _ZN9daPiano_c19func_ov063_0211dbb8Ev
/* State-0 body, run every frame: scan the player slots for a moving target
 * within 500.0 and attack it; otherwise refresh the idle cylinder sweep
 * (current position plus a 36.0 lookahead along the facing). */
void daPiano_c::func_ov063_0211dbb8() {
    extern unsigned char data_0209f21c;
    extern void* data_0209f394[];
    extern short data_02082214[];   /* sine table: (angle >> 4) * 2 taps sin/cos */

    volatile Vector3 tmp;
    Vector3 v;
    int bestDist;
    int i;
    void* cand;
    int dist;

    bestDist = 0x7fffffff;
    for (i = 0; i < data_0209f21c; i++) {
        cand = data_0209f394[i];
        if (cand == 0) continue;
        Vec3_Sub((Vec3*)&v, (Vec3*)&this->mPosX, (Vec3*)&((dActor_c*)cand)->mPosX);
        dist = LenVec3((Vec3*)&v);
        if (func_ov063_0211dd84((int)this, (dActor_c*)cand) != 0 && dist < 0x1f4000 && dist < bestDist) {
            this->mTarget = (dActor_c*)cand;
            func_ov063_0211ddac(this, kStateAttack);
            return;
        }
    }

    {
        int x = this->mPosX;
        int z;
        int j;
        dCcAcPos_c* cc;

        tmp.x = x;
        tmp.y = this->mPosY;
        z = this->mPosZ;
        tmp.z = z;
        {
            int sinVal = data_02082214[(*(volatile u16*)&this->mAngleY >> 4) * 2];
            tmp.x = x + FX_MUL(sinVal, 0x24000);
        }
        tmp.z = z + FX_MUL(data_02082214[(*(volatile u16 *)&this->mAngleY >> 4) * 2 + 1], 0x24000);

        this->mCylinderClsn[0].pos.x = *(volatile int*)&this->mPosX;
        this->mCylinderClsn[0].pos.y = *(volatile int*)&this->mPosY;
        this->mCylinderClsn[0].pos.z = *(volatile int*)&this->mPosZ;
        this->mCylinderClsn[1].pos.x = tmp.x;
        this->mCylinderClsn[1].pos.y = tmp.y;
        this->mCylinderClsn[1].pos.z = tmp.z;

        cc = this->mCylinderClsn;
        for (j = 0; j < 2; j++) {
            this->mCylinderClsn[j].radius = 0x9b000;
            cc->Clear();
            cc++;
        }
    }
}

// @symbol _ZN9daPiano_c19func_ov063_0211dba4Ev
/* State-1 entry, reached via the PMF table: full-speed animation and a
 * 30-frame windup before the bite. */
void daPiano_c::func_ov063_0211dba4()
{
    this->mModelAnim.speed = 4096;
    this->mAttackTimer = 30;
}

#pragma opt_strength_reduction off
// @symbol _ZN9daPiano_c19func_ov063_0211d8ccEv
/* State-1 body, run every frame: lunge at the target and bite any player
 * caught by the cylinders. Gives up (back to state 0) when the target is
 * gone or farther than 939.0 at a frame boundary. */
void daPiano_c::func_ov063_0211d8cc()
{
    extern s16 data_02082214[];   /* sine table: (angle >> 4) * 2 taps sin/cos */

    dActor_c* victim;
    int i;
    volatile Vector3 proj;
    Vector3 hurtPos;
    Vector3 tmp;
    int zero;
    int one;
    int three;
    int knock;
    int dist;
    dActor_c* target;
    int k;
    dCcAcPos_c* cc;
    int x;
    int z;

    if (DecIfAbove0_Byte(&this->mAttackTimer) != 0)
        return;

    this->mHorzSpeed = 0x5000;
    victim = 0;
    i = 0;
    zero = 0;
    one = 1;
    three = 3;
    knock = 0xc000;

    for (; i < 2; i++) {
        int id = this->mCylinderClsn[i].otherOwner;
        if (id != 0) {
            victim = dActor_c::FindWithID((unsigned int)id);
            if (victim != 0) {
                /* The actor profile is PLAYER before the Player-specific call. */
                int isPlayer = (victim->actorID == 0xbf);
                if (isPlayer) {
                    hurtPos.x = this->mPosX;
                    hurtPos.y = this->mPosY;
                    hurtPos.z = this->mPosZ;
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj((Player*)victim, &hurtPos, three, knock, one, zero, one);
                }
            }
        }
        if (victim != 0)
            break;
    }

    if ((u16)(this->mModelAnim.currFrame >> 12) == 0)
        func_0201267c(0x106, &this->mCamSpacePos);

    target = this->mTarget;
    if (target != 0) {
        Vec3_Sub((Vec3*)&tmp, (Vec3*)&this->mPosX, (Vec3*)&target->mPosX);
        dist = LenVec3((Vec3*)&tmp);
        ApproachLinear(this->mPrevAngleY, Vec3_HorzAngle(&this->mPos, &this->mTarget->mPos), 0x200);
        this->mAngleY = this->mPrevAngleY;
        if (dist > 0x3ab000) {
            if ((u16)(this->mModelAnim.currFrame >> 12) == 0) {
                func_ov063_0211ddac(this, kStateSearch);
                return;
            }
        }
    } else {
        if ((u16)(this->mModelAnim.currFrame >> 12) == 0) {
            func_ov063_0211ddac(this, kStateSearch);
            return;
        }
    }

    this->mModelAnim.Advance();

    x = this->mPosX;
    proj.x = x;
    proj.y = this->mPosY;
    z = this->mPosZ;
    proj.z = z;
    {
        int sinVal = data_02082214[(*(volatile u16*)&this->mAngleY >> 4) * 2];
        proj.x = x + FX_MUL(sinVal, 0x24000);
    }
    proj.z = z + FX_MUL(data_02082214[(*(volatile u16 *)&this->mAngleY >> 4) * 2 + 1], 0x24000);

    this->mCylinderClsn[0].pos.x = *(volatile int*)&this->mPosX;
    this->mCylinderClsn[0].pos.y = *(volatile int*)&this->mPosY;
    this->mCylinderClsn[0].pos.z = *(volatile int*)&this->mPosZ;
    this->mCylinderClsn[1].pos.x = proj.x;
    this->mCylinderClsn[1].pos.y = proj.y;
    this->mCylinderClsn[1].pos.z = proj.z;

    cc = this->mCylinderClsn;
    for (k = 0; k < 2; k++) {
        this->mCylinderClsn[k].radius = 0x9b000;
        cc->Clear();
        if (this->ClosestPlayer()->mIsVanish == 0)
            cc->Update();
        cc++;
    }
}

extern "C" {
// @symbol func_ov063_0211d88c
/* Sync the model matrix from the facing and position (eighths). */
void func_ov063_0211d88c(daPiano_c *self)
{
    extern void Matrix4x3_FromRotationY(Matrix4x3 *, int);
    Matrix4x3_FromRotationY(&self->mModelAnim.mat4x3, self->mAngleY);
    self->mModelAnim.mat4x3.m[9] = self->mPosX >> 3;
    self->mModelAnim.mat4x3.m[10] = self->mPosY >> 3;
    self->mModelAnim.mat4x3.m[11] = self->mPosZ >> 3;
}
}

extern "C" {
// @symbol func_ov063_0211d828
/* Refresh the collision matrix from the model matrix and position, then
 * refit the mesh collider to it. */
void func_ov063_0211d828(daPiano_c *self){
    self->mClsnMat = self->mModelAnim.mat4x3;
    self->mClsnMat.m[9] = self->mPosX;
    self->mClsnMat.m[10] = self->mPosY;
    self->mClsnMat.m[11] = self->mPosZ;
    self->mMeshCollider.Transform(self->mClsnMat, self->mAngleY);
}
}

extern "C" {
// @symbol func_ov063_0211d5f4
/* Pose the three drop shadows: rotate a body-frame offset by the facing,
 * add the actor position, and drop one shadow per dExtShadowModel_c. */
void func_ov063_0211d5f4(daPiano_c *self)
{
    extern void Matrix4x3_FromRotationY(Matrix4x3* m, int angle);
    extern void MulVec3Mat4x3(Vector3* in, Matrix4x3* m, Vector3* out);
    extern void AddVec3(Vector3* a, Vector3* b, Vector3* c);
    extern void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
        dActor_c* self, dExtShadowModel_c* sm, Matrix4x3* mtx, Fix12i fx, Fix12i t, unsigned int u);
    extern void _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
        dActor_c* self, dExtShadowModel_c* sm, Matrix4x3* mtx, Fix12i fx, Fix12i t1, Fix12i t2, unsigned int u);

    extern Matrix4x3 data_020a0e68;   /* shared scratch matrix */

    Vector3 in, out;

    in.x = 0x40000;
    in.y = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    in.z = -0x10000;
    Matrix4x3_FromRotationY(&data_020a0e68, self->mAngleY);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    AddVec3(&out, &self->mPos, &out);
    Matrix4x3_FromRotationY(&self->mShadowMats[2], self->mAngleY);
    self->mShadowMats[2].m[9] = out.x >> 3;
    self->mShadowMats[2].m[10] = self->mPosY >> 3;
    self->mShadowMats[2].m[11] = out.z >> 3;

    out.x = 0;
    out.y = 0;
    out.z = 0;
    in.y = 0;
    in.z = 0;
    in.x = -0x60000;
    Matrix4x3_FromRotationY(&data_020a0e68, self->mAngleY);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    AddVec3(&out, &self->mPos, &out);
    Matrix4x3_FromRotationY(&self->mShadowMats[0], self->mAngleY);
    self->mShadowMats[0].m[9] = out.x >> 3;
    self->mShadowMats[0].m[10] = self->mPosY >> 3;
    self->mShadowMats[0].m[11] = out.z >> 3;

    out.x = 0;
    out.y = 0;
    out.z = 0;
    in.x = 0;
    in.y = 0;
    in.z = -0x10000;
    Matrix4x3_FromRotationY(&data_020a0e68, self->mAngleY);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    AddVec3(&out, &self->mPos, &out);
    Matrix4x3_FromRotationY(&self->mShadowMats[1], self->mAngleY);
    self->mShadowMats[1].m[9] = out.x >> 3;
    self->mShadowMats[1].m[10] = self->mPosY >> 3;
    self->mShadowMats[1].m[11] = out.z >> 3;

    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
        self, &self->mShadowModel3, &self->mShadowMats[2], 0xf0000, 0x50000, 0xf);
    _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
        self, &self->mShadowModel1, &self->mShadowMats[0], 0xa0000, 0x50000, 0x110000, 0xf);
    _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
        self, &self->mShadowModel2, &self->mShadowMats[1], 0x80000, 0x50000, 0xf0000, 0xf);
}
}

/* _ZN9daPiano_cD1Ev (0x0211d4b8) and _ZN9daPiano_cD0Ev (0x0211d54c) are NOT
 * written here on purpose, and carry no @symbol marker: the inline destructor
 * in include/daPiano_c.h emits D1 then D0 -- the cartridge's order -- and no
 * D2, ahead of every function written below. tools/tiers.py scores both
 * through that inline definition. Their pre-promotion one-line sources lived
 * directly under src/ and no longer exist; the manifest entry
 * config/tu_manifest.d/ov063/daPiano_c.json keeps their full paths in the two
 * legacy_source rows. */

/* The TU's static-init globals in retail initializer order: the three
 * resource handles (model 0x40a, mesh collision 0x40b, attack animation
 * 0x40c) each construct and register a destructor node, then the two-state
 * PMF table copies its four {ptr, adj} records. mwcc emits all of it as
 * __sinit_d_a_piano.cpp in .init; the handwritten __sinit_ov063_0211e5fc.c
 * is retired. */
PianoModelFilePtr data_ov063_0211ef80(MAD_PIANO_MODEL_ASSET);
PianoCollisionFilePtr data_ov063_0211ef88(MAD_PIANO_COLLISION_ASSET);
PianoAnimationFilePtr data_ov063_0211ef90(MAD_PIANO_ATTACK_ANIMATION_ASSET);

PianoStateEntry data_ov063_0211efbc[2] = {
    { &daPiano_c::func_ov063_0211dd78, &daPiano_c::func_ov063_0211dbb8 },
    { &daPiano_c::func_ov063_0211dba4, &daPiano_c::func_ov063_0211d8cc },
};
