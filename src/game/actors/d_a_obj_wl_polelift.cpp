//cpp
/* leftovers:
 * - common.h stays first.
 * - The destructor stays inline. Out of line it emits D0 before D1,
 *   and a D2.
 * - dCcAcPos_c::Init is not on the header, and a by-value Fix12
 *   changes InitResources. DropShadowScaleXYZ takes three Fix12<int>
 *   by value.
 * - func_ov026_02111234 and func_ov026_021112a4 stay address-named
 *   C-linkage helpers.
 * - data_ov026_02113ea0 is the BMD handle and data_ov026_02113a9c
 *   the collision offset. data_0209caa0 is the stage-state block.
 *   g_profile_WL_POLELIFT is outside this TU.
 *
 * daObjWlPolelift_c, the Wet-Dry World pole lift. It rides the path
 * in param1 at 10.0 a frame, pauses 60 frames at a dead end, and
 * drops a shadow. InitResources requires a bit of 0x204 in
 * data_0209caa0[1]. .text 0x021111a0..0x021116c8. _ZTS17daObjWlPolelift_c
 * and the vtable at 0x02113ae0 name the class. classInit is
 * reconstructed.
 */
/* common.h FIRST: func_ov026_02111234 whole-struct-assigns the model matrix
 * into the drop-shadow matrix, and the cartridge does it as one flat
 * twelve-word ldm/stm run. common.h's flat `s32 m[12]` spelling of Matrix4x3
 * is that copy; math/Matrix.h's nested {Matrix3x3 r; Vector3 t;} -- which
 * Model.h would otherwise pull in first -- scalarizes the last three words,
 * because Vector3 declares a destructor and is therefore not POD. Measured:
 * 0x88 emitted against the cartridge's 0x70. */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_wl_polelift_AssetId {
    ASSET_WL_POLE_LIFT_BMD = 0x06db
};

#include "common.h"
#include "daObjWlPolelift_c.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"

/* The file-scope object at the end of this file constructs the resource
 * handle (model file 0x6db). mwcc emits __sinit_d_a_obj_wl_polelift.cpp
 * from that definition. The wrapper name is local; the handle constructor
 * and destructor are the ROM resource-family functions, aliased in the
 * manifest. */
struct PoleliftModelFile : SharedFilePtr {
    u32 words[2];

    PoleliftModelFile(u32 fileID);
    ~PoleliftModelFile();
};

/* The class vtable. mwccarm's own emitted _ZTV symbol addresses the vtable
 * object; the address recorded in ov026/symbols.txt is the public address
 * point eight bytes later, which is exactly what &_ZTV[2] on an int[]
 * computes. */
extern int _ZTV17daObjWlPolelift_c[];

namespace cstd {
int fdiv(int a, int b);
}

extern "C" {
/* Factory ABI: the allocation goes through fBase_c::operator new by name, but
 * the base/subobject construction sequence stays explicit here. */
void *_ZN8dActor_cC2Ev(void *self);
void *_ZN5ModelC1Ev(void *self);
void *_ZN10dCcAcPos_cC1Ev(void *self);
void *_ZN7PathPtrC1Ev(void *self);
void *_ZN17dExtShadowModel_cC1Ev(void *self);

/* ABI seams: both take Fix12<int> by value. */
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c *self, dActor_c *actor, const Vector3 *offset,
    Fix12i radius, Fix12i height, u32 flags, u32 vulnFlags);
void _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *shadow, void *mtx, int x, int y, int z, unsigned int flags);

u16 DecIfAbove0_Short(u16 *timer);
void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
int LenVec3(Vector3 *v);
void Vec3_MulScalar(Vector3 *out, Vector3 *a, int scale);
void SubVec3(Vector3 *out, Vector3 *a, Vector3 *b);
void Matrix4x3_FromRotationY(void *mtx, int angleY);

void func_ov026_02111234(daObjWlPolelift_c *actor);
void func_ov026_021112a4(daObjWlPolelift_c *actor);

extern u32 data_0209caa0[];
extern Vector3 data_ov026_02113a9c;
extern PoleliftModelFile data_ov026_02113ea0;
}

// @symbol daObjWlPolelift_c_classInit
extern "C" daObjWlPolelift_c *daObjWlPolelift_c_classInit()
{
    daObjWlPolelift_c *actor =
        (daObjWlPolelift_c *)fBase_c::operator new(sizeof(daObjWlPolelift_c));
    if (actor) {
        _ZN8dActor_cC2Ev(actor);
        *(int *)actor = (int)&_ZTV17daObjWlPolelift_c[2];
        _ZN5ModelC1Ev(&actor->model);
        _ZN10dCcAcPos_cC1Ev(&actor->clsn);
        _ZN7PathPtrC1Ev(&actor->path);
        _ZN17dExtShadowModel_cC1Ev(&actor->shadow);
    }
    return actor;
}

// @symbol _ZN17daObjWlPolelift_c13InitResourcesEv
/* Load the lift model, bind its path and collision state, and initialize the
 * actor-owned shadow and cylinder. */
