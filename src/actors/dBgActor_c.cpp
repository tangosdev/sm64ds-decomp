//cpp
/* dBgActor_c -- the base of the level-object family (ov002).
 *
 * Platforms, lifts, blocks: a model, a moving mesh collider, and the matrix
 * pinning the collider to the model. Kill (slot 31) is the one virtual this
 * class adds -- and the key function, so this TU owns the vtable with D1/D0
 * dragged in beside it. Mega Mario launches these actors flying via
 * KillByMegaChar/UpdateKillByMegaChar; IsClsnInRange* toggle the collider by
 * camera distance. Size is asserted in include/dBgActor_c.h (0x320).
 *
 * Written last-to-first: mwccarm emits one .text section per function in
 * reverse source order. Do not reorder.
 *
 * deslop leftovers:
 * - IsClsnInRange / IsClsnInRangeOnScreen stay extern "C" free functions with
 *   int params: the ROM names claim Fix12<int> by value and the bytes refuse
 *   it (notes/mwccarm-codegen.md 6az), which is why the header deliberately
 *   does not declare them. Same spelling every leaf call site uses.
 * - func_ov002_020ee5d0 keeps its ROM label: the mega-knockback model-matrix
 *   helper, called only from UpdateKillByMegaChar. C linkage is load-bearing.
 * - dActor_c::Earthquake / Particle::System::NewSimple stay TU-local mangled
 *   calls: Fix12<int> by value (6az); typed shared-header decls of them are
 *   overload poison (S13).
 * - (Vector3 *)&mPosX puns stay: dActor_c::Pos() does not exist yet (#2513).
 * - Player+0x5c stays an offset: Player is only forward-declared here, and
 *   naming three words is not worth pulling Player.h into a TU that has no
 *   other use for it.
 * - unk_31c/unk_31d stay: the mega-knockback armed flag and its 30-frame fuse
 *   to Kill(), but RotatingUpDownPlatformUtm::Kill also writes unk_31c, so
 *   renaming is a shared-header plus leaf change, out of scope for this TU.
 * - data_020a0e68 is the shared scratch matrix; no header owns it, TU-local.
 */
/* dBgActor_c.h FIRST: it pulls common.h ahead of Model.h, which is the
 * Matrix4x3-spelling rule its own header comment records. */
#include "dBgActor_c.h"
#include "dBgW.h"
#include "dBgCh_Lin.h"
#include "Sound.h"

/* ROM symbols with no header of their own. Spelled to agree with their
 * definitions (const Vector3 * Vec3 pairs, void * rotation target); the two
 * mangled names keep TU-local scalar spellings per the leftovers above. */
extern "C" {
/* Forward: UpdateKillByMegaChar calls down to it (reverse layout). */
void func_ov002_020ee5d0(dBgActor_c *self, int pivotY);

void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(dBgActor_c *self, Vector3 *pos, int magnitude);
s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12i x, Fix12i y, Fix12i z);
int Vec3_Dist(const Vector3 *a, const Vector3 *b);
void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
void Vec3_Add(Vector3 *out, Vector3 *a, Vector3 *b);
void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *dst);
void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_FromRotationY(void *m, int angleY);
void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, int x, int y, int z);
unsigned char DecIfAbove0_Byte(unsigned char *p);
extern Matrix4x3 data_020a0e68;
}

/* Base step, vptr store, then mModel and mMeshCollider in declaration order:
 * the compiler's own sequence, so the body is empty. */
// @symbol _ZN10dBgActor_cC2Ev
dBgActor_c::dBgActor_c()
{
}

/* Same enable/disable-by-distance as IsClsnInRange, except an off-screen actor
 * (mFlags & 8) always drops its collider, and radius 0 unconditionally enables
 * instead of falling back to the clip volume. */
// @symbol _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_
extern "C" {
int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(dBgActor_c *self, int radius, int yOffset)
{
    int offScreen = (self->mFlags & 8) != 0;
    if (offScreen) {
        if (self->mMeshCollider.IsEnabled())
            self->mMeshCollider.Disable();
        return 0;
    }
    if (radius == 0) {
        if (!self->mMeshCollider.IsEnabled())
            self->mMeshCollider.Enable(self);
    } else {
        Vector3 center;
        center.x = self->mPosX;
        center.y = self->mPosY;
        center.z = self->mPosZ;
        if (yOffset == 0)
            center.y += self->mClipOffsetY;
        else
            center.y += yOffset;
        Player *player = self->ClosestPlayer();
        int dist = Vec3_Dist(&center, (Vector3 *)((char *)player + 0x5c));
        if (dist > radius) {
            if (self->mMeshCollider.IsEnabled())
                self->mMeshCollider.Disable();
            return 0;
        }
        if (!self->mMeshCollider.IsEnabled())
            self->mMeshCollider.Enable(self);
    }
    return 1;
}
}

/* Keeps the mesh collider in the world only while the closest player is within
 * radius of (pos + yOffset). Zero radius falls back to the clip volume
 * (mClipRadius, shifted into fix12); zero yOffset to mClipOffsetY. */
