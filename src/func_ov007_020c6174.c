struct Vec3 { int x, y, z; };
struct Elem {
    struct Vec3 **a;   /* 0x0 */
    struct Vec3 **b;   /* 0x4 */
    int len;           /* 0x8 */
    struct Vec3 v;     /* 0xc */
};
struct Node {
    char pad[8];
    unsigned short count;  /* 0x8 */
};

struct T {
    int p0, p4, p8;
    char pad[0x2c - 0xc];
    struct Vec3 **p2c;   /* 0x2c */
    char pad2[0x38 - 0x30];
    struct Node **p38;   /* 0x38 */
    struct Vec3 **p3c;   /* 0x3c */
    struct Elem **p40;   /* 0x40 */
    struct Elem **p44;   /* 0x44 */
    unsigned short *p48; /* 0x48 */
    unsigned short *p4c; /* 0x4c */
    unsigned short *p50; /* 0x50 */
    unsigned short *p54; /* 0x54 */
};

extern void func_ov007_020c7694(struct Vec3 *a, struct Vec3 *b);
extern void SubVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c);
extern int LenVec3(struct Vec3 *v);
extern void func_ov007_020c775c(struct Vec3 *t);

void func_ov007_020c6174(struct T *t, int idx, int flag)
{
    struct Elem *e;
    if (flag) {
        func_ov007_020c7694(t->p3c[idx], t->p2c[idx]);
        if (t->p8 <= 1) return;
        e = t->p40[idx];
        SubVec3(t->p2c[idx], &t->p2c[t->p48[idx]][t->p50[idx]], &e->v);
        e->len = LenVec3(&e->v);
        e = t->p44[idx];
        SubVec3(&t->p2c[idx][t->p38[idx]->count - 1],
                &t->p2c[t->p4c[idx]][t->p54[idx]], &e->v);
        e->len = LenVec3(&e->v);
    } else {
        func_ov007_020c775c(t->p3c[idx]);
        if (t->p8 <= 1) return;
        e = t->p40[idx];
        SubVec3(*e->a, *e->b, &e->v);
        e->len = LenVec3(&e->v);
        e = t->p44[idx];
        SubVec3(*e->a, *e->b, &e->v);
        e->len = LenVec3(&e->v);
    }
}