s32 daObjWlPolelift_c::InitResources()
{
    model.SetFile((BMD_File *)Model::LoadFile(data_ov026_02113ea0), 1, -1);
    if ((data_0209caa0[1] & 0x204) == 0)
        return 0;

    shadow.InitCuboid();
    path.FromID(param1 & 0xff);
    numNodes = path.NumNodes();
    moveSpeed = 1;

    Vector3 offset;
    offset.x = data_ov026_02113a9c.x;
    offset.y = data_ov026_02113a9c.y;
    offset.z = data_ov026_02113a9c.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &clsn, this, &offset, 0x50000, 0x12c000, 0x80000c, 0);
    func_ov026_021112a4(this);
    return 1;
}

// @symbol _ZN17daObjWlPolelift_c8BehaviorEv
/* Move toward the current path node, reverse or wrap at path ends, and keep
 * the owned collision and shadow state aligned with the lift. */
s32 daObjWlPolelift_c::Behavior()
{
    Vector3 node;
    Vector3 prevNode;
    Vector3 delta;
    Vector3 rayPos;
    Vector3 step;
    Vector3 clsnOffset;
    int moved;
    int prevNodeIdx;
    int speed;
    int current;
    int length;
    int scale;
    int skipCollision;

    if (DecIfAbove0_Short((u16 *)&bounceTimer) == 0) {
        speed = moveSpeed;
        current = curNodeIdx;
        prevNodeIdx = current - speed;
        moved = 0;
        if (speed > 0) {
            if (prevNodeIdx < 0)
                prevNodeIdx = numNodes - 1;
        } else if (prevNodeIdx >= numNodes) {
            prevNodeIdx = moved;
        }

        path.GetNode(node, current);
        path.GetNode(prevNode, prevNodeIdx);
        Vec3_Sub(&delta, &mPos, &node);
        length = LenVec3(&delta);
        if (length == 0 || length <= 0xa000) {
            mPosX = node.x;
            moved = 1;
            mPosY = node.y;
            mPosZ = node.z;
        } else {
            scale = cstd::fdiv(0xa000, length);
            Vec3_MulScalar(&step, &delta, scale);
            SubVec3(&mPos, &step, &mPos);
        }

        if (moved) {
            curNodeIdx += moveSpeed;
            if (curNodeIdx < 0) {
                if (path.Loops()) {
                    curNodeIdx = numNodes - 1;
                } else {
                    bounceTimer = 0x3c;
                    moveSpeed = 1;
                    curNodeIdx += moveSpeed << 1;
                }
            }
            if (curNodeIdx >= numNodes) {
                if (path.Loops()) {
                    curNodeIdx = 0;
                } else {
                    bounceTimer = 0x3c;
                    moveSpeed = -1;
                    curNodeIdx += moveSpeed << 1;
                }
            }
        }
    }

    clsn.Clear();
    skipCollision = (mFlags & 8) != 0;
    if (!skipCollision) {
        rayPos = mPos;
        rayPos.y -= 0x14000;

        dBgCh_Gnd ray;
        ray.SetObjAndPos(rayPos, 0);
        groundY = rayPos.y;
        if (ray.DetectClsn())
            groundY = ray.clsnY;

        clsnOffset = data_ov026_02113a9c;
        clsn.SetPosRelativeToActor(clsnOffset);
        clsn.Update();
        func_ov026_021112a4(this);
        func_ov026_02111234(this);
    }
    return 1;
}

// @symbol _ZN17daObjWlPolelift_c6RenderEv
/* Vtable slot 9. The owned Model renders through its own vtable. */
s32 daObjWlPolelift_c::Render()
{
    model.Render(0);
    return 1;
}

// @symbol _ZN17daObjWlPolelift_c16CleanupResourcesEv
/* Vtable slot 3. Releases the one shared file the class holds and reports
 * success; it never touches the object. */
s32 daObjWlPolelift_c::CleanupResources()
{
    data_ov026_02113ea0.Release();
    return 1;
}

// @symbol func_ov026_021112a4
/* File-local helper: rebuild the model matrix from the actor's Y rotation and
 * position. */
extern "C" void func_ov026_021112a4(daObjWlPolelift_c *actor)
{
    Matrix4x3_FromRotationY(&actor->model.mat4x3, actor->mAngleY);
    actor->model.mat4x3.m[9] = actor->mPosX >> 3;
    actor->model.mat4x3.m[10] = actor->mPosY >> 3;
    actor->model.mat4x3.m[11] = actor->mPosZ >> 3;
}

// @symbol func_ov026_02111234
/* File-local helper: copy the model matrix into the drop-shadow matrix and
 * hand the owned dExtShadowModel_c to dActor_c::DropShadowScaleXYZ. */
extern "C" void func_ov026_02111234(daObjWlPolelift_c *actor)
{
    actor->dropShadowMtx = actor->model.mat4x3;
    actor->dropShadowMtx.m[10] = actor->groundY >> 3;
    _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
        actor, &actor->shadow, &actor->dropShadowMtx, 0x64000, 0xc8000, 0x64000, 0xf);
}

/* _ZN17daObjWlPolelift_cD1Ev and _ZN17daObjWlPolelift_cD0Ev are NOT written
 * here on purpose, and carry no @symbol marker: the inline destructor in
 * include/daObjWlPolelift_c.h emits D1 then D0 -- the cartridge's order -- and
 * no D2. tools/tiers.py scores both through that inline definition. */

/* Static-init global (was the handwritten __sinit_ov026_02112b7c shard):
 * the model handle (file 0x6db). */
PoleliftModelFile data_ov026_02113ea0(ASSET_WL_POLE_LIFT_BMD);
