/* func_ov081_021250c8 at 0x021250c8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov081).
 */
struct Vec3 { int x, y, z; };
struct PathPtr { int a, b; };

extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
extern void func_02012694(int a, void *b);
extern int func_ov081_02125488(void *c, void *p);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, int a, int fix, unsigned int b);
extern void _ZN7PathPtrC1Ev(struct PathPtr *self);
extern void _ZN7PathPtr6FromIDEj(struct PathPtr *self, unsigned int id);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(struct PathPtr *self, struct Vec3 *v, unsigned int i);
extern void Vec3_Sub(struct Vec3 *out, struct Vec3 *a, struct Vec3 *b);
extern int LenVec3(struct Vec3 *v);
extern short Vec3_HorzAngle(struct Vec3 *a, struct Vec3 *b);
extern void ApproachAngle(void *p, int a, int b, int c, int d);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Vec3_MulScalar(struct Vec3 *out, struct Vec3 *in, int s);
extern void SubVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c);
extern char data_ov081_02128e54[];
extern char data_ov081_02128da0[];

int func_ov081_021250c8(char *c) {
    struct PathPtr p;
    struct Vec3 node;
    struct Vec3 diff;
    struct Vec3 scaled;
    int len;

    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x150)) {
        func_02012694(0xe5, c + 0x74);
        func_ov081_02125488(c, data_ov081_02128e54);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x30c, *(void**)(data_ov081_02128da0 + 4), 0x40000000, 0x1000, 0);
        return 1;
    }

    _ZN7PathPtrC1Ev(&p);
    _ZN7PathPtr6FromIDEj(&p, *(unsigned int*)(c + 0x418));
    _ZNK7PathPtr7GetNodeER7Vector3j(&p, &node, *(unsigned int*)(c + 0x424));
    node.y = *(int*)(c + 0x60);
    Vec3_Sub(&diff, (struct Vec3*)(c + 0x5c), &node);
    len = LenVec3(&diff);
    ApproachAngle(c + 0x94, Vec3_HorzAngle((struct Vec3*)(c + 0x5c), &node), 1, 0x1000, 0x500);
    if (len == 0) goto epi;
    if (len > *(int*)(c + 0x458)) goto work;
epi:
    return 1;
work:
    {
        int q = _ZN4cstd4fdivEii(*(int*)(c + 0x458), len);
        Vec3_MulScalar(&scaled, &diff, q);
        SubVec3((struct Vec3*)(c + 0x5c), &scaled, (struct Vec3*)(c + 0x5c));
    }
    return 1;
}