/* func_ov006_020c42bc at 0x020c42bc (ov006)
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 */
typedef struct { int x, y, z; } Vec3;

struct Obj {
    char pad0[0x9c];
    Vec3 pos;             /* 0x9c */
    Vec3 vel;             /* 0xa8 */
    char padb4[0xe0 - 0xb4];
    int e0;               /* 0xe0 */
    char pade4[0xf0 - 0xe4];
};

extern struct Obj* data_ov006_02140324;
extern int data_ov006_02140328;

extern void func_ov006_020c4cd8(struct Obj* o);
extern int func_ov006_020c4710(struct Obj* o);
extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern int LenVec3(Vec3* v);
extern int NormalizeVec3IfNonZero(Vec3* v);
extern void AddVec3(Vec3* a, Vec3* b, Vec3* c);
extern void SubVec3(Vec3* a, Vec3* b, Vec3* c);

void func_ov006_020c42bc(void)
{
    Vec3 v1;
    Vec3 v2;
    struct Obj* oi;
    struct Obj* oj;
    Vec3* pi;
    Vec3* pj;
    int i, j, k, t;
    Vec3* ai;

    for (i = 0; i < data_ov006_02140328; i++) {
        func_ov006_020c4cd8(&data_ov006_02140324[i]);
    }
    for (k = 0; k < data_ov006_02140328; k++) {
        data_ov006_02140324[k].e0 = 0;
    }
    for (t = 0; t < data_ov006_02140328; t++) {
        if (func_ov006_020c4710(&data_ov006_02140324[t]) != 0) {
            oi = &data_ov006_02140324[t];
            pi = &oi->pos;
            ai = &oi->vel;
            for (j = t + 1; j < data_ov006_02140328; j++) {
                if (func_ov006_020c4710(&data_ov006_02140324[j]) != 0) {
                    oj = (struct Obj*)((long long)(int)&data_ov006_02140324[j] & 0xFFFFFFFFFFFFFFFFLL);
                    pj = &oj->pos;
                    Vec3_Sub(&v1, pi, pj);
                    Vec3_Sub(&v2, ai, &oj->vel);
                    if (LenVec3(&v1) >= 0x12000) continue;
                    if (LenVec3(&v2) >= 0x100) continue;
                    if (NormalizeVec3IfNonZero(&v1) == 0) continue;
                    AddVec3(pi, &v1, pi);
                    SubVec3(pj, &v1, pj);
                    data_ov006_02140324[t].e0 = 1;
                    data_ov006_02140324[j].e0 = 1;
                }
            }
        }
    }
}
