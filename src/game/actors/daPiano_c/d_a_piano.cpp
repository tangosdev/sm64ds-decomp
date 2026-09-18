//cpp
/* The Mad Piano (PIANO) -- ov063/daPiano_c, Big Boo's Haunt.
 *
 * The haunted piano: dormant until a moving player comes within 500.0,
 * then lunges across the floor and bites (Player::Hurt). Two PMF
 * states, search (0) and attack (1), dispatched through
 * data_ov063_0211efbc by func_ov063_0211ddf4/ddac;
 * __sinit_ov063_0211e5fc fills the table from the four ROM {ptr, adj}
 * records. This is SM64's biting piano.
 *
 * Proof: RTTI names this class daPiano_c; the registry holds
 * g_profile_PIANO; the factory allocates sizeof(daPiano_c) (0x6e4) and
 * punches &_ZTV9daPiano_c; ov063's roster (daObjTh_Fall_Block_c, the bookshelf
 * trap) is Big Boo's Haunt.
 *
 * common.h's flat Matrix4x3 (s32 m[12]) stands in this TU: dBgActor_c.h
 * pulls common.h before ModelAnim.h reaches math/Matrix.h, so the
 * nested {r, t} spelling never defines. Translation is m[9..11]; the
 * d828 struct copy is the ROM's twelve-word copy (daBmb's note).
 *
 * deslop leftovers:
 * - Factory stays hand-rolled (see the comment above it): `return new
 *   daPiano_c()` matches but emits unlicensed dBgActor_cD2/Vector3D1
 *   copies the multi-symbol path refuses. daPiano_c is not a promoted
 *   TU, so it owns no manifest under config/tu_manifest.d/ov063 -- and a
 *   TU manifest is the only place a deadstrip-duplicate license can live.
 *   (The overlay itself does have manifests; ov063 carries
 *   daObjTh_Fall_Block_c.) Packaging, not a byte wall. daWanwan-consistent.
 * - D0/D1 stay split (one line each): the vtable is a symbols.txt blob
 *   (_ZTV9daPiano_c), and defining ~daPiano_c here would emit a
 *   competing _ZTV (key function) the fail-closed path refuses. Same
 *   wall as the factory.
 * - __sinit_ov063_0211e5fc stays split: delinks places one range per
 *   file, and nothing owns .text plus .init.
 * - ModelAnim::SetAnim / dBgCh_Actr::Init / dCcAcPos_c::Init /
 *   dBgW_KcMbg::SetFile stay mangled free declarations (wall 6az: the
 *   ROM signatures carry Fix12<int> by value; the header method forms
 *   home differently).
 * - dActor_c::DropShadowRadHeight/ScaleXYZ stay mangled (wall 6az):
 *   Fix12<int> is a bare aggregate, so the member form must materialize
 *   every argument and func_ov063_0211d5f4 grows 0x234 -> 0x29c; the
 *   literal spelling does not compile at all. Measured, not assumed --
 *   notes/experiments/piano-2712-dropshadow-advance.md.
 * - func_ov063_* helpers keep their ROM labels (true names unknown) and
 *   the C-origin ones keep PianoVec3 locals: Vector3's inline dtor
 *   would emit _ZN7Vector3D1Ev (S3). Volatile reloads and sine taps are
 *   ROM-true.
 * - (Vector3 *)&mPosX puns stay (daWanwan-consistent; dActor_c::Pos()
 *   is out of scope for this TU's shared-header budget).
 * - func_0203568c / func_02035684: dBgCh_Actr radius/height stores; no
 *   setter (daBmb's leftover).
 * - g_profile_PIANO stays a symbols.txt blob (S14).
 * - d8cc's (char *)victim+0x0c keeps its offset: no base-header name
 *   exists for dBase_c+0x0c.
 * - data_ov063_0211ecb8 keeps its label: CLPS_Block is forward-only in
 *   this tree, so the SetFile arg block has no type.
 * - func_0201267c (sound 0x106) is unmatched; called by address.
 */
#include "daPiano_c.h"
#include "MadPianoResources.h"
#include "SharedFilePtr.h"
#include "Player.h"

