//cpp
// @symbol _ZN8MantaRay8BehaviorEv
/* recovered: named members + shared header, real C++ method
 *
 * One frame of the ray, and it is a PATH FOLLOWER rather than a wanderer.
 *
 * mPathNode indexes the path unk_37c names. Within 0x258000 of that node it
 * advances, wrapping at unk_380 -- the node count InitResources cached once.
 * A distance of exactly zero counts as "arrived" alongside the radius test, so
 * a node the ray is sitting on cannot stall it.
 *
 * The heading then EASES toward the node: yaw at 0x60 a frame, pitch at 0x40,
 * so the ray turns flat faster than it climbs. Those eased values live in the
 * mPrev* slots and are published into mAngleX/Y/Z afterwards.
 *
 * Motion falls out of the heading rather than being tracked separately. A
 * matrix built from the eased yaw and pitch turns a fixed 0xa000 forward vector
 * into a velocity written across 0x0a4..0x0b0 -- and only then is gravity
 * applied to its y, as max(mTerminalVelocity, mVertSpeed + mVertAccel). See MantaRay.h: those
 * are Actor's speed slots, repurposed as a vector.
 */
#include "MantaRay.h"
struct C3;
typedef void (C3::*Fn)();
struct Obj { char pad[8]; Fn fn; };

struct PathPtr {
    char pad[8];
    PathPtr();
    void FromID(unsigned int id);
    void GetNode(Vector3& out, unsigned int idx) const;
};

extern "C" {
unsigned short DecIfAbove0_Short(unsigned short* p);
void Vec3_Sub(Vector3* out, Vector3* a, Vector3* b);
int LenVec3(Vector3* v);
short Vec3_HorzAngle(Vector3* v0, Vector3* v1);
short Vec3_VertAngle(Vector3* v0, Vector3* v1);
void Matrix4x3_FromRotationY(void* m, int angle);
void Matrix4x3_ApplyInPlaceToRotationX(void* m, short angle);
void MulVec3Mat4x3(Vector3* v, void* m, Vector3* out);
void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void* thiz, void* clsn);
void func_ov090_02132b14(void* c);
void _ZN12CylinderClsn5ClearEv(void* c);
void _ZN12CylinderClsn6UpdateEv(void* c);
extern char data_020a0e68[];
}

void ApproachLinear(short& v, short target, short step);

int MantaRay::Behavior()
{
    char* c = (char*)this;
    DecIfAbove0_Short((unsigned short*)&unk_100);
    {
        Obj* o = *(Obj**)&unk_370;
        if (*(int*)((char*)o + 8) != 0) {
            (((C3*)c)->*(o->fn))();
        }
    }
    {
        PathPtr p;
        Vector3 node;
        Vector3 diff;
        Vector3 v;
        int len;

        p.FromID(*(unsigned int*)&unk_37c);
        p.GetNode(node, *(unsigned int*)&mPathNode);
        Vec3_Sub(&diff, (Vector3*)&mPosX, &node);
        len = LenVec3(&diff);
        if (len == 0 || len <= 0x258000) {
            mPathNode++;
            if (mPathNode >= unk_380)
                mPathNode = 0;
        }
        ApproachLinear(mPrevAngleY, Vec3_HorzAngle((Vector3*)&mPosX, &node), 0x60);
        ApproachLinear(mPrevAngleX, Vec3_VertAngle((Vector3*)&mPosX, &node), 0x40);
        mAngleX = mPrevAngleX;
        mAngleY = mPrevAngleY;
        mAngleZ = mPrevAngleZ;
        v.y = v.x = v.z = 0;
        v.z = 0xa000;
        Matrix4x3_FromRotationY(data_020a0e68, mPrevAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, mPrevAngleX);
        MulVec3Mat4x3(&v, data_020a0e68, (Vector3*)&unk_0a4);
    }
    {
        int s = mVertSpeed + mVertAccel;
        int m2 = mTerminalVelocity;
        int ac = unk_0ac;
        if (s >= m2) m2 = s;
        mVertSpeed = m2;
        unk_0ac = ac;
    }
    _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(c, &mMovingCylinderClsnWithPos);
    func_ov090_02132b14(c);
    _ZN12CylinderClsn5ClearEv(&mMovingCylinderClsnWithPos);
    _ZN12CylinderClsn6UpdateEv(&mMovingCylinderClsnWithPos);
    return 1;
}
