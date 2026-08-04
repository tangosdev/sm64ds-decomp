typedef short s16;
typedef long long s64;

typedef struct { int x, y; } V2;

struct C {
    V2 v0;              /* 0x00 */
    V2 v8;              /* 0x08 */
    char pad10[8];      /* 0x10 */
    V2 v18[5];          /* 0x18 */
    int w40;            /* 0x40 */
    int w44;            /* 0x44 */
    int w48;            /* 0x48 */
    int w4c[5];         /* 0x4c */
    char pad60[0x16];   /* 0x60 */
    s16 s76;            /* 0x76 */
    char pad78[2];      /* 0x78 */
    s16 s7a[5];         /* 0x7a */
    char pad84[2];      /* 0x84 */
    s16 s86;            /* 0x86 */
    char pad88[0xc];    /* 0x88 */
    unsigned char b94;  /* 0x94 */
    unsigned char b95;  /* 0x95 */
};

extern int _ZN4cstd4fdivEii(int a, int b);
extern int data_ov006_0213c958;
extern V2 data_ov006_0213c9dc;
extern void func_0203d388(V2* p, int angle);
extern void func_0203d704(V2* o, V2* a, V2* b);
extern void func_ov006_020ec2bc(struct C* c);

void func_ov006_020ecba4(struct C* c, int param)
{
    V2 vec0;
    V2 vec1;
    V2 vec3;
    V2 vec2;
    V2 vec4;
    int m, r1, r2, k, i, j;

    c->b94 = 1;
    c->w48 = 0x3000;
    c->b95 = 0;
    c->s86 = 0x200;
    c->w40 = 0;
    c->w44 = 0;

    m = data_ov006_0213c958 - 1;
    r1 = _ZN4cstd4fdivEii(param << 12, m << 12);
    r2 = _ZN4cstd4fdivEii((m - param) << 12, m << 12);
    k = (m - 2) * 8;

    vec0.y = 0;
    vec0.y = (int)(((s64)((0x40 - k) << 12) * r1 + 0x800) >> 12)
           + (int)(((s64)((k + 0x98) << 12) * r2 + 0x800) >> 12);
    vec0.x = 0;
    if (param & 1)
        vec0.x = 0xa8000;
    else
        vec0.x = 0x38000;

    c->s76 = -0x8000;
    c->v18[0].x = *(int*)&vec0;
    c->v18[0].y = *((int*)&vec0 + 1);
    c->s7a[0] = c->s76 - 0x2000;

    for (i = 1; i < 5; i++) {
        j = i - 1;
        vec1.x = 0;
        vec1.y = (i * 2 + 0x10) << 12;
        func_0203d388(&vec1, (s16)(c->s7a[j] + 0x2000));
        c->s7a[i] = c->s7a[j] + 0x2000;
        func_0203d704(&vec2, &c->v18[j], &vec1);
        c->v18[i].x = vec2.x;
        c->v18[i].y = vec2.y;
    }

    func_ov006_020ec2bc(c);

    vec3.x = 0;
    vec3.y = -0x10000;
    c->s7a[0] = c->s76 - 0x2000;
    func_0203d388(&vec3, c->s7a[0]);
    func_0203d704(&vec4, &c->v18[1], &vec3);
    c->v18[0].x = vec4.x;
    c->v18[0].y = vec4.y;

    c->v0 = data_ov006_0213c9dc;
    c->v8 = c->v0;
}