extern SharedFilePtr gPianoModelFile;
extern SharedFilePtr gPianoCollisionFile;
extern SharedFilePtr gPianoAttackAnimationFile;
extern char data_ov063_0211ecb8;

extern SharedFilePtr daPiano_c_AnimFile;
extern SharedFilePtr daPiano_c_ClsnFile;
extern SharedFilePtr daPiano_c_ModelFile;

/* One file-scope extern "C" region: the union of the five legacy shards'
 * declarations, deduplicated. C++-named members cannot carry block-scope
 * linkage specifications, so everything they call lives here. */
extern "C" {
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *, BCA_File *, int, int, unsigned int);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(dBgCh_Actr *, dActor_c *, Fix12i, Fix12i, Vector3_16 *, Vector3_16 *);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(dCcAcPos_c *, dActor_c *, const Vector3 *, Fix12i, Fix12i, unsigned int, unsigned int);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, void *, const Matrix4x3 *, int, short, void *);
extern void func_ov063_0211d88c(daPiano_c *self);
extern void func_ov063_0211d828(daPiano_c *self);
extern void func_ov063_0211d5f4(daPiano_c *self);
int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
void dBgCh_Actr_UpdateContinuous_Veneer(dBgCh_Actr *clsn);
void func_0203568c(int *clsn, int radius);
void func_02035684(int *clsn, int height);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *, int, int);
void func_ov063_0211ddf4(daPiano_c *self);
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN10dBgActor_cC2Ev(void *self);
void _ZN9ModelAnimC1Ev(void *self);
void _ZN11ShadowModelC1Ev(void *self);
void __cxa_vec_ctor(void *array, int count, int stride, void *ctor, void *dtor);
void _ZN10dCcAcPos_cC1Ev(void *self);
void _ZN10dCcAcPos_cD1Ev(void *self);
void _ZN10dBgCh_ActrC1Ev(void *self);
extern void *_ZTV9daPiano_c;
}

/* The registry factory. C LINKAGE IS LOAD-BEARING -- the ROM symbol is the
 * bare name.
 *
 * deslop leftover: this factory stays hand-rolled, and that is measured, not
 * stylistic. `return new daPiano_c()` MATCHES the factory's own bytes (0x98,
 * linkcheck VERIFIED) -- but the TU then will not LINK in production: the
 * new-expression makes mwccarm emit unreferenced vague copies of
 * _ZN10dBgActor_cD2Ev (0x38, dBgActor_c's dtor is inline in its header) and
 * _ZN7Vector3D1Ev (0x4) alongside the TU's text, and objisolate's fail-closed
 * multi-symbol path admits no content no manifest licenses -- and ov063 has
 * no manifest to carry a deadstrip-duplicate license (daBmb_c's Vector3D1
 * row is the shape of the fix). Bisected 4-way: hand-rolled with and without
 * the leaf operator new header is clean; the new-expression strays with and
 * without parens. Until this TU is promoted with a compiler_only license,
 * the factory keeps the explicit ABI boundary and the subobjects keep the
 * real class layout. (dossunbar's factories on main are hand-rolled for the
 * same production shape; daWanwan keeps its hand-rolled factory on a
 * size-DIFF.)
 *
 * Reconstructed source-style name: SM64DS proves daPiano_c through RTTI,
 * allocation size, vtable identity, and the PIANO registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: daPiano_c_Spawn. */
// @symbol daPiano_c_classInit
extern "C" daPiano_c *daPiano_c_classInit()
{
    daPiano_c *actor = (daPiano_c *)_ZN7fBase_cnwEj(sizeof(daPiano_c));
    if (actor) {
        _ZN10dBgActor_cC2Ev(actor);
        *(void **)actor = &_ZTV9daPiano_c;
        _ZN9ModelAnimC1Ev(&actor->mModelAnim);
        _ZN11ShadowModelC1Ev(&actor->mShadowModel1);
        _ZN11ShadowModelC1Ev(&actor->mShadowModel2);
        _ZN11ShadowModelC1Ev(&actor->mShadowModel3);
        __cxa_vec_ctor(actor->mCylinderClsn, 2, sizeof(dCcAcPos_c),
            (void *)_ZN10dCcAcPos_cC1Ev, (void *)_ZN10dCcAcPos_cD1Ev);
        _ZN10dBgCh_ActrC1Ev(&actor->mWithMeshClsn);
    }
    return actor;
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
    f = Animation::LoadFile(gPianoAttackAnimationFile);
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
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
            cylinder, this, (const Vector3 *)&mPosX, 0x9b000, 0xfa000, 0x200004, 0);
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
    if (Vec3_HorzDist(&mHomePos, (const Vector3 *)&mPosX) > 0x180000) {
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
    daPiano_c_ModelFile.Release();
    daPiano_c_AnimFile.Release();
    daPiano_c_ClsnFile.Release();
    return 1;
}

