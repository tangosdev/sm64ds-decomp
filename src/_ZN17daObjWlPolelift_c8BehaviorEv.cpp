//cpp
// @symbol _ZN17daObjWlPolelift_c8BehaviorEv
/* Move toward the current path node, reverse or wrap at path ends, and keep
 * the owned collision/shadow state aligned with the lift. */
#include "daObjWlPolelift_c.h"
#include "dBgCh_Gnd.h"

namespace cstd {
int fdiv(int a, int b);
}

extern "C" {
u16 DecIfAbove0_Short(u16 *timer);
void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
int LenVec3(Vector3 *v);
void Vec3_MulScalar(Vector3 *out, Vector3 *a, int scale);
void SubVec3(Vector3 *out, Vector3 *a, Vector3 *b);
void func_ov026_021112a4(daObjWlPolelift_c *actor);
void func_ov026_02111234(daObjWlPolelift_c *actor);
}

extern Vector3 data_ov026_02113a9c;

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
