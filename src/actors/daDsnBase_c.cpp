//cpp
/* daDsnBase_c -- shared base of the Thwomp (daDsn_c, DOSUN 161, ov091)
 * and the Grindel (daDkk_c, DONKAKU 162, ov025). `dsn` is dossun.
 *
 * This TU owns CleanupResources (slot 3, the key function, so it emits
 * _ZTV11daDsnBase_c), Render (slot 9), Init, and the seven helpers both
 * leaves' Behaviors call: rise, hover, slam, rest, recover, the drop
 * shadow, and the Yoshi-egg check that wakes the mesh collider. No
 * factory: InitResources and Behavior are pure. Grindel's states 5..7
 * and the Thwomp's trigger live on the leaves.
 *
 * mwccarm lays .text in reverse source order. Do not reorder. The
 * destructor stays inline in daDsnBase_c.h: the cartridge orders D0
 * below D1, and this TU cannot emit that order, so the shards stay the
 * canonical copies. daDsnBase_c.h is the first include so Matrix4x3 is
 * common.h's flat s32 m[12]. Do not hoist math/Matrix.h.
 *
 * deslop leftovers:
 * - func_ov091_02133098 keeps the early `s32 *clipRadius` and the braced
 *   mClipRadius clamp. An if-clamp in that spot is a 64-word DIFF.
 *   Dropping the pointer and shifting mClipRadius directly is a 1-word
 *   DIFF. dActor_c::DropShadowScaleXYZ with Fix12<int> by value
 *   size-DIFFs 0x120 -> 0x134; the scalar extern stays.
 * - func_ov091_02132f04 keeps `int isDosun` and `if (isDosun != 0)`.
 *   Comparing actorID in the if size-DIFFs 0xf0 -> 0xe4. Combining
 *   dustPos.y into `mPosY + 0x3c000` size-DIFFs 0xf0 -> 0xec. NewSimple
 *   and Earthquake are not header members; the scalar externs stay.
 * - Init keeps the two-step probePos.y (combined form size-DIFFs
 *   0x1a8 -> 0x1a4). dBgW_KcMbg::SetFile with Fix12<int> size-DIFFs
 *   0x1a8 -> 0x1ac. TextureSequence::SetFile with Fix12<int> size-DIFFs
 *   0x1a8 -> 0x1b0. Storing beforeClsnCallback directly size-DIFFs
 *   0x1a8 -> 0x1a4; the body calls func_020393d4.
 * - CleanupResources reloads mFileTable around each Release. One pointer
 *   across the three calls is a 20-word DIFF.
 * - func_ov091_02132ff4, func_ov091_02132e98 and func_ov091_02132e64 stay
 *   extern "C". func_ov091_02133098, func_ov091_02133020, func_ov091_02132f04
 *   and func_ov091_02132dc0 are methods; the address is the method name.
 *   DsnCycle only names the shared tail inside this TU.
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

/* SharedFilePtr has no fields (include/SharedFilePtr.h). Both leaves' tables
 * store the loaded BMD/BTP at +4; Prepare and SetFile read that word. */
struct DsnFileHandle {
    s32 fileId;
    void *loaded;
};

/* 0x360..0x39f is the same on both leaves, and daDsn_c.h / daDkk_c.h already
 * declare it, so the view stays in this TU. Based at the actor, shadowMtx.m[9]
 * is the store at 0x384. */
struct DsnCycle {
    char prefix[0x5c];
    s32 posX;                 /* 0x05c */
    s32 posY;                 /* 0x060 */
    s32 posZ;                 /* 0x064 */
    char mid[0x360 - 0x68];
    Matrix4x3 shadowMtx;      /* 0x360; translation row is m[9..11] at 0x384 */
    s32 riseY;                /* 0x390 */
    s32 groundY;              /* 0x394 */
    s32 state;                /* 0x398 */
    s16 turnAng;              /* 0x39c -- Grindel only; this TU does not write it */
    u8 timer;                 /* 0x39e */
    u8 landCount;             /* 0x39f */
};

