typedef short s16;

typedef struct { int x, y; } V2;

struct C {
    char pad0[0x18];
    V2 v18[5];          /* 0x18 */
    char pad40[0xc];    /* 0x40 */
    int w4c[5];         /* 0x4c */
    char pad60[0x16];   /* 0x60 */
    s16 s76;            /* 0x76 */
    char pad78[2];      /* 0x78 */
    s16 s7a[5];         /* 0x7a */
    char pad84[0x10];   /* 0x84 */
    unsigned char b94;  /* 0x94 */
};

extern void _Z11UpdateAngleRssis(s16* p, s16 tgt, int step, s16 amt);
extern void func_0203d6d0(V2* o, V2* a, V2* b);
extern int func_0203d434(V2* in);
extern void func_0203d630(V2* p, int m);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void func_0203d388(V2* p, int angle);
extern void func_0203d704(V2* o, V2* a, V2* b);

void func_ov006_020ebf20(struct C* c)
{
    V2 d;
    V2 g;
    V2 e;
    V2 h;
    int i;
    s16 na;
    int j;
    s16 d2;
    int cur;
    V2* pj;
    int zero;
    int step;
    int amt;
    V2* vb2;

    if (c->b94 != 0)
        return;

    _Z11UpdateAngleRssis(&c->s7a[1], c->s76, 8, 0x100);

    i = 2;
    step = 2;
    zero = 0;
    amt = 0x100;

    for (; i < 5; i++) {
        j = i - 1;
        pj = &c->v18[j];
        func_0203d6d0(&d, &c->v18[i], pj);
        if (func_0203d434(&d) != 0) {
            s16 a, prev, diff;
            func_0203d630(&d, c->w4c[i] + i * 0x2000);
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
            _Z11UpdateAngleRssis(&c->s7a[i], na, step, amt);
            cur = c->s7a[i];
            cur = na - cur;
            func_0203d388(&d, (s16)(d2 - cur));
        }
        func_0203d704(&e, pj, &d);
        c->v18[i].x = e.x;
        c->v18[i].y = e.y;
    }

    {
        s16 dd = (s16)(c->s76 - c->s7a[1]);
        g.x = 0;
        g.y = -0x10000;
        _Z11UpdateAngleRssis(&c->s7a[0], (s16)(c->s7a[1] + dd), 8, 0x100);
    }
    func_0203d388(&g, c->s7a[0]);
    vb2 = c->v18;
    func_0203d704(&h, vb2 + 1, &g);
    c->v18[0].x = h.x;
    c->v18[0].y = h.y;
}
