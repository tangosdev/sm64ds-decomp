struct Vec3 { int x, y, z; };
struct PathPtr { int a, b; };

extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
extern void func_02012694(int a, void *b);
extern int func_ov081_02125488(void *c, void *p);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a, int fix, unsigned int b);
extern void _ZN7PathPtrC1Ev(struct PathPtr *self);
extern void _ZN7PathPtr6FromIDEj(struct PathPtr *self, unsigned int id);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(struct PathPtr *self, struct Vec3 *v, unsigned int i);
extern short Vec3_HorzAngle(struct Vec3 *a, struct Vec3 *b);
extern void ApproachAngle(void *p, int a, int b, int c, int d);
extern int AngleDiff(int a, int b);
extern char data_ov081_02128d88[];
extern char data_ov081_02128e74[];

int func_ov081_02125208(char *c) {
    struct PathPtr p;
    struct Vec3 node;
    int st;
    int z;
    int n;

    if (*(unsigned short*)(c + 0x100) != 0) {
        goto exit;
    }

    if (*(int*)(c + 0x408) == 1) {
        *(int*)(c + 0xa8) = 0x14000;
        n = 0x4000;
        *(int*)(c + 0x9c) = -n;
        *(int*)(c + 0x408) = 2;
        func_02012694(0xdd, c + 0x74);
    }

    _ZN7PathPtrC1Ev(&p);
    _ZN7PathPtr6FromIDEj(&p, *(unsigned int*)(c + 0x418));
    _ZNK7PathPtr7GetNodeER7Vector3j(&p, &node, *(unsigned int*)(c + 0x424));
    ApproachAngle(c + 0x94, Vec3_HorzAngle((struct Vec3*)(c + 0x5c), &node), 1, 0x1000, 0x500);

    if (AngleDiff(*(short*)(c + 0x94), Vec3_HorzAngle((struct Vec3*)(c + 0x5c), &node)) >= 0x100) {
        goto exit;
    }

    if (!_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x150)) {
        goto exit;
    }

    st = *(int*)(c + 0x408);
    if (st == 0) {
        goto label_118;
    }
    if (st != 1) {
        goto label_ec;
    }

    func_02012694(0xe5, c + 0x74);

label_ec:
    {
        int *pState = (int*)(((int)c + 0x408) & 0xFFFFFFFFFFFFFFFF);
        *pState = *pState + 1;
        if (*(int*)(c + 0x408) < 0xb) {
            return 1;
        }
    }

label_118:
    z = 0;
    n = 0x4000;
    *(int*)(c + 0x408) = z;
    *(int*)(c + 0xa8) = 0x3a000;
    *(int*)(c + 0x9c) = -n;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x30c, *(void**)(data_ov081_02128d88 + 4), 0x40000000, 0x1000, z);
    func_02012694(0xdd, c + 0x74);
    func_ov081_02125488(c, data_ov081_02128e74);

exit:
    return 1;
}
