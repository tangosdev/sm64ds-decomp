//cpp
/* The crushers' shared base: Thwomp (DOSUN 161, ov091) and Grindel
 * (DONKAKU 162, ov025). `dsn` is dossun, the Thwomp's Japanese name.
 *
 * This TU owns the two vtable slots both leaves inherit -- CleanupResources
 * (slot 3, the key function, so this TU emits _ZTV11daDsnBase_c) and Render
 * (slot 9) -- plus the seven helpers both leaves' Behaviors call: the five
 * state steps of the shared 0..4 cycle (rise, hover, slam, rest, recover),
 * the drop-shadow update, and the Yoshi-egg proximity check that wakes the
 * mesh collider. Grindel's states 5..7 are its own (ov025); the Thwomp's
 * trigger logic is its own (Thwomp::Behavior). There is no factory: the
 * class is abstract (InitResources/Behavior are pure) and each leaf builds
 * itself.
 *
 * mwccarm lays .text down in reverse source order, so the definitions run
 * from the highest ROM address back toward the destructor pair. Do not
 * reorder. The D1/D0 pair itself is unwritten: the destructor is inline in
 * the class body, and owning the key function emits both variants
 * byte-identically (licensed deadstrip-duplicate; the enrolled shards keep
 * the canonical addresses because the cartridge orders D0 below D1 and no
 * admissible source form reproduces that order -- see the manifest).
 *
 * daDsnBase_c.h comes FIRST: dBgActor_c.h includes common.h before Model.h,
 * fixing Matrix4x3 to the flat s32 m[12] spelling the shadow-matrix copy in
 * func_ov091_02133098 compiled against. Do not hoist math/Matrix.h.
 *
 * deslop
 * Leftover:
 * - func_ov091_* keep ROM labels and C linkage: daDkk_c::Behavior (ov025)
 *   and Thwomp::Behavior call all seven by name across the TU boundary.
 * - 0x360..0x39f stay offset soup. The shadow Matrix4x3, the rise/ground
 *   heights, mState and the timer live on the LEAVES (daDkk_c.h, Thwomp.h),
 *   and both leaves plus this TU read the same words -- Init writes
 *   0x390/0x394/0x39e/0x39f directly. The move up that daDsnBase_c.h
 *   invites is deferred: it edits both leaf headers and re-verifies
 *   ov025/daDkk_c.
 * - DropShadowScaleXYZ / Earthquake / NewSimple stay mangled and TU-local:
 *   all three carry Fix12<int> BY VALUE (6az), and Earthquake/NewSimple are
 *   on no header. Earthquake's (void *, const Vector3 &, int) spelling is
 *   daDkk_c.cpp's, and the mangled name spells that reference out.
 * - CleanupResources reloads the file table after each Release (a Release
 *   clobbers); the three loads are the ROM's.
 * - kYoshiEggActorID / kDosunActorID are TU-local: no header names actor
 *   IDs. Both values are the ROM debug table's
 *   (symbols/profile_reconstruction_registry.tsv).
 * - dBgW_KcMbg::SetFile / TextureSequence::SetFile stay mangled: both take
 *   Fix12<int> BY VALUE (6az); the header method homes the argument.
 * - func_020393d4 is an 8-byte store into dBgW+0x18 (beforeClsnCallback).
 *   This TU calls it; naming belongs with dBgW in arm9.
 * - SharedFilePtr +4 BMD/BTP load (layout unrecovered; Prepare/SetFile).
 */

#include "daDsnBase_c.h"
#include "common.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dBgCh_Gnd.h"

/* Actor IDs this TU compares. No header vocabulary exists (cleaned callers
 * pass raw hex), so they live here, cited to the ROM's own debug table. */
enum {
    kYoshiEggActorID = 9,   /* YOSHI_EGG: func_ov091_02132dc0's target */
    kDosunActorID = 0xa1,   /* DOSUN (161): the Thwomp leaf; Grindel takes
                               the particle path in func_ov091_02132f04 */
};

/* The per-leaf resource table both InitResources store into mFileTable (the
 * Thwomp's at data_ov091_02135138, Grindel's at data_ov025_02113814). Shaped
 * from its consumers: Init loads the model/collision files,
 * binds the CLPS block and, when [3] is non-null, the texture animation;
 * CleanupResources releases [0], [1] and [3]; Render animates only when [3]
 * is set; func_ov091_02133098 reads [4]/[5] as shadow extents. Owned by the
 * leaf overlays, never defined here. */
struct DsnBaseFileTable {
    SharedFilePtr *model;       /* +0x00, BMD */
    SharedFilePtr *collision;   /* +0x04, KCL */
    CLPS_Block *clps;           /* +0x08, CLPS block: not a file, not released */
    SharedFilePtr *texAnim;     /* +0x0c, BTP, or null when the leaf has none */
    int shadowExtentX;          /* +0x10, DropShadow X base */
    int shadowExtentZ;          /* +0x14, DropShadow Z base */
};

