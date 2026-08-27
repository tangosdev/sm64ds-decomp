struct V2 { int x, y; };

#define AT(p, off) ((void*)(int)((char*)(p) + (off)))

extern void func_ov006_020ce8a0(char* self, void* other, struct V2* a, struct V2* b);
extern void Vec2_Sub(struct V2* o, struct V2* a, struct V2* b);
extern int func_0203d524(struct V2* a, struct V2* b);
extern int data_ov006_0212e070[];
extern int data_ov006_0213b324[2];
extern int data_ov006_0213b334[2];

int func_ov006_020ce674(char* self, void* other) {
    struct V2 a, b, n, t0, d, e, f, g, h, i;
    int f0 = *(int*)self;
    int s0, s1, s2, s3;
    int result;

    {
        int *p324 = (int *)AT(data_ov006_0213b324, 0);
        int *p334;
        if ((f0 == p324[0] && (*(int*)(self + 4) == p324[1] || f0 == 0)) ||
            (p334 = (int *)AT(data_ov006_0213b334, 0),
             f0 == p334[0] && (*(int*)(self + 4) == p334[1] || f0 == 0)))
            return 0;
    }

    result = 0;
    func_ov006_020ce8a0(self, other, &a, &b);

    {
        int *dp = (int *)AT(data_ov006_0212e070, 0);
        n.x = -dp[*(short*)(self + 0x96)];
        n.y = 0;
        t0.x = dp[*(short*)(self + 0x96)];
        t0.y = 0;
    }

    Vec2_Sub(&d, &t0, &n);
    Vec2_Sub(&e, &a, &n);
    Vec2_Sub(&f, &b, &n);
    s0 = func_0203d524(&d, &e);
    s1 = func_0203d524(&d, &f);
    if (s0 < -1) s0 = -1; else if (s0 > 1) s0 = 1;
    if (s1 < -1) s1 = -1; else if (s1 > 1) s1 = 1;
    if (s0 * s1 > 0) goto done;
    if (s0 == 0 && s1 == 0) goto done;

    Vec2_Sub(&g, &a, &b);
    d = g;
    Vec2_Sub(&h, &t0, &a);
    e = h;
    Vec2_Sub(&i, &n, &a);
    f = i;
    s2 = func_0203d524(&d, &e);
    s3 = func_0203d524(&d, &f);
    if (s2 < -1) s2 = -1; else if (s2 > 1) s2 = 1;
    if (s3 < -1) s3 = -1; else if (s3 > 1) s3 = 1;
    if (s2 * s3 < 0 && (s2 != 0 || s3 != 0)) result = 1;
done:
    return result;
}
