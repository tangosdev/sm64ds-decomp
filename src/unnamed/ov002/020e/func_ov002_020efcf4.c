// @symbol func_ov002_020efcf4
/* recovered: shared common types */
#include "common.h"
extern int func_ov002_020efe9c(int *p);
extern int _ZNK7PathPtr8NumNodesEv(void *p);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *p, struct Vector3 *out, unsigned int idx);
extern void Vec3_Sub(struct Vector3 *out, struct Vector3 *a, struct Vector3 *b);
extern int LenVec3(struct Vector3 *v);
extern int _ZNK7PathPtr5LoopsEv(void *p);

void func_ov002_020efcf4(int *sl) {
    char *c = (char *)sl;
    int idx;
    int found;
    int i;
    int r6;
    char *obj;
    struct Vector3 node;
    int one;

    if (func_ov002_020efe9c(sl) == 0)
        return;

    found = 0;
    idx = *(int *)(c + 0x438);
    i = found;
    obj = c;
    one = 1;

    for (; i < 3; i++, obj += 0x50) {
        node.x = 0;
        node.y = 0;
        node.z = 0;

        if (i == 0) {
            if (idx < _ZNK7PathPtr8NumNodesEv(c + 0x430) && idx >= 0) {
                struct Vector3 diff;
                _ZNK7PathPtr7GetNodeER7Vector3j(c + 0x430, &node, idx);
                Vec3_Sub(&diff, &node, (struct Vector3 *)(c + 0x5c));
                if (LenVec3(&diff) < 0xc8000) {
                    if (found == 0) {
                        found = one;
                        idx += 1;
                    }
                }
            }
        }

        r6 = i * *(int *)(c + 0x43c) + idx;
        if (_ZNK7PathPtr5LoopsEv(c + 0x430)) {
            if (r6 >= _ZNK7PathPtr8NumNodesEv(c + 0x430)) {
                r6 -= _ZNK7PathPtr8NumNodesEv(c + 0x430);
            }
            if (r6 < 0) {
                r6 += _ZNK7PathPtr8NumNodesEv(c + 0x430);
            }
        } else if (r6 >= _ZNK7PathPtr8NumNodesEv(c + 0x430)) {
            continue;
        }
        if (r6 < _ZNK7PathPtr8NumNodesEv(c + 0x430) && r6 >= 0) {
            _ZNK7PathPtr7GetNodeER7Vector3j(c + 0x430, &node, r6);
            *(int *)(obj + 0x360) = node.x >> 3;
            *(int *)(obj + 0x364) = node.y >> 3;
            *(int *)(obj + 0x368) = node.z >> 3;
        }
    }
}