#ifndef SM64DS_PLATFORM_PC
typedef char DsnBaseFileTable_size_must_be_0x18[
    sizeof(DsnBaseFileTable) == 0x18 ? 1 : -1];
#endif

/* --------------------------------------------------------------------------
 * The one file-scope extern "C" region. Everything here is reached from a
 * body below that cannot declare it in its own scope. The seven func_ov091_*
 * come from decl_common.h instead (all (char*), the real header wins), so
 * they are not restated.
 * ------------------------------------------------------------------------ */
extern "C" {

/* No header declares these (checked include/decl_common.h and its decl_Actor
 * siblings at promotion).
 * DecIfAbove0_Byte is spelled from its definition at src/DecIfAbove0_Byte.c --
 * unsigned char in, unsigned char out. A call site cannot evidence either: the
 * casts and the int temporaries below are the caller's, not the callee's
 * interface. RandomIntInternal's sites pass the RNG state. */
extern u8 DecIfAbove0_Byte(u8 *);
extern int RandomIntInternal(void *);
extern int data_0209e650[];

extern int Vec3_Dist(const Vector3 *, const Vector3 *);

/* daDkk_c.cpp's spelling, whose void return the enrolled definition at
 * src/func_0201267c.cpp confirms. */
extern void func_0201267c(int id, void *pos);

/* Fix12<int> BY VALUE (6az): the header member form would home the argument
 * and move the caller. Scalar tail is deliberate; the (const Vector3 &)
 * is what the mangled name spells. */
extern void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *self, const Vector3 &pos, int magnitude);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned id, int x, int y, int z);

/* Same 6az tail, and the same pointer spelling the enrolled definition uses
 * (dActor_c / ShadowModel / Matrix4x3 pointers, scalar ints -- see the
 * _ZN8dActor_c18DropShadowScaleXYZ file); daObjPathLift_c.cpp calls it this
 * way. The three scales stay scalar ints. */
extern void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    dActor_c *self, ShadowModel *shadow, Matrix4x3 *matrix, int scaleX, int scaleY, int scaleZ, unsigned opacity);

/* Same 6az tail: both take Fix12<int> BY VALUE, so the header member form
 * would home the argument. */
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat, int scale, s16 angY,
    CLPS_Block *clps);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
    TextureSequence *self, BTP_File *file, int flags, int speed,
    unsigned startFrame);
/* 8-byte store into dBgW+0x18 (beforeClsnCallback). No SetCallback member.
 * Spelling matches src/func_020393d4.c -- (int *, int). */
extern void func_020393d4(int *collider, int callback);

}

/* Init is the run's highest-address definition, so it is written first.
 * Both leaves' InitResources call it with their own file table already
 * stored. `Init` is a coined name: class ownership, the two inbound calls,
 * the body and the layout are proven; the original English is not. */
// @symbol _ZN11daDsnBase_c4InitEv
s32 daDsnBase_c::Init()
{
    Vector3 v;
    BMD_File *bmd;
    KCL_File *kcl;
    DsnBaseFileTable *files;
    CLPS_Block *clps;
    SharedFilePtr *texAnim;

    files = (DsnBaseFileTable *)mFileTable;
    bmd = (BMD_File *)Model::LoadFile(*files->model);
    mModel.SetFile(bmd, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    files = (DsnBaseFileTable *)mFileTable;
    kcl = (KCL_File *)dBgW_Kc::LoadFile(*files->collision);
    clps = files->clps;
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY, clps);
    func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosAndAngs);
    mMeshCollider.Enable(this);

    texAnim = ((DsnBaseFileTable *)mFileTable)->texAnim;
    if (texAnim != 0) {
        TextureSequence::LoadFile(*texAnim);
        files = (DsnBaseFileTable *)mFileTable;
        TextureSequence::Prepare(
            *(BMD_File *)((int *)files->model)[1],
            *(BTP_File *)((int *)files->texAnim)[1]);
        files = (DsnBaseFileTable *)mFileTable;
        texAnim = files->texAnim;
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
            &mTextureSequence, (BTP_File *)((int *)texAnim)[1],
            0x40000000, 0x1000, 0);
    }

    if (!mShadowModel.InitCuboid())
        return 0;

    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    v.y = v.y + 0x32000;
    {
        dBgCh_Gnd rg;
        rg.SetObjAndPos(v, 0);
        *(s32 *)((char *)this + 0x394) = v.y;
        if (rg.DetectClsn())
            *(s32 *)((char *)this + 0x394) = rg.clsnY;

        *(s32 *)((char *)this + 0x390) = mPosY + 0x190000;
        mPosY = *(s32 *)((char *)this + 0x394);
        *(u8 *)((char *)this + 0x39e) = 0x28;
        mVertAccel = -0x4000;
        mTerminalVelocity = -0x3c000;
        mHorzSpeed = 0xc000;
        *(u8 *)((char *)this + 0x39f) = 0;
    }
    return 1;
}