// @symbol _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_
extern "C" {
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int radius, int yOffset)
{
    Vector3 center;
    center.x = self->mPosX;
    center.y = self->mPosY;
    center.z = self->mPosZ;
    if (radius == 0)
        radius = self->mClipRadius << 3;
    if (yOffset == 0)
        center.y += self->mClipOffsetY;
    else
        center.y += yOffset;
    Player *player = self->ClosestPlayer();
    int dist = Vec3_Dist(&center, (Vector3 *)((char *)player + 0x5c));
    if (dist > radius) {
        if (self->mMeshCollider.IsEnabled())
            self->mMeshCollider.Disable();
        return 0;
    }
    if (!self->mMeshCollider.IsEnabled())
        self->mMeshCollider.Enable(self);
    return 1;
}
}

/* Rebuilds the model matrix from the yaw and drops the position into its
 * translation row at 1/8 scale, the model-space unit. */
// @symbol _ZN10dBgActor_c21UpdateModelPosAndRotYEv
void dBgActor_c::UpdateModelPosAndRotY()
{
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.m[9]  = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

/* The collider follows the model: copy its matrix, overwrite the translation
 * row (m[9..11]) with the actor position, and hand it to the collider. */
// @symbol _ZN10dBgActor_c19UpdateClsnPosAndRotEv
void dBgActor_c::UpdateClsnPosAndRot()
{
    mClsnMat = mModel.mat4x3;
    mClsnMat.m[9]  = mPosX;
    mClsnMat.m[10] = mPosY;
    mClsnMat.m[11] = mPosZ;
    mMeshCollider.Transform(mClsnMat, mAngleY);
}

/* One frame of the mega-knockback flight KillByMegaChar starts: probe 200
 * units ahead along the launch heading and reverse it into a wall, tumble by
 * the per-frame angle steps, integrate, and Kill() when the fuse runs out.
 * Callers (pushblock, signpost, ski lift, Bill Blaster) pass the X step and a
 * model-pivot height; nonzero return means "still flying". */
// @symbol _ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE
int dBgActor_c::UpdateKillByMegaChar(s16 angStepX, s16 angStepY, s16 angStepZ, Fix12<int> pivotY)
{
    Vector3 probe;
    Vector3 rayEnd;
    Vector3 sum;

    if (unk_31c == 0)
        return 0;

    probe.x = 0;
    probe.y = 0;
    probe.z = 0xc8000;
    rayEnd.x = 0;
    rayEnd.y = 0;
    rayEnd.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, mPrevAngleY);
    MulVec3Mat4x3(&probe, &data_020a0e68, &rayEnd);
    Vec3_Add(&sum, (Vector3 *)&mPosX, &rayEnd);
    rayEnd = sum;

    dBgCh_Lin ray;
    ray.SetObjAndLine(*(Vector3 *)&mPosX, rayEnd, this);
    if (ray.DetectClsn())
        mPrevAngleY += 0x8000;
    mAngleX += angStepX;
    mAngleY += angStepY;
    mAngleZ += angStepZ;
    UpdatePos(0);
    if (DecIfAbove0_Byte(&unk_31d) == 0)
        Kill();
    func_ov002_020ee5d0(this, pivotY.val);
    return 1;
}

/* The mega-knockback model matrix: actor position at 1/8 scale, rotated about
 * a pivot pivotY above it (translate up, rotate, translate back). */
// @symbol func_ov002_020ee5d0
extern "C" {
void func_ov002_020ee5d0(dBgActor_c *self, int pivotY)
{
    Vector3 modelPos;
    Vec3_Asr(&modelPos, (Vector3 *)&self->mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, modelPos.x, modelPos.y, modelPos.z);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, pivotY >> 3, 0);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68,
        self->mAngleX, self->mAngleY, self->mAngleZ);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, -pivotY >> 3, 0);
    self->mModel.mat4x3 = data_020a0e68;
}
}

/* Slot 31, the one virtual this class adds (and the key function). Poof 100
 * units up, dust at the actor, sound, gone.
 *
 * MEMBERWISE, NOT `dustPos = pos`. Vector3 declares a destructor (see
 * types.h), so it is non-POD, and a whole-object assignment compiles to an
 * ldm/stm pair -- four instructions where the ROM has six. Three field
 * stores are what the cartridge does. */
// @symbol _ZN10dBgActor_c4KillEv
void dBgActor_c::Kill()
{
    Vector3 pos;
    Vector3 dustPos;
    Fix12i x = mPosX;
    Fix12i y = mPosY + 0x64000;
    Fix12i z = mPosZ;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xa, pos.x, pos.y, pos.z);
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    MarkForDestruction();
}

/* Mega Mario's shockwave hits: shake the ground, arm the knockback flight
 * (20 u/frame away from the player, 30 up, gravity 2, 30-frame fuse), and
 * take the mesh collider out of the world. */
// @symbol _ZN10dBgActor_c14KillByMegaCharER6Player
void dBgActor_c::KillByMegaChar(Player &player)
{
    Vector3 v;
    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(this, &v, 0x5dc000);
    unk_31c = 1;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHorzSpeed = 0x14000;
    mVertSpeed = 0x1e000;
    mPrevAngleY = Vec3_HorzAngle((Vector3 *)((char *)&player + 0x5c), (Vector3 *)&mPosX);
    unk_31d = 0x1e;
    if (mMeshCollider.IsEnabled() == 0)
        return;
    mMeshCollider.Disable();
}

/* Nothing forces D0/D1 here: this TU defines Kill (slot 31), the class's key
 * function, so the vtable is emitted here and drags both destructor variants
 * with it. */
// @symbol _ZN10dBgActor_cD0Ev
// @symbol _ZN10dBgActor_cD1Ev
