//cpp
// @symbol _ZN14SquarePathLift8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_PathPtr.h"
/* recovered: named members + shared header, real C++ method */
#include "SquarePathLift.h"
extern "C" {
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *p, void *out, unsigned idx);
extern void Vec3_Sub(void *out, void *a, void *b);
extern int LenVec3(void *v);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Vec3_MulScalar(void *out, void *v, int s);
extern void SubVec3(void *a, void *b, void *c);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *p);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *p);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *p, int a, int b);
}
struct V3 { int x, y, z; };

int SquarePathLift::Behavior()
{
    struct V3 prev, node, diff, scaled1, scaled2;
    int looped, delta, len;
    mHorzSpeed = 0xa000;
    delta = mNodeIndex - mPathDir;
    looped = 0;
    if (_ZNK7PathPtr5LoopsEv((char *)&mPath)) {
        if (mPathDir > 0) { if (delta < 0) delta = _ZNK7PathPtr8NumNodesEv((char *)&mPath) - 1; }
        else { if (delta >= _ZNK7PathPtr8NumNodesEv((char *)&mPath)) delta = 0; }
        _ZNK7PathPtr7GetNodeER7Vector3j(((char *)this) + 0x320, &node, delta);
    } else {
        if (delta < 0 || delta >= _ZNK7PathPtr8NumNodesEv((char *)&mPath)) {
            node.x = mPosX; node.y = mPosY; node.z = mPosZ;
        } else _ZNK7PathPtr7GetNodeER7Vector3j(((char *)this) + 0x320, &node, delta);
    }
    _ZNK7PathPtr7GetNodeER7Vector3j(((char *)this) + 0x320, &prev, mNodeIndex);
    Vec3_Sub(&diff, ((char *)this) + 0x5c, &prev);
    len = LenVec3(&diff);
    if (len == 0 || len <= mHorzSpeed) {
        Vec3_MulScalar(&scaled1, &diff, _ZN4cstd4fdivEii(mHorzSpeed, len));
        SubVec3(((char *)this) + 0x5c, &scaled1, ((char *)this) + 0x5c);
        looped = 1;
    } else {
        Vec3_MulScalar(&scaled2, &diff, _ZN4cstd4fdivEii(mHorzSpeed, len));
        SubVec3(((char *)this) + 0x5c, &scaled2, ((char *)this) + 0x5c);
    }
    if (looped) {
        *(int *)((int)((char *)this) + 0x328) += mPathDir;
        if (mNodeIndex < 0) {
            if (_ZNK7PathPtr5LoopsEv((char *)&mPath)) {
                mNodeIndex = _ZNK7PathPtr8NumNodesEv((char *)&mPath) - 1;
            } else {
                mPathDir = 1;
                *(int *)((unsigned)((char *)this) + 0x328) += mPathDir * 2;
            }
        }
        if (mNodeIndex >= _ZNK7PathPtr8NumNodesEv((char *)&mPath)) {
            if (_ZNK7PathPtr5LoopsEv((char *)&mPath)) {
                mNodeIndex = 0;
            } else {
                mPathDir = -1;
                *(int *)((char *)&mNodeIndex) += mPathDir * 2;
            }
        }
    }
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char *)this));
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char *)this));
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char *)this), 0x320000, 0);
    return 1;
}