/* Vtable slot 9, inherited by both leaves. The texture animation runs only
 * when the leaf bound one: Grindel's table carries null at [3], the Thwomp's
 * carries its BTP handle. */
// @symbol _ZN11daDsnBase_c6RenderEv
int daDsnBase_c::Render()
{
    DsnBaseFileTable *files = (DsnBaseFileTable *)mFileTable;
    if (files->texAnim != 0)
        mTextureSequence.Update(mModel.data);
    mModel.Render(0);
    return 1;
}

/* Vtable slot 3, inherited by both leaves -- and the key function, so this
 * TU emits the vtable. Releases the model, the collision mesh and, when the
 * leaf bound one, the texture animation; the CLPS block at [2] is not a
 * file and is not released. */
// @symbol _ZN11daDsnBase_c16CleanupResourcesEv
int daDsnBase_c::CleanupResources()
{
    DsnBaseFileTable *files;
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    files = (DsnBaseFileTable *)mFileTable;
    files->model->Release();
    files = (DsnBaseFileTable *)mFileTable;
    files->collision->Release();
    files = (DsnBaseFileTable *)mFileTable;
    if (files->texAnim != 0)
        files->texAnim->Release();
    return 1;
}

/* The per-frame shadow refresh both Behaviors run after the state step.
 * Scales the drop shadow with the height above the stored ground (0x394),
 * grows mClipRadius with the same height, and rebuilds the shadow matrix
 * at 0x360 from the model's own matrix with the position shifted in.
 * 0x360/0x394 are leaf-span (see the file header); mClipRadius doubles as
 * func_ov091_02132dc0's trigger radius below. */
// @symbol func_ov091_02133098
extern "C" void func_ov091_02133098(char *c)
{
    /* One pointer only: a named shadowMtx/files pair spanning the body
     * reassigns every register (measured 64-word DIFF), and the 0x384/0x388/
     * 0x38c stores must stay direct [self,#imm] -- through a matrix pointer
     * they come out [r4,#0x24]. */
    daDsnBase_c *self = (daDsnBase_c *)c;
    int shadowDrop = 0x20000;
    int heightAboveGround = self->mPosY - *(s32 *)((char *)self + 0x394);
    if (heightAboveGround <= 0x14000) {
        heightAboveGround = 0x14000;
        shadowDrop = 0;
    }
    int radius = (int)(((long long)heightAboveGround * 0x60 + 0x800) >> 12);
    s32 *clipRadius = &self->mClipRadius;
    int scaleX = ((DsnBaseFileTable *)self->mFileTable)->shadowExtentX - radius;
    if (scaleX < 0xa000)
        scaleX = 0xa000;
    int scaleZ = ((DsnBaseFileTable *)self->mFileTable)->shadowExtentZ - radius;
    if (scaleZ < 0xa000)
        scaleZ = 0xa000;
    self->mClipRadius = heightAboveGround + 0x8c000;
    {
        int clamped = self->mClipRadius;
        if (clamped < 0x200000)
            clamped = 0x200000;
        self->mClipRadius = clamped;
    }
    *clipRadius = *clipRadius >> 3;
    *(Matrix4x3 *)((char *)self + 0x360) = self->mModel.mat4x3;
    *(s32 *)((char *)self + 0x384) = self->mPosX >> 3;
    *(s32 *)((char *)self + 0x388) = (self->mPosY - shadowDrop) >> 3;
    *(s32 *)((char *)self + 0x38c) = self->mPosZ >> 3;
    _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        self, &self->mShadowModel, (Matrix4x3 *)((char *)self + 0x360),
        scaleX, heightAboveGround + 0x28000, scaleZ, 0xf);
}

/* State 0, the rise. Climbs 0xa000 a frame toward the stored top height
 * (0x390); on arrival snaps to it, moves to state 1 and rolls the hover
 * time (0xa..0x27 frames). */
