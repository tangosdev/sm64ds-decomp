// @symbol func_ov007_020c5fc8
/* recovered: shared common types */
#include "common.h"
typedef unsigned short u16;
typedef short s16;
typedef long long s64;


extern void SubVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern int LenVec3(struct Vector3 *v);

void func_ov007_020c5fc8(char *sb, int idx, int t)
{
    int i;
    int k;
    int inv;
    struct Vector3 cur;
    struct Vector3 prev;

    i = 0;
    if ((int)*(u16 *)((*(char ***)(sb + 0x38))[idx] + 8) <= 0)
        return;

    inv = 0x1000 - t;
    k = 0;
    do {
        int *a = (*(int ***)(sb + 0x28))[idx];
        int *b = (*(int ***)(sb + 0x2c))[idx];
        int va = *(int *)((char *)a + k);
        int vb = *(int *)((char *)b + k);
        int r = va;
        if (t < 0x1000) {
            if (t <= 0) r = vb;
            else r = (int)(((s64)inv * vb + (s64)t * va) >> 12);
        }
        cur.x = r;

        a = (*(int ***)(sb + 0x28))[idx];
        b = (*(int ***)(sb + 0x2c))[idx];
        va = *(int *)((char *)a + k + 4);
        vb = *(int *)((char *)b + k + 4);
        r = va;
        if (t < 0x1000) {
            if (t <= 0) r = vb;
            else r = (int)(((s64)inv * vb + (s64)t * va) >> 12);
        }
        cur.y = r;

        cur.z = *(int *)((char *)(*(int ***)(sb + 0x2c))[idx] + k + 8);

        if (i > 0) {
            char *p = (*(char ***)(sb + 0x3c))[idx];
            char *arr = *(char **)(p + 8);
            char *seg = ((char **)arr)[i - 1];
            SubVec3(&prev, &cur, (struct Vector3 *)(seg + 0xc));
            *(int *)(seg + 8) = LenVec3((struct Vector3 *)(seg + 0xc));
        }

        prev = cur;

        i++;
        k += 0xc;
    } while (i < (int)*(u16 *)((*(char ***)(sb + 0x38))[idx] + 8));
}
