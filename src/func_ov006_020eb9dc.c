extern int RandomIntInternal(int* seed);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern int data_0209e650;

struct P2 { int a; int b; };

extern struct P2 data_ov006_0213c9f4;
extern struct P2 data_ov006_0213c9ec;
extern struct P2 data_ov006_0213c9e4;
extern struct P2 data_ov006_0213c974;
extern struct P2 data_ov006_0213c95c;

struct Obj {
    struct P2 m0;                   /* 0x00 */
    struct P2 m8;                   /* 0x08 */
    struct P2 m10;                  /* 0x10 */
    char pad18[0x40 - 0x18];
    int w40;                        /* 0x40 */
    char pad44[0x70 - 0x44];
    int w70;                        /* 0x70 */
    char pad74[0x78 - 0x74];
    short h78;                      /* 0x78 */
    char pad7a[0x8c - 0x7a];
    short h8c;                      /* 0x8c */
    short pad8e;
    short h90;                      /* 0x90 */
    unsigned char pad92;
    unsigned char b93;              /* 0x93 */
};

void func_ov006_020eb9dc(struct Obj* c, int v)
{
    short* h = (short*)(int)(((long long)(int)&c->h8c) & 0xFFFFFFFFFFFFFFFFLL);
    struct P2* p = (struct P2*)(int)(((long long)(int)&c->m8) & 0xFFFFFFFFFFFFFFFFLL);
    struct P2* g = (struct P2*)(int)(((long long)(int)&data_ov006_0213c9f4) & 0xFFFFFFFFFFFFFFFFLL);

    (*h)++;
    if (p->a == g->a &&
        (p->b == g->b || c->m8.a == 0) &&
        c->h8c < 3) {
        c->b93 = 0;
        c->h78 = (RandomIntInternal(&data_0209e650) & 0x20) + 0x60;
        if (c->h8c == 1) {
            c->m10 = data_ov006_0213c9ec;
            c->m8 = c->m10;
        } else {
            c->m10 = data_ov006_0213c9e4;
            c->m8 = c->m10;
        }
        if (c->w70 == 0) {
            _ZN5Sound12PlayBank2_2DEj(0x1e9);
        }
        c->m0 = data_ov006_0213c974;
    } else {
        c->w40 = 0;
        c->h90 = (v >> 12) / 4 + 0x31;
        c->m10 = c->m8;
        c->m0 = data_ov006_0213c95c;
    }
}