/* The piano's two-state dispatch table (data_ov063_0211efbc, filled by
 * __sinit_ov063_0211e5fc from the four ROM {ptr, adj} records): per state,
 * an entry routine and a body routine. Bound to daPiano_c itself. An earlier
 * draft routed this through a non-polymorphic shadow layout on the theory
 * that a PMF of the real class does not carry the ROM's plain {ptr, adj}
 * representation; that is false for this TU -- under 2004/b56 the real class
 * gives byte-identical bodies and relocation records for all 17 functions. */
typedef void (daPiano_c::*PianoPMF)();
struct PianoStateEntry { PianoPMF pmf[2]; };
/* State 0 searches (body func_ov063_0211dbb8, entry func_ov063_0211dd78);
 * state 1 attacks (body func_ov063_0211d8cc, entry func_ov063_0211dba4).
 * Proven by who calls ddac with what and which routine runs per-frame. */
enum PianoState { kStateSearch = 0, kStateAttack = 1 };

/* TU-local POD shadow for the scratch vectors in the absorbed C-origin
 * helpers. types.h's Vector3 carries an inline destructor, which would make
 * this TU emit _ZN7Vector3D1Ev -- unlicensed content the production
 * multi-symbol path refuses. Plain ints keep the C codegen exactly. */
struct PianoVec3 { int x, y, z; };

