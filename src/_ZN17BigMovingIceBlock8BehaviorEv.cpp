//cpp
// @symbol _ZN17BigMovingIceBlock8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_PathPtr.h"
/* recovered: named members + shared header, real C++ method */
#include "BigMovingIceBlock.h"
typedef int Fix12;
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *p, void *out, unsigned idx);
extern void Vec3_Sub(void *out, void *a, void *b);
extern int LenVec3(void *v);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Vec3_MulScalar(void *out, void *v, int s);
extern void SubVec3(void *a, void *b, void *c);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *p);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *p);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *p, Fix12 a, int b);

struct V3 { int x, y, z; };

int BigMovingIceBlock::Behavior()
{
    struct V3 prev;
    struct V3 node;
    struct V3 diff;
    struct V3 scaled1;
    struct V3 scaled2;
    int looped;
    int delta;
    int len;

    mHorzSpeed = 0xa000;
    delta = mPathNodeIdx - mPathDir;
    looped = 0;

    if (_ZNK7PathPtr5LoopsEv((char *)&mPath)) {
        if (mPathDir > 0) {
            if (delta < 0)
                delta = _ZNK7PathPtr8NumNodesEv((char *)&mPath) - 1;
        } else {
            if (delta >= _ZNK7PathPtr8NumNodesEv((char *)&mPath))
                delta = looped;
        }
        _ZNK7PathPtr7GetNodeER7Vector3j(((char *)this) + 0x320, &node, delta);
    } else {
        if (delta < 0 || delta >= _ZNK7PathPtr8NumNodesEv((char *)&mPath)) {
            node.x = mPosX;
            node.y = mPosY;
            node.z = mPosZ;
        } else {
            _ZNK7PathPtr7GetNodeER7Vector3j(((char *)this) + 0x320, &node, delta);
        }
    }

    _ZNK7PathPtr7GetNodeER7Vector3j(((char *)this) + 0x320, &prev, mPathNodeIdx);
    Vec3_Sub(&diff, ((char *)this) + 0x5c, &prev);
    len = LenVec3(&diff);
    if (len == 0 || len <= mHorzSpeed) {
        int q = _ZN4cstd4fdivEii(mHorzSpeed, len);
        Vec3_MulScalar(&scaled1, &diff, q);
        SubVec3(((char *)this) + 0x5c, &scaled1, ((char *)this) + 0x5c);
        looped = 1;
    } else {
        int q = _ZN4cstd4fdivEii(mHorzSpeed, len);
        Vec3_MulScalar(&scaled2, &diff, q);
        SubVec3(((char *)this) + 0x5c, &scaled2, ((char *)this) + 0x5c);
    }

    if (looped) {
        int *pn = (int *)(((int)((char *)this) + 0x328));
        *pn = *pn + mPathDir;
        if (mPathNodeIdx < 0) {
            if (_ZNK7PathPtr5LoopsEv((char *)&mPath))
                mPathNodeIdx = _ZNK7PathPtr8NumNodesEv((char *)&mPath) - 1;
            else {
                int *pn2;
                mPathDir = 1;
                pn2 = (int *)(((unsigned int)((char *)this) + 0x328));
                *pn2 = *pn2 + mPathDir * 2;
            }
        }
        if (mPathNodeIdx >= _ZNK7PathPtr8NumNodesEv((char *)&mPath)) {
            if (_ZNK7PathPtr5LoopsEv((char *)&mPath))
                mPathNodeIdx = 0;
            else {
                int *pn3;
                mPathDir = -1;
                pn3 = (int *)(((int)((char *)&mPathNodeIdx)));
                *pn3 = *pn3 + mPathDir * 2;
            }
        }
    }

    _ZN8Platform21UpdateModelPosAndRotYEv(((char *)this));
    _ZN8Platform19UpdateClsnPosAndRotEv(((char *)this));
    _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char *)this), 0x5dc000, 0);
    return 1;
}
