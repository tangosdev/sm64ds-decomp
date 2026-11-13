//cpp
/* leftovers:
 * - common.h stays first.
 * - The destructor stays inline. Out of line it emits D0 before D1,
 *   and a D2.
 * - data_ov026_02113ea0 is the BMD handle and data_ov026_02113a9c
 *   the collision offset. data_0209caa0 is the save block.
 *   g_profile_WL_POLELIFT is outside this TU.
 *
 * daObjWlPolelift_c, the Wet-Dry World pole lift. It rides the path
 * in param1 at 10.0 a frame, pauses 60 frames at a dead end, and
 * drops a shadow. InitResources requires a bit of 0x204 in
 * data_0209caa0.file.flags1. .text 0x021111a0..0x021116c8. _ZTS17daObjWlPolelift_c
 * and the vtable at 0x02113ae0 name the class. classInit is
 * reconstructed.
 */
/* common.h FIRST: UpdateShadow whole-struct-assigns the model matrix
 * into the drop-shadow matrix, and the cartridge does it as one flat
 * twelve-word ldm/stm run. common.h's flat `s32 m[12]` spelling of Matrix4x3
 * is that copy; math/Matrix.h's nested {Matrix3x3 r; Vector3 t;} -- which
 * dExtSimpleModel_c.h would otherwise pull in first -- scalarizes the last three words,
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
#include "math/MtxStack.h"
#include "math/CalcLib.h"
#include "SaveBlock.h"
#include "nitro/fx/fx_cp.h"

/* The file-scope object at the end of this file constructs the resource
 * handle (model file 0x6db). mwcc emits __sinit_d_a_obj_wl_polelift.cpp
 * from that definition. The wrapper name is local; the handle constructor
 * and destructor are the ROM resource-family functions, aliased in the
 * manifest. */
struct PoleliftModelFile : SharedFilePtr {
    PoleliftModelFile(u32 fileID);
    ~PoleliftModelFile();
};

extern "C" {

void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
void Vec3_MulScalar(Vector3 *out, const Vector3 *a, int scale);

extern Vector3 data_ov026_02113a9c;
extern PoleliftModelFile data_ov026_02113ea0;
}


extern "C" daObjWlPolelift_c *daObjWlPolelift_c_classInit()
{
    return new daObjWlPolelift_c();
}

/* Load the lift model, bind its path and collision state, and initialize the
 * actor-owned shadow and cylinder. */
s32 daObjWlPolelift_c::InitResources()
{
    model.SetFile(dExtSimpleModel_c::LoadFile(data_ov026_02113ea0), 1, -1);
    if ((data_0209caa0.file.flags1 & 0x204) == 0)
        return 0;

    shadow.InitCuboid();
    path.FromID(param1 & 0xff);
    numNodes = path.NumNodes();
    moveSpeed = 1;

    Vector3 offset;
    offset.x = data_ov026_02113a9c.x;
    offset.y = data_ov026_02113a9c.y;
    offset.z = data_ov026_02113a9c.z;
    clsn.Init(this, offset, 0x50000, 0x12c000, 0x80000c, 0);
    UpdateModelTransform();
    return 1;
}

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
        length = VEC_Mag(&delta);
        if (length == 0 || length <= 0xa000) {
            mPosX = node.x;
            moved = 1;
            mPosY = node.y;
            mPosZ = node.z;
        } else {
            scale = FX_Div(0xa000, length);
            Vec3_MulScalar(&step, &delta, scale);
            VEC_Subtract(&mPos, &step, &mPos);
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
        UpdateModelTransform();
        UpdateShadow();
    }
    return 1;
}

/* Vtable slot 9. The owned dExtSimpleModel_c renders through its own vtable. */
s32 daObjWlPolelift_c::Render()
{
    model.Render(0);
    return 1;
}

/* Vtable slot 3. Releases the one shared file the class holds and reports
 * success; it never touches the object. */
s32 daObjWlPolelift_c::CleanupResources()
{
    data_ov026_02113ea0.Release();
    return 1;
}

/* Rebuild the model matrix from the actor's Y rotation and position. */
void daObjWlPolelift_c::UpdateModelTransform() {
    Matrix4x3_FromRotationY(&model.mat4x3, mAngleY);
    model.mat4x3.m[9] = mPosX >> 3;
    model.mat4x3.m[10] = mPosY >> 3;
    model.mat4x3.m[11] = mPosZ >> 3;
}

/* Copy the model matrix into the drop-shadow matrix, drop it to the ground
 * and hand the owned dExtShadowModel_c to dActor_c::DropShadowScaleXYZ. */
void daObjWlPolelift_c::UpdateShadow() {
    dropShadowMtx = model.mat4x3;
    dropShadowMtx.m[10] = groundY >> 3;
    DropShadowScaleXYZ(shadow, dropShadowMtx, 0x64000, 0xc8000, 0x64000, 0xf);
}

/* _ZN17daObjWlPolelift_cD1Ev and _ZN17daObjWlPolelift_cD0Ev are NOT written
 * here on purpose: the inline destructor in
 * include/daObjWlPolelift_c.h emits D1 then D0 -- the cartridge's order -- and
 * no D2. tools/tiers.py scores both through that inline definition. */

/* Static-init global (was the handwritten __sinit_ov026_02112b7c shard):
 * the model handle (file 0x6db). */
PoleliftModelFile data_ov026_02113ea0(ASSET_WL_POLE_LIFT_BMD);
