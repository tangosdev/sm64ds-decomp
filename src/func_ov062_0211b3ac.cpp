//cpp
#include "types.h"
// @symbol func_ov062_0211b3ac
/* recovered: shared common types */
#include "common.h"
struct PathPtr { int a, b; };
extern "C" void* _ZN5Actor13ClosestPlayerEv(void* self);
extern "C" void _ZN7PathPtrC1Ev(PathPtr* self);
extern "C" void _ZN7PathPtr6FromIDEj(PathPtr* self, u32 id);
extern "C" void _ZNK7PathPtr7GetNodeER7Vector3j(PathPtr* self, Vector3* v, u32 i);
extern "C" int Vec3_HorzDist(const Vector3* a, const Vector3* b);
extern signed char data_0209f2f8;
extern "C" int func_ov062_0211b3ac(char* sl)
{
    char* player;
    PathPtr path;
    Vector3 node;
    Vector3 best;
    Vector3 ppos;
    int i;
    int bestIdx;
    int bestDist;

    {
        int *ctr = (int *)(sl + 0x460);
        *ctr = *ctr + 1;
        *ctr = *ctr & 7;
    }
    if (*(int *)(sl + 0x460) != 0)
        return 0;

    player = (char*)_ZN5Actor13ClosestPlayerEv(sl);
    _ZN7PathPtrC1Ev(&path);
    _ZN7PathPtr6FromIDEj(&path, *(u32*)(sl + 0x464));

    bestIdx = 0;
    node.x = bestIdx; node.y = bestIdx; node.z = bestIdx;
    best.x = bestIdx; best.y = bestIdx; best.z = bestIdx;

    if (player != 0) {
        int *pp = (int *)(player + 0x5c);
        ppos.x = *pp;
        i = bestIdx;
        ppos.y = pp[1];
        bestDist = 0x10000000;
        ppos.z = pp[2];
        while (i < 4) {
            _ZNK7PathPtr7GetNodeER7Vector3j(&path, &node, (u32)i);
            if (bestDist > Vec3_HorzDist(&node, &ppos)) {
                bestIdx = i;
                bestDist = Vec3_HorzDist(&node, &ppos);
                best = node;
            }
            i = i + 1;
        }
    }

    if (Vec3_HorzDist(&best, (Vector3*)(sl + 0x430)) == 0)
        return 0;

    if (data_0209f2f8 == 0x10) {
        *(unsigned char*)(sl + 0x446) = 0;
        if (*(int*)(sl + 0x474) == (bestIdx ^ 2))
            *(unsigned char*)(sl + 0x446) = 1;
    }
    *(int*)(sl + 0x474) = bestIdx;
    return 1;
}