/* --------------------------------------------------------------------------
 * The one file-scope extern "C" region. Everything here is reached from a
 * body below that cannot declare it in its own scope.
 * func_ov091_02132ff4/02132e98/02132e64 come from decl_common.h (all (char*),
 * the real header wins), so they are not restated.
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
    Vector3 probePos;
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
            *(BMD_File *)((DsnFileHandle *)files->model)->loaded,
            *(BTP_File *)((DsnFileHandle *)files->texAnim)->loaded);
        files = (DsnBaseFileTable *)mFileTable;
        texAnim = files->texAnim;
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
            &mTextureSequence, (BTP_File *)((DsnFileHandle *)texAnim)->loaded,
            0x40000000, 0x1000, 0);
    }

    if (!mShadowModel.InitCuboid())
        return 0;

    probePos.x = mPosX;
    probePos.y = mPosY;
    probePos.z = mPosZ;
    probePos.y = probePos.y + 0x32000;
    {
        dBgCh_Gnd ground;
        ground.SetObjAndPos(probePos, 0);
        DsnCycle *cycle = (DsnCycle *)this;
        cycle->groundY = probePos.y;
        if (ground.DetectClsn())
            cycle->groundY = ground.clsnY;

        cycle->riseY = mPosY + 0x190000;
        mPosY = cycle->groundY;
        cycle->timer = 0x28;
        mVertAccel = -0x4000;
        mTerminalVelocity = -0x3c000;
        mHorzSpeed = 0xc000;
        cycle->landCount = 0;
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
 * Scales the drop shadow with the height above groundY, grows mClipRadius
 * with that height, and copies the model matrix into shadowMtx with the
 * translation shifted. mClipRadius is also func_ov091_02132dc0's trigger
 * radius. The clipRadius pointer and the braced clamp are load-bearing;
 * see the file comment. */
// @symbol _ZN11daDsnBase_c19func_ov091_02133098Ev
void daDsnBase_c::func_ov091_02133098()
{
    DsnCycle *cycle = (DsnCycle *)this;
    int shadowDrop = 0x20000;
    int heightAboveGround = mPosY - cycle->groundY;
    if (heightAboveGround <= 0x14000) {
        heightAboveGround = 0x14000;
        shadowDrop = 0;
    }
    int radius = (int)(((long long)heightAboveGround * 0x60 + 0x800) >> 12);
    s32 *clipRadius = &mClipRadius;
    DsnBaseFileTable *files = (DsnBaseFileTable *)mFileTable;
    int scaleX = files->shadowExtentX - radius;
    if (scaleX < 0xa000)
        scaleX = 0xa000;
    int scaleZ = files->shadowExtentZ - radius;
    if (scaleZ < 0xa000)
        scaleZ = 0xa000;
    mClipRadius = heightAboveGround + 0x8c000;
    {
        int clamped = mClipRadius;
        if (clamped < 0x200000)
            clamped = 0x200000;
        mClipRadius = clamped;
    }
    *clipRadius = *clipRadius >> 3;
    cycle->shadowMtx = mModel.mat4x3;
    cycle->shadowMtx.m[9] = cycle->posX >> 3;
    cycle->shadowMtx.m[10] = (cycle->posY - shadowDrop) >> 3;
    cycle->shadowMtx.m[11] = cycle->posZ >> 3;
    _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        this, &mShadowModel, &cycle->shadowMtx,
        scaleX, heightAboveGround + 0x28000, scaleZ, 0xf);
}

/* State 0, the rise. Climbs 0xa000 a frame toward the stored top height
 * (0x390); on arrival snaps to it, moves to state 1 and rolls the hover
 * time (0xa..0x27 frames). */
