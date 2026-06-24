struct Vec3 { int x, y, z; };

struct E {
    struct Vec3 **p0;   // 0x0
    struct Vec3 **p1;   // 0x4
    int len;            // 0x8
    struct Vec3 v;      // 0xc
};

struct T {
    char pad4[4];
    int f4;             // 0x4
    struct E **arr;     // 0x8
    int fc;             // 0xc
    int f10;            // 0x10
};

extern void SubVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c);
extern int LenVec3(struct Vec3 *v);

void func_ov007_020c775c(struct T *t)
{
    int n;
    int cnt;
    int i;

    n = t->f4;
    if (n <= 1) {
        cnt = 0;
    } else {
        cnt = n - ((t->fc & 2) ? 0 : 1);
    }

    switch (t->f10) {
    case 0:
    case 1:
    case 2:
    case 3:
        for (i = 0; i < cnt; i++) {
            struct E *e = t->arr[i];
            SubVec3(*e->p0, *e->p1, &e->v);
            e->len = LenVec3(&e->v);
        }
        break;
    }
}