// @symbol func_ov091_02133020
extern "C" void func_ov091_02133020(char *c)
{
    daDsnBase_c *self = (daDsnBase_c *)c;
    self->mPosY += 0xa000;
    if (self->mPosY < *(s32 *)(c + 0x390))
        return;
    self->mPosY = *(s32 *)(c + 0x390);
    *(s32 *)(c + 0x398) = 1;
    *(u8 *)(c + 0x39e) =
        (u8)(((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 0x1e + 0xa);
}

/* State 1, the hover. Spends the timer state 0 rolled, then drops to
 * state 2. */
// @symbol func_ov091_02132ff4
extern "C" void func_ov091_02132ff4(char *c)
{
    int r = DecIfAbove0_Byte((u8 *)c + 0x39e);
    if (r == 0)
        *(s32 *)(c + 0x398) = 2;
}

/* State 2, the slam. Integrates the fall at 0x4000 a frame; on reaching the
 * stored ground (0x394) snaps to it, stops, moves to state 3 and lands:
 * the Thwomp (DOSUN) raises its landing dust, Grindel spawns particle 0x2e,
 * and both shake the camera and play 0xc7. */
// @symbol func_ov091_02132f04
extern "C" void func_ov091_02132f04(char *c)
{
    daDsnBase_c *self = (daDsnBase_c *)c;
    Vector3 dustPos;
    Vector3 quakePos;
    s32 *vertSpeed = &self->mVertSpeed;
    s32 fallSpeed = *vertSpeed;
    s32 *posY = &self->mPosY;
    fallSpeed = fallSpeed - 0x4000;
    *vertSpeed = fallSpeed;
    fallSpeed = *posY + self->mVertSpeed;
    *posY = fallSpeed;
    if (self->mPosY > *(s32 *)(c + 0x394))
        return;
    self->mPosY = *(s32 *)(c + 0x394);
    self->mVertSpeed = 0;
    *(s32 *)(c + 0x398) = 3;
    *(u8 *)(c + 0x39e) = 0xa;
    int isDosun = (self->actorID == kDosunActorID);
    if (isDosun != 0) {
        self->HugeLandingDust(true);
    } else {
        dustPos.x = self->mPosX;
        dustPos.y = self->mPosY;
        dustPos.z = self->mPosZ;
        dustPos.y = dustPos.y + 0x3c000;
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x2e, dustPos.x, dustPos.y, dustPos.z);
    }
    quakePos.x = self->mPosX;
    quakePos.y = self->mPosY;
    quakePos.z = self->mPosZ;
    _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(self, quakePos, 0x7d0000);
    func_0201267c(0xc7, &self->mCamSpacePosX);
}

/* State 3, the rest. Spends the landing timer, then moves to state 4 with
 * a fresh 0x14..0x1d-frame recovery time. */
// @symbol func_ov091_02132e98
extern "C" void func_ov091_02132e98(char *c)
{
    if (DecIfAbove0_Byte((u8 *)c + 0x39e) != 0)
        return;
    *(s32 *)(c + 0x398) = 4;
    unsigned int r = RandomIntInternal(data_0209e650);
    unsigned int v = r >> 16;
    c[0x39e] = (char)(v % 10 + 0x14);
}

/* State 4, the recover. Spends the timer state 3 set, then closes the cycle
 * back to state 0 with a 0x28-frame hover preload. */
// @symbol func_ov091_02132e64
extern "C" void func_ov091_02132e64(char *c)
{
    int r = DecIfAbove0_Byte((u8 *)c + 0x39e);
    if (r == 0) {
        *(s32 *)(c + 0x398) = 0;
        *(u8 *)(c + 0x39e) = 0x28;
    }
}

/* The egg proximity check both Behaviors run in the collision tail. When the
 * nearest Yoshi egg closes to (mClipRadius << 3) and the mesh collider is
 * still asleep, wakes it and reports 1 so the caller refreshes the collider
 * position. The aimPos block is dead by value -- the distance check reads
 * the actor origin, not it -- but it keeps the OnAimedAtWithEgg
 * call and the three stores the ROM emits. */
// @symbol func_ov091_02132dc0
extern "C" int func_ov091_02132dc0(char *c)
{
    daDsnBase_c *self = (daDsnBase_c *)c;
    dActor_c *egg = self->ClosestWithActorID(kYoshiEggActorID);
    if (egg != 0) {
        Vector3 aimPos;
        aimPos.x = self->mPosX;
        aimPos.y = self->mPosY;
        aimPos.z = self->mPosZ;
        aimPos.y = aimPos.y + self->OnAimedAtWithEgg();
        if (Vec3_Dist((const Vector3 *)(c + 0x5c),
                      (const Vector3 *)((char *)egg + 0x5c)) < (self->mClipRadius << 3)) {
            if (!self->mMeshCollider.IsEnabled()) {
                self->mMeshCollider.Enable(self);
                return 1;
            }
        }
    }
    return 0;
}

/* D1 (0x02132d6c) and D0 (0x02132d04): deliberately unwritten. The header's
 * inline destructor plus the key function above emits both byte-identically;
 * the manifest licenses them deadstrip-duplicate against the enrolled shards
 * (see the file header). */
// @symbol _ZN11daDsnBase_cD1Ev
// @symbol _ZN11daDsnBase_cD0Ev
