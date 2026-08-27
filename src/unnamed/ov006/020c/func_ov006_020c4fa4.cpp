//cpp
typedef struct Vec3 { int x, y, z; } Vec3;

struct VB {
    virtual int *m0();
    virtual int *m1();
    virtual int m2();
};

extern "C" {
extern int _Z14ApproachLinearRiii(int *p, int t, int r);
extern int _Z14ApproachLinearRsss(short *p, short t, short r);
extern int _Z15ApproachLinear2Rsss(short *p, short t, short r);
extern int RandomIntInternal(int *seed);
extern int NormalizeVec3IfNonZero(Vec3 *v);
extern int Vec3_Dist(Vec3 *a, Vec3 *b);
extern void Vec3_MulScalar(Vec3 *d, Vec3 *s, int m);
extern void Vec3_Sub(Vec3 *d, Vec3 *a, Vec3 *b);
extern void Vec3_Add(Vec3 *d, Vec3 *a, Vec3 *b);
extern int LenVec3(Vec3 *v);
extern int DotVec3(Vec3 *a, Vec3 *b);
extern void func_ov006_020c49d8(char *c);
extern int data_0209e650;
extern VB *data_ov006_021403f4[];
}

extern "C" void func_ov006_020c4fa4(char *c)
{
    VB *best;
    Vec3 bp;
    Vec3 op;
    Vec3 delta;
    Vec3 nrm;
    Vec3 cand;
    Vec3 dir;
    Vec3 diff;
    Vec3 scaled;
    Vec3 proj;
    Vec3 sc2;
    Vec3 tgt;
    Vec3 vel;
    int bm, i, ra, rb;

    if (*(int *)(c + 0xe0) != 0) {
        _Z14ApproachLinearRiii((int *)(c + 0xb4), *(int *)(c + 0x9c), 0x1000);
        _Z14ApproachLinearRiii((int *)(c + 0xb8), *(int *)(c + 0xa0), 0x1000);
    }
    {
        int t = *(int *)(c + 0xa8);
        if (t < 0) t = -t;
        ra = _Z14ApproachLinearRiii((int *)(c + 0x9c), *(int *)(c + 0xb4), t);
    }
    {
        int t = *(int *)(c + 0xac);
        if (t < 0) t = -t;
        rb = _Z14ApproachLinearRiii((int *)(c + 0xa0), *(int *)(c + 0xb8), t);
    }
    if (ra != 0) *(int *)(c + 0xa8) = 0;
    if (rb != 0) *(int *)(c + 0xac) = 0;
    if (ra != 0 && rb != 0
        && _Z14ApproachLinearRsss((short *)(c + 0xe6), 0, 0x200) != 0
        && _Z15ApproachLinear2Rsss((short *)(c + 0xec), 0, 1) != 0) {
        best = 0;
        bm = 0x20000;
        for (i = 0; i < 3; i++) {
            if (data_ov006_021403f4[i]->m2() != 0) {
                int *v;
                int dot;
                v = data_ov006_021403f4[i]->m0();
                op.x = v[0]; op.y = v[1]; op.z = v[2];
                Vec3_Sub(&delta, (Vec3 *)(c + 0x9c), &op);
                v = data_ov006_021403f4[i]->m1();
                nrm.x = v[0]; nrm.y = v[1]; nrm.z = v[2];
                if (NormalizeVec3IfNonZero(&nrm) != 0) {
                    dot = DotVec3(&nrm, &delta);
                    if (dot > 0) {
                        int d;
                        Vec3_MulScalar(&scaled, &nrm, dot);
                        Vec3_Add(&proj, &op, &scaled);
                        cand.x = proj.x; cand.y = proj.y; cand.z = proj.z;
                        d = Vec3_Dist((Vec3 *)(c + 0x9c), &cand);
                        if (d < bm) {
                            best = data_ov006_021403f4[i];
                            bp = cand;
                            bm = d;
                        }
                    }
                }
            }
        }
        if (best != 0) {
            Vec3_Sub(&dir, (Vec3 *)(c + 0x9c), &bp);
            if (NormalizeVec3IfNonZero(&dir) == 0) {
                dir.x = 0x1000; dir.y = 0; dir.z = 0;
            }
            Vec3_MulScalar(&sc2, &dir, 0x40000);
            Vec3_Add(&tgt, (Vec3 *)(c + 0x9c), &sc2);
            *(int *)(c + 0xb4) = tgt.x;
            *(int *)(c + 0xb8) = tgt.y;
            *(int *)(c + 0xbc) = tgt.z;
            {
                int v2 = *(int *)(c + 0xb4);
                if (v2 < -0x68000) v2 = -0x68000;
                else if (v2 > 0x68000) v2 = 0x68000;
                *(int *)(c + 0xb4) = v2;
            }
            {
                int v3 = *(int *)(c + 0xb8);
                if (v3 < -0xa8000) v3 = -0xa8000;
                else if (v3 > 0xa0000) v3 = 0xa0000;
                *(int *)(c + 0xb8) = v3;
            }
            {
                int len;
                Vec3_Sub(&diff, (Vec3 *)(c + 0xb4), (Vec3 *)(c + 0x9c));
                len = LenVec3((Vec3 *)(c + 4));
                if (NormalizeVec3IfNonZero(&diff) != 0) {
                    Vec3_MulScalar(&vel, &diff, len);
                    *(int *)(c + 0xa8) = vel.x;
                    *(int *)(c + 0xac) = vel.y;
                    *(int *)(c + 0xb0) = vel.z;
                } else {
                    int w = (int)(((long long)len * 0xb50 + 0x800) >> 12);
                    *(int *)(c + 0xa8) = w;
                    *(int *)(c + 0xac) = w;
                }
            }
            {
                int rr = RandomIntInternal(&data_0209e650) & 0x7fffffff;
                *(short *)(c + 0xec) = (short)(((int)((unsigned int)rr >> 0x13) * 0x2d >> 12) + 0xf);
            }
        }
    } else {
        int df = *(int *)(c + 0xb4) - *(int *)(c + 0x9c);
        if (df > 0) _Z14ApproachLinearRsss((short *)(c + 0xe6), 0x3000, 0x200);
        else if (df < 0) _Z14ApproachLinearRsss((short *)(c + 0xe6), -0x3000, 0x200);
    }
    func_ov006_020c49d8(c);
}