extern "C" {
// @symbol func_ov063_0211ddf4
/* Run the current state's body routine (pmf[1]) every frame. */
void func_ov063_0211ddf4(daPiano_c *self)
{
    extern PianoStateEntry data_ov063_0211efbc[];
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
    extern PianoStateEntry data_ov063_0211efbc[];
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

extern "C" {
// @symbol func_ov063_0211dd78
/* State-0 entry, reached via the PMF table: stop dead. */
void func_ov063_0211dd78(daPiano_c *self)
{
    self->mHorzSpeed = 0;
}
}

extern "C" {
#pragma opt_strength_reduction off
// @symbol func_ov063_0211dbb8
/* State-0 body, run every frame: scan the player slots for a moving target
 * within 500.0 and attack it; otherwise refresh the idle cylinder sweep
 * (current position plus a 36.0 lookahead along the facing). */
void func_ov063_0211dbb8(daPiano_c *self) {
    extern void Vec3_Sub(PianoVec3* out, PianoVec3* a, PianoVec3* b);
    extern int LenVec3(PianoVec3* v);
    extern int func_ov063_0211dd84(int unused, dActor_c* cand);
    extern void func_ov063_0211ddac(daPiano_c* c, int state);

    extern unsigned char data_0209f21c;
    extern void* data_0209f394[];
    extern short data_02082214[];   /* sine table: (angle >> 4) * 2 taps sin/cos */

    volatile PianoVec3 tmp;
    PianoVec3 v;
    int bestDist;
    int i;
    void* cand;
    int dist;

    bestDist = 0x7fffffff;
    for (i = 0; i < data_0209f21c; i++) {
        cand = data_0209f394[i];
        if (cand == 0) continue;
        Vec3_Sub(&v, (PianoVec3*)&self->mPosX, (PianoVec3*)&((dActor_c*)cand)->mPosX);
        dist = LenVec3(&v);
        if (func_ov063_0211dd84((int)self, (dActor_c*)cand) != 0 && dist < 0x1f4000 && dist < bestDist) {
            self->mTarget = (dActor_c*)cand;
            func_ov063_0211ddac(self, kStateAttack);
            return;
        }
    }

    {
        int x = self->mPosX;
        int z;
        int j;
        dCcAcPos_c* cc;

        tmp.x = x;
        tmp.y = self->mPosY;
        z = self->mPosZ;
        tmp.z = z;
        {
            int sinVal = data_02082214[(*(volatile u16*)&self->mAngleY >> 4) * 2];
            tmp.x = x + (int)(((s64)sinVal * 0x24000 + 0x800) >> 12);
        }
        tmp.z = z + (int)(((s64)data_02082214[(*(volatile u16*)&self->mAngleY >> 4) * 2 + 1] * 0x24000 + 0x800) >> 12);

        self->mCylinderClsn[0].pos.x = *(volatile int*)&self->mPosX;
        self->mCylinderClsn[0].pos.y = *(volatile int*)&self->mPosY;
        self->mCylinderClsn[0].pos.z = *(volatile int*)&self->mPosZ;
        self->mCylinderClsn[1].pos.x = tmp.x;
        self->mCylinderClsn[1].pos.y = tmp.y;
        self->mCylinderClsn[1].pos.z = tmp.z;

        cc = self->mCylinderClsn;
        for (j = 0; j < 2; j++) {
            self->mCylinderClsn[j].radius = 0x9b000;
            cc->Clear();
            cc++;
        }
    }
}
}

extern "C" {
// @symbol func_ov063_0211dba4
/* State-1 entry, reached via the PMF table: full-speed animation and a
 * 30-frame windup before the bite. */
void func_ov063_0211dba4(daPiano_c *self)
{
    self->mModelAnim.speed = 4096;
    self->mAttackTimer = 30;
}
}

extern "C" {
#pragma opt_strength_reduction off
// @symbol func_ov063_0211d8cc
/* State-1 body, run every frame: lunge at the target and bite any player
 * caught by the cylinders. Gives up (back to state 0) when the target is
 * gone or farther than 939.0 at a frame boundary. */
void func_ov063_0211d8cc(daPiano_c* self)
{
    extern u8 DecIfAbove0_Byte(u8* p);
    extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(Player* self, const PianoVec3* v, unsigned int a, int fix, unsigned int b, unsigned int d, unsigned int e);
    extern void func_0201267c(int a, void* p);
    extern void Vec3_Sub(PianoVec3* out, PianoVec3* a, PianoVec3* b);
    extern int LenVec3(PianoVec3* v);
    extern s16 Vec3_HorzAngle(const PianoVec3* v0, const PianoVec3* v1);
    extern int _Z14ApproachLinearRsss(s16& v, s16 target, s16 step);
    extern void func_ov063_0211ddac(daPiano_c* c, int state);

    extern s16 data_02082214[];   /* sine table: (angle >> 4) * 2 taps sin/cos */

    dActor_c* victim;
    int i;
    volatile PianoVec3 proj;
    PianoVec3 hurtPos;
    PianoVec3 tmp;
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

    if (DecIfAbove0_Byte(&self->mAttackTimer) != 0)
        return;

    self->mHorzSpeed = 0x5000;
    victim = 0;
    i = 0;
    zero = 0;
    one = 1;
    three = 3;
    knock = 0xc000;

    for (; i < 2; i++) {
        int id = self->mCylinderClsn[i].otherOwner;
        if (id != 0) {
            victim = dActor_c::FindWithID((unsigned int)id);
            if (victim != 0) {
                /* C-view actorID; 0xbf is the player profile. No base-header
                 * name exists for dBase_c+0x0c, so the offset stays. */
                int isPlayer = (*(u16*)((char*)victim + 0xc) == 0xbf);
                if (isPlayer) {
                    hurtPos.x = self->mPosX;
                    hurtPos.y = self->mPosY;
                    hurtPos.z = self->mPosZ;
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj((Player*)victim, &hurtPos, three, knock, one, zero, one);
                }
            }
        }
        if (victim != 0)
            break;
    }

    if ((u16)(self->mModelAnim.currFrame >> 12) == 0)
        func_0201267c(0x106, &self->mCamSpacePosX);

    target = self->mTarget;
    if (target != 0) {
        Vec3_Sub(&tmp, (PianoVec3*)&self->mPosX, (PianoVec3*)&target->mPosX);
        dist = LenVec3(&tmp);
        _Z14ApproachLinearRsss(
            self->mPrevAngleY,
            Vec3_HorzAngle((PianoVec3*)&self->mPosX, (PianoVec3*)&self->mTarget->mPosX),
            0x200);
        self->mAngleY = self->mPrevAngleY;
        if (dist > 0x3ab000) {
            if ((u16)(self->mModelAnim.currFrame >> 12) == 0) {
                func_ov063_0211ddac(self, kStateSearch);
                return;
            }
        }
    } else {
        if ((u16)(self->mModelAnim.currFrame >> 12) == 0) {
            func_ov063_0211ddac(self, kStateSearch);
            return;
        }
    }

    self->mModelAnim.Advance();

    x = self->mPosX;
    proj.x = x;
    proj.y = self->mPosY;
    z = self->mPosZ;
    proj.z = z;
    {
        int sinVal = data_02082214[(*(volatile u16*)&self->mAngleY >> 4) * 2];
        proj.x = x + (int)(((s64)sinVal * 0x24000 + 0x800) >> 12);
    }
    proj.z = z + (int)(((s64)data_02082214[(*(volatile u16*)&self->mAngleY >> 4) * 2 + 1] * 0x24000 + 0x800) >> 12);

    self->mCylinderClsn[0].pos.x = *(volatile int*)&self->mPosX;
    self->mCylinderClsn[0].pos.y = *(volatile int*)&self->mPosY;
    self->mCylinderClsn[0].pos.z = *(volatile int*)&self->mPosZ;
    self->mCylinderClsn[1].pos.x = proj.x;
    self->mCylinderClsn[1].pos.y = proj.y;
    self->mCylinderClsn[1].pos.z = proj.z;

    cc = self->mCylinderClsn;
    for (k = 0; k < 2; k++) {
        self->mCylinderClsn[k].radius = 0x9b000;
        cc->Clear();
        if (self->ClosestPlayer()->mIsVanish == 0)
            cc->Update();
        cc++;
    }
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
 * add the actor position, and drop one shadow per ShadowModel. */
void func_ov063_0211d5f4(daPiano_c *self)
{
    extern void Matrix4x3_FromRotationY(Matrix4x3* m, int angle);
    extern void MulVec3Mat4x3(PianoVec3* in, Matrix4x3* m, PianoVec3* out);
    extern void AddVec3(PianoVec3* a, PianoVec3* b, PianoVec3* c);
    extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        dActor_c* self, ShadowModel* sm, Matrix4x3* mtx, Fix12i fx, Fix12i t, unsigned int u);
    extern void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        dActor_c* self, ShadowModel* sm, Matrix4x3* mtx, Fix12i fx, Fix12i t1, Fix12i t2, unsigned int u);

    extern Matrix4x3 data_020a0e68;   /* shared scratch matrix */

    PianoVec3 in, out;

    in.x = 0x40000;
    in.y = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    in.z = -0x10000;
    Matrix4x3_FromRotationY(&data_020a0e68, self->mAngleY);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    AddVec3(&out, (PianoVec3*)&self->mPosX, &out);
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
    AddVec3(&out, (PianoVec3*)&self->mPosX, &out);
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
    AddVec3(&out, (PianoVec3*)&self->mPosX, &out);
    Matrix4x3_FromRotationY(&self->mShadowMats[1], self->mAngleY);
    self->mShadowMats[1].m[9] = out.x >> 3;
    self->mShadowMats[1].m[10] = self->mPosY >> 3;
    self->mShadowMats[1].m[11] = out.z >> 3;

    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        self, &self->mShadowModel3, &self->mShadowMats[2], 0xf0000, 0x50000, 0xf);
    _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        self, &self->mShadowModel1, &self->mShadowMats[0], 0xa0000, 0x50000, 0x110000, 0xf);
    _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        self, &self->mShadowModel2, &self->mShadowMats[1], 0x80000, 0x50000, 0xf0000, 0xf);
}
}
