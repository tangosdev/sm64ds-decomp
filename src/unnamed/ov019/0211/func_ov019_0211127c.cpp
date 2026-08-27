//cpp
// @symbol func_ov019_0211127c
/* recovered: shared common types */
#include "common.h"

typedef int Fix12i;

struct PathPtr {
    void GetNode(Vector3 &, unsigned int) const;
};

extern "C" Fix12i Vec3_HorzDist(const Vector3 *a, const Vector3 *b);

extern "C" int func_ov019_0211127c(char *c, Vector3 *arg, unsigned int j) {
    Vector3 node;
    ((PathPtr *)(c + 0x364))->GetNode(node, j);
    Fix12i dist = Vec3_HorzDist(&node, arg);
    return dist <= 0x320000 ? 1 : 0;
}
