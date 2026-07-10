struct Vec3 { int x, y, z; };
struct PathPtr { int a, b; };

extern void _ZN7PathPtrC1Ev(struct PathPtr *self);
extern void _ZN7PathPtr6FromIDEj(struct PathPtr *self, unsigned int id);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(struct PathPtr *self, struct Vec3 *v, unsigned int i);
extern void Vec3_Sub(struct Vec3 *out, struct Vec3 *a, struct Vec3 *b);
extern int LenVec3(struct Vec3 *v);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a, int fix, unsigned int b);
extern void *data_ov081_02128d98[];

int func_ov081_0212538c(char *c)
{
    struct PathPtr pp;
    struct Vec3 node;
    struct Vec3 diff;
    int len;

    _ZN7PathPtrC1Ev(&pp);
    _ZN7PathPtr6FromIDEj(&pp, *(unsigned int *)(c + 0x418));
    _ZNK7PathPtr7GetNodeER7Vector3j(&pp, &node, *(unsigned int *)(c + 0x424));
    node.y = *(int *)(c + 0x60);
    Vec3_Sub(&diff, (struct Vec3 *)(c + 0x5c), &node);
    len = LenVec3(&diff);
    *(int *)(c + 0x458) = 0xa000;
    *(int *)(c + 0x408) = 0;
    if (len == 0 || len <= *(int *)(c + 0x458)) {
        *(int *)(c + 0x5c) = node.x;
        *(int *)(c + 0x60) = node.y;
        *(int *)(c + 0x64) = node.z;
        (*(int *)(((long long)(int)(c + 0x424)) & 0xFFFFFFFFFFFFFFFFLL))++;
        if (*(int *)(c + 0x424) >= *(int *)(c + 0x420))
            *(int *)(c + 0x424) = 0;
        *(int *)(c + 0x408) = 1;
    }
    *(short *)(c + 0x100) = 0xa;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        c + 0x30c, data_ov081_02128d98[1], 0, 0x1000, 0);
    return 1;
}
