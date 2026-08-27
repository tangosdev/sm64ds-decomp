// @symbol func_ov081_0212538c
/* recovered: shared common types */
#include "common.h"
struct PathPtr { int a, b; };

extern void _ZN7PathPtrC1Ev(struct PathPtr *self);
extern void _ZN7PathPtr6FromIDEj(struct PathPtr *self, unsigned int id);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(struct PathPtr *self, struct Vector3 *v, unsigned int i);
extern void Vec3_Sub(struct Vector3 *out, struct Vector3 *a, struct Vector3 *b);
extern int LenVec3(struct Vector3 *v);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a, int fix, unsigned int b);
extern void *data_ov081_02128d98[];

int func_ov081_0212538c(char *c)
{
    struct PathPtr pp;
    struct Vector3 node;
    struct Vector3 diff;
    int len;

    _ZN7PathPtrC1Ev(&pp);
    _ZN7PathPtr6FromIDEj(&pp, *(unsigned int *)(c + 0x418));
    _ZNK7PathPtr7GetNodeER7Vector3j(&pp, &node, *(unsigned int *)(c + 0x424));
    node.y = *(int *)(c + 0x60);
    Vec3_Sub(&diff, (struct Vector3 *)(c + 0x5c), &node);
    len = LenVec3(&diff);
    *(int *)(c + 0x458) = 0xa000;
    *(int *)(c + 0x408) = 0;
    if (len == 0 || len <= *(int *)(c + 0x458)) {
        *(int *)(c + 0x5c) = node.x;
        *(int *)(c + 0x60) = node.y;
        *(int *)(c + 0x64) = node.z;
        (*(int *)(c + 0x424))++;
        if (*(int *)(c + 0x424) >= *(int *)(c + 0x420))
            *(int *)(c + 0x424) = 0;
        *(int *)(c + 0x408) = 1;
    }
    *(short *)(c + 0x100) = 0xa;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        c + 0x30c, data_ov081_02128d98[1], 0, 0x1000, 0);
    return 1;
}
