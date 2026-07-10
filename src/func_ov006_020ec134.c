/* func_ov006_020ec134 at 0x020ec134 (ov006)
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 */
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

void func_ov006_020ec134(struct C* c)
{
    V2 d;
    V2 g;
    V2 e;
    V2 h;
    int i = 4;
    V2* p = &c->v18[i];
    int m = 0x8000;
    s16 zero = 0;
    int j;

    for (; i >= 1; i--) {
        j = i - 1;
        func_0203d6d0(&d, p, &c->v18[j]);
        if (func_0203d434(&d) != 0) {
            s16 a, prev, na, d2, diff, cur;
            { int w = c->w4c[i]; func_0203d630(&d, w + m); }
            a = _ZN4cstd5atan2E5Fix12IiES1_(d.x, d.y);
            prev = c->s7a[j];
            d2 = zero;
            na = (s16)(-a);
            diff = (s16)(prev - (s16)(-a));
            if (diff > 0x2000) {
                d2 = (s16)(diff - 0x2000);
                na = (s16)(prev - 0x2000);
            } else if (diff < -0x2000) {
                d2 = (s16)(diff + 0x2000);
                na = (s16)(prev + 0x2000);
            }
            cur = c->s7a[i];
            func_0203d388(&d, (s16)(d2 - (na - cur)));
        }
        func_0203d6d0(&e, p, &d);
        c->v18[j].x = e.x;
        c->v18[j].y = e.y;
        p--;
        m -= 0x2000;
    }
    g.x = 0;
    g.y = -0x10000;
    func_0203d388(&g, c->s7a[0]);
    func_0203d704(&h, (V2*)((long long)(int)c->v18 & 0xFFFFFFFFFFFFFFFFLL) + 1, &g);
    c->v18[0].x = h.x;
    c->v18[0].y = h.y;
}
