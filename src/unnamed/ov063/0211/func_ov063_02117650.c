// @symbol func_ov063_02117650
/* recovered: shared common types */
#include "common.h"
extern void *_ZN8dActor_c13ClosestPlayerEv(void);
extern int RandomIntInternal(int *seed);
extern int Vec3_HorzDist(struct Vector3 *a, struct Vector3 *b);
extern short Vec3_HorzAngle(struct Vector3 *a, struct Vector3 *b);
extern int data_0209e650;

void func_ov063_02117650(char *self)
{
    struct Vector3 ppos;
    struct Vector3 npos;
    char *p;
    int neg1 = (int)(-1LL);

    p = (char *)_ZN8dActor_c13ClosestPlayerEv();
    if (p == 0) {
        return;
    }

    {
        int *pp = (int *)(p + 0x5c);
        ppos.x = pp[0];
        ppos.y = pp[1];
        ppos.z = pp[2];
    }
    npos.y = *(int *)(self + 0x60);

    do {
        npos.x = ((int)(((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 0x578) * neg1 - 0x190) << 0xc;
        npos.z = ((int)(((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 0xa28) - 0x514) << 0xc;
    } while (Vec3_HorzDist(&ppos, &npos) < 0x320000);

    *(int *)(self + 0x5c) = npos.x;
    *(int *)(self + 0x60) = npos.y;
    *(int *)(self + 0x64) = npos.z;
    *(short *)(self + 0x8e) = Vec3_HorzAngle((struct Vector3 *)(self + 0x5c), &ppos);
    *(short *)(self + 0x94) = *(short *)(self + 0x8e);
}