// @symbol _ZN11daDsnBase_c19func_ov091_02133020Ev
void daDsnBase_c::func_ov091_02133020()
{
    DsnCycle *self = (DsnCycle *)this;
    self->posY += 0xa000;
    if (self->posY < self->riseY)
        return;
    self->posY = self->riseY;
    self->state = 1;
    self->timer =
        (u8)(((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 0x1e + 0xa);
}

/* State 1, the hover. Spends the timer state 0 rolled, then drops to
 * state 2. */
// @symbol func_ov091_02132ff4
extern "C" void func_ov091_02132ff4(char *c)
{
    DsnCycle *self = (DsnCycle *)c;
    int timeLeft = DecIfAbove0_Byte(&self->timer);
    if (timeLeft == 0)
        self->state = 2;
}

/* State 2, the slam. Integrates the fall at 0x4000 a frame; on reaching the
 * stored ground (0x394) snaps to it, stops, moves to state 3 and lands:
 * the Thwomp (DOSUN) raises its landing dust, Grindel spawns particle 0x2e,
 * and both shake the camera and play 0xc7. */
// @symbol _ZN11daDsnBase_c19func_ov091_02132f04Ev
void daDsnBase_c::func_ov091_02132f04()
{
    Vector3 dustPos;
    Vector3 quakePos;
    mVertSpeed = mVertSpeed - 0x4000;
    mPosY = mPosY + mVertSpeed;
    DsnCycle *cycle = (DsnCycle *)this;
    if (mPosY > cycle->groundY)
        return;
    mPosY = cycle->groundY;
    mVertSpeed = 0;
    cycle->state = 3;
    cycle->timer = 0xa;
    int isDosun = (actorID == kDosunActorID);
    if (isDosun != 0) {
        HugeLandingDust(true);
    } else {
        dustPos.x = mPosX;
        dustPos.y = mPosY;
        dustPos.z = mPosZ;
        dustPos.y = dustPos.y + 0x3c000;
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x2e, dustPos.x, dustPos.y, dustPos.z);
    }
    quakePos.x = mPosX;
    quakePos.y = mPosY;
    quakePos.z = mPosZ;
    _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(this, quakePos, 0x7d0000);
    func_0201267c(0xc7, &mCamSpacePosX);
}

/* State 3, the rest. Spends the landing timer, then moves to state 4 with
 * a fresh 0x14..0x1d-frame recovery time. */
// @symbol func_ov091_02132e98
extern "C" void func_ov091_02132e98(char *c)
{
    DsnCycle *self = (DsnCycle *)c;
    if (DecIfAbove0_Byte(&self->timer) != 0)
        return;
    self->state = 4;
    unsigned int roll = RandomIntInternal(data_0209e650);
    unsigned int rollHigh = roll >> 16;
    self->timer = (char)(rollHigh % 10 + 0x14);
}

/* State 4, the recover. Spends the timer state 3 set, then closes the cycle
 * back to state 0 with a 0x28-frame hover preload. */
// @symbol func_ov091_02132e64
extern "C" void func_ov091_02132e64(char *c)
{
    DsnCycle *self = (DsnCycle *)c;
    int timeLeft = DecIfAbove0_Byte(&self->timer);
    if (timeLeft == 0) {
        self->state = 0;
        self->timer = 0x28;
    }
}

/* The egg proximity check both Behaviors run in the collision tail. When the
 * nearest Yoshi egg closes to (mClipRadius << 3) and the mesh collider is
 * still asleep, wakes it and reports 1 so the caller refreshes the collider
 * position. The aimPos block is dead by value -- the distance check reads
 * the actor origin, not it -- but it keeps the OnAimedAtWithEgg
 * call and the three stores the ROM emits. */
// @symbol _ZN11daDsnBase_c19func_ov091_02132dc0Ev
int daDsnBase_c::func_ov091_02132dc0()
{
    dActor_c *egg = ClosestWithActorID(kYoshiEggActorID);
    if (egg != 0) {
        Vector3 aimPos;
        aimPos.x = mPosX;
        aimPos.y = mPosY;
        aimPos.z = mPosZ;
        aimPos.y = aimPos.y + OnAimedAtWithEgg();
        if (Vec3_Dist((const Vector3 *)&mPosX,
                      (const Vector3 *)&egg->mPosX) < (mClipRadius << 3)) {
            if (!mMeshCollider.IsEnabled()) {
                mMeshCollider.Enable(this);
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
