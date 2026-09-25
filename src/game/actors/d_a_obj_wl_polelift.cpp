//cpp
/**
 * daObjWlPolelift_c -- the Wet-Dry World pole lift (WL_POLELIFT), ov026.
 *
 * It rides path param1 & 0xff at 10.0 a frame, node to node. On a looping
 * path it wraps; otherwise it pauses 60 frames at each end and reverses. It
 * drops a shadow onto the ground a ray finds below it. InitResources fails
 * unless the stage-state word data_0209caa0[1] has a bit of 0x204 set.
 *
 * One translation unit for the whole 0x021111a0..0x021116c8 linker run: the
 * destructor pair, the two file-local helpers the cartridge kept unnamed, the
 * four virtual overrides, and the class initializer.
 *
 * daObjWlPolelift_c_classInit is reconstructed -- the ROM proves the class
 * through RTTI (_ZTS17daObjWlPolelift_c), the 0x1e4 allocation, the vtable at
 * 0x02113ae0 and the WL_POLELIFT registry profile, but retail stores no
 * spelling for the factory itself. Historical placeholder:
 * func_ov026_02111678. Coined C name; no param-type claim.
 *
 * DO NOT "TIDY" THESE -- each one is load-bearing:
 *   common.h first (see the note on the includes).
 *   The destructor is inline: out of line it emits D0 before D1, plus a D2.
 *
 * WHY SOME CALLS ARE SPELLED AS MANGLED SYMBOLS (Fix12<int> by value, see
 * notes/mwccarm-codegen.md 6az):
 *   dCcAcPos_c::Init is not declared on dCcAcPos_c.h, and a by-value Fix12
 *   homes the argument and changes the size of InitResources.
 *   dActor_c::DropShadowScaleXYZ takes three Fix12<int> by value.
 *
 * Known limits:
 *   func_ov026_02111234 and func_ov026_021112a4 keep their placeholder names
 *   and char-cursor bodies. Both are file-local helpers over fields this
 *   class still models as padding (0x0f0 model matrix, 0x114, 0x1d8); naming
 *   them belongs with the member pass that names those fields.
 *   data_ov026_02113ea0 is this overlay's BMD handle and data_ov026_02113a9c
 *   the collision offset vector, in overlay .data this TU does not own.
 *   data_0209caa0 is the arm9 stage-state block; naming belongs with it.
 *   g_profile_WL_POLELIFT lives outside this TU.
 */

/* common.h FIRST: func_ov026_02111234 whole-struct-assigns the model matrix
 * into the drop-shadow matrix, and the cartridge does it as one flat
 * twelve-word ldm/stm run. common.h's flat `s32 m[12]` spelling of Matrix4x3
 * is that copy; math/Matrix.h's nested {Matrix3x3 r; Vector3 t;} -- which
 * Model.h would otherwise pull in first -- scalarizes the last three words,
 * because Vector3 declares a destructor and is therefore not POD. Measured:
 * 0x88 emitted against the cartridge's 0x70. */
#include "common.h"
#include "daObjWlPolelift_c.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"

/* The class vtable. mwccarm's own emitted _ZTV symbol addresses the vtable
 * object; the address recorded in ov026/symbols.txt is the public address
 * point eight bytes later, which is exactly what &_ZTV[2] on an int[]
 * computes. */
extern int _ZTV17daObjWlPolelift_c[];

namespace cstd {
int fdiv(int a, int b);
}

extern "C" {
/* Factory ABI: CodeWarrior rejects the actor hierarchy's class-specific
 * operator-new declaration, so the allocation and the base/subobject
 * construction sequence stay explicit here. */
void _ZN8dActor_cC2Ev(void *self);
void _ZN5ModelC1Ev(void *self);
void _ZN10dCcAcPos_cC1Ev(void *self);
void _ZN7PathPtrC1Ev(void *self);
void _ZN11ShadowModelC1Ev(void *self);

/* ABI seams: both take Fix12<int> by value. */
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c *self, dActor_c *actor, const Vector3 *offset,
    Fix12i radius, Fix12i height, u32 flags, u32 vulnFlags);
void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
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
extern SharedFilePtr data_ov026_02113ea0;
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
        _ZN11ShadowModelC1Ev(&actor->shadow);
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
        Vec3_Sub(&delta, (Vector3 *)&mPosX, &node);
        length = LenVec3(&delta);
        if (length == 0 || length <= 0xa000) {
            mPosX = node.x;
            moved = 1;
            mPosY = node.y;
            mPosZ = node.z;
        } else {
            scale = cstd::fdiv(0xa000, length);
            Vec3_MulScalar(&step, &delta, scale);
            SubVec3((Vector3 *)&mPosX, &step, (Vector3 *)&mPosX);
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
        rayPos = *(Vector3 *)&mPosX;
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
 * position. The fields it walks are still modelled as padding. */
extern "C" void func_ov026_021112a4(daObjWlPolelift_c *actor)
{
    char *t = (char *)actor;
    Matrix4x3_FromRotationY(t + 0xf0, *(short *)(t + 0x8e));
    *(int *)(t + 0x114) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x118) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x11c) = *(int *)(t + 0x64) >> 3;
}

// @symbol func_ov026_02111234
/* File-local helper: copy the model matrix into the drop-shadow matrix and
 * hand the owned ShadowModel to dActor_c::DropShadowScaleXYZ. */
extern "C" void func_ov026_02111234(daObjWlPolelift_c *actor)
{
    char *c = (char *)actor;
    *(struct Matrix4x3 *)(c + 0x1b0) = *(struct Matrix4x3 *)(c + 0xf0);
    *(int *)(c + 0x1d8) = *(int *)(c + 0x1e0) >> 3;
    _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        c, c + 0x188, c + 0x1b0, 0x64000, 0xc8000, 0x64000, 0xf);
}

/* _ZN17daObjWlPolelift_cD1Ev and _ZN17daObjWlPolelift_cD0Ev are NOT written
 * here on purpose, and carry no @symbol marker: the inline destructor in
 * include/daObjWlPolelift_c.h emits D1 then D0 -- the cartridge's order -- and
 * no D2. tools/tiers.py scores both through that inline definition. */
