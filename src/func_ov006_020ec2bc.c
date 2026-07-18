typedef short s16;

typedef struct { int x, y; } V2;

struct C {
    char pad0[0x18];
    V2 v18[5];          /* 0x18 */
    char pad40[0xc];    /* 0x40 */
    int w4c[5];         /* 0x4c */
    char pad60[0x1a];   /* 0x60 */
    s16 s7a[5];         /* 0x7a */
};

extern void func_0203d6d0(V2* o, V2* a, V2* b);
extern int func_0203d434(V2* in);
extern void func_0203d630(V2* p, int m);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void func_0203d388(V2* p, int angle);
extern void func_0203d704(V2* o, V2* a, V2* b);

void func_ov006_020ec2bc(struct C* c)
{
    V2 d;
    V2 g;
    V2 e;
    V2 h;
    int i = 2;
    V2* p = &c->v18[i];
    int j;

    for (; i < 5; i++) {
        j = i - 1;
        func_0203d6d0(&d, p, &c->v18[j]);
        if (func_0203d434(&d) != 0) {
            s16 a, prev, na, d2, diff, cur;
            func_0203d630(&d, (i * 2 + 0x10) << 12);
            a = _ZN4cstd5atan2E5Fix12IiES1_(d.x, d.y);
            prev = c->s7a[j];
            d2 = 0;
            na = (s16)(-a);
            diff = (s16)(prev - (s16)(-a));
            if (diff > 0x2000) {
                d2 = (s16)(diff - 0x2000);
                na = (s16)(prev - 0x2000);
            } else if (diff < -0x2000) {
                d2 = (s16)(diff + 0x2000);
                na = (s16)(prev + 0x2000);
            }
            c->s7a[i] = na;
            cur = c->s7a[i];
            func_0203d388(&d, (s16)(d2 - (na - cur)));
        }
        func_0203d704(&e, &c->v18[j], &d);
        c->v18[i].x = e.x;
        c->v18[i].y = e.y;
        p++;
    }
    g.x = 0;
    g.y = -0x10000;
    c->s7a[0] = (s16)(c->s7a[1] + (((c->s7a[1] - c->s7a[2]) << 14) >> 16));
    func_0203d388(&g, c->s7a[0]);
    func_0203d704(&h, (V2*)((long long)(int)c->v18 & 0xFFFFFFFFFFFFFFFFLL) + 1, &g);
    c->v18[0].x = h.x;
    c->v18[0].y = h.y;
}
