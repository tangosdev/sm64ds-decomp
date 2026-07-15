// NONMATCHING: incomplete / schedule (div high; size 0x384 vs target 0x3cc).
// ApproachLinear + vtable scan draft for refine. mwccarm 1.2/sp2p3.
typedef struct { int x, y, z; } Vec3;
typedef struct Obj {
    void **vtable;
} Obj;

extern int ApproachLinear_iii(int *var, int target, int step);
extern int ApproachLinear_sss(short *var, short target, short step);
extern int ApproachLinear2_sss(short *var, short target, short step);
extern void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
extern int NormalizeVec3IfNonZero(Vec3 *v);
extern int DotVec3(Vec3 *a, Vec3 *b);
extern void Vec3_MulScalar(Vec3 *out, Vec3 *v, int s);
extern void Vec3_Add(Vec3 *out, Vec3 *a, Vec3 *b);
extern int Vec3_Dist(Vec3 *a, Vec3 *b);
extern int LenVec3(Vec3 *v);
extern int RandomIntInternal(int *seed);
extern void func_ov006_020c49d8(void *c);
extern Obj *data_ov006_021403f4[];
extern int data_0209e650;

/* virtual: +0 GetPos, +4 GetVel?, +8 IsActive */
void func_ov006_020c4fa4(char *c)
{
    int r4, r0;
    int i;
    Obj *best;
    int best_dist;
    Vec3 tmp, vel, dir, scaled, sum, diff, clamp_pos, nvel;
    int sp0; /* best obj saved */
    Vec3 best_pos;
    int len;

    if (*(int *)(c + 0xe0) != 0) {
        ApproachLinear_iii((int *)(c + 0xb4), *(int *)(c + 0x9c), 0x1000);
        ApproachLinear_iii((int *)(c + 0xb8), *(int *)(c + 0xa0), 0x1000);
    }

    {
        int step = *(int *)(c + 0xa8);
        int tgt = *(int *)(c + 0xb4);
        if (step < 0)
            step = -step;
        r4 = ApproachLinear_iii((int *)(c + 0x9c), tgt, step);
    }
    {
        int step = *(int *)(c + 0xac);
        int tgt = *(int *)(c + 0xb8);
        if (step < 0)
            step = -step;
        r0 = ApproachLinear_iii((int *)(c + 0xa0), tgt, step);
    }
    if (r4 != 0)
        *(int *)(c + 0xa8) = 0;
    if (r0 != 0)
        *(int *)(c + 0xac) = 0;

    if (r4 == 0 || r0 == 0)
        goto fallback;
    if (ApproachLinear_sss((short *)(c + 0xe6), 0, 0x200) == 0)
        goto fallback;
    if (ApproachLinear2_sss((short *)(c + 0xec), 0, 1) == 0)
        goto fallback;

    best = 0;
    best_dist = 0x20000;
    for (i = 0; i < 3; i++) {
        Obj *o = data_ov006_021403f4[i];
        int (*is_active)(Obj *) = (int (*)(Obj *))o->vtable[2]; /* +8 */
        Vec3 *pos;
        Vec3 *v;
        int d;
        if (is_active(o) == 0)
            continue;
        {
            Vec3 *(*get_pos)(Obj *) = (Vec3 *(*)(Obj *))o->vtable[0];
            pos = get_pos(o);
            tmp = *pos;
        }
        Vec3_Sub(&dir, &tmp, (Vec3 *)(c + 0x9c));
        {
            Vec3 *(*get_vel)(Obj *) = (Vec3 *(*)(Obj *))o->vtable[1]; /* +4 */
            v = get_vel(o);
            vel = *v;
        }
        if (NormalizeVec3IfNonZero(&vel) == 0)
            continue;
        d = DotVec3(&vel, &dir);
        if (d <= 0)
            continue;
        Vec3_MulScalar(&scaled, &vel, d);
        Vec3_Add(&sum, &tmp, &scaled);
        d = Vec3_Dist((Vec3 *)(c + 0x9c), &sum);
        if (d >= best_dist)
            continue;
        best = o;
        best_dist = d;
        best_pos = sum;
    }

    if (best == 0)
        goto end;

    Vec3_Sub(&diff, &best_pos, (Vec3 *)(c + 0x9c));
    if (NormalizeVec3IfNonZero(&diff) == 0) {
        diff.x = 0x1000;
        diff.y = 0;
        diff.z = 0;
    }
    Vec3_MulScalar(&nvel, &diff, 0x40000);
    Vec3_Add(&clamp_pos, (Vec3 *)(c + 0x9c), &nvel);
    *(int *)(c + 0xb4) = clamp_pos.x;
    *(int *)(c + 0xb8) = clamp_pos.y;
    *(int *)(c + 0xbc) = clamp_pos.z;

    {
        int x = *(int *)(c + 0xb4);
        if (x < -0x68000)
            x = -0x68000;
        else if (x > 0x68000)
            x = 0x68000;
        *(int *)(c + 0xb4) = x;
    }
    {
        int y = *(int *)(c + 0xb8);
        if (y < -0xa8000)
            y = -0xa8000;
        else if (y > 0xa0000)
            y = 0xa0000;
        *(int *)(c + 0xb8) = y;
    }

    Vec3_Sub(&diff, (Vec3 *)(c + 0xb4), (Vec3 *)(c + 0x9c));
    len = LenVec3(&diff);
    if (NormalizeVec3IfNonZero(&diff) != 0) {
        Vec3_MulScalar(&nvel, &diff, len);
        *(int *)(c + 0xa8) = nvel.x;
        *(int *)(c + 0xac) = nvel.y;
        *(int *)(c + 0xb0) = nvel.z;
    } else {
        /* umull fix12: len * 0xb50 + 0x800 >> 12 */
        long long t = (long long)len * 0xb50 + 0x800;
        int v = (int)(t >> 12);
        *(int *)(c + 0xa8) = v;
        *(int *)(c + 0xac) = v;
    }
    {
        int r = RandomIntInternal(&data_0209e650) & 0x7fffffff;
        int v = (int)((unsigned)r >> 0x13);
        v = v * 0x2d;
        v = v >> 0xc;
        v = v + 0xf;
        *(short *)(c + 0xec) = (short)v;
    }
    goto end;

fallback:
    {
        int d = *(int *)(c + 0xb4) - *(int *)(c + 0x9c);
        if (d > 0)
            ApproachLinear_sss((short *)(c + 0xe6), 0x3000, 0x200);
        else if (d < 0)
            ApproachLinear_sss((short *)(c + 0xe6), -0x3000, 0x200);
    }
end:
    func_ov006_020c49d8(c);
}
