// @symbol func_ov007_020c7694
/* recovered: shared common types */
#include "common.h"
struct E {
    int f0;
    int f4;
    int len;            // 0x8
    struct Vector3 v;      // 0xc
};

struct T {
    char pad4[4];
    int f4;             // 0x4
    struct E **arr;     // 0x8
    int fc;             // 0xc
    int f10;            // 0x10
};

extern void SubVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern int LenVec3(struct Vector3 *v);

void func_ov007_020c7694(struct T *t, struct Vector3 *base)
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
        i = 0;
        if (cnt <= 0)
            return;
        do {
            int j = (i < t->f4 - 1) ? i + 1 : 0;
            struct E *e = t->arr[i];
            SubVec3(&base[i], &base[j], &e->v);
            e->len = LenVec3(&e->v);
            i++;
        } while (i < cnt);
        break;
    }
}
