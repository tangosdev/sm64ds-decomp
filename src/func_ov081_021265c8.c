typedef int Fix12i;
extern Fix12i _ZN5Actor13DistToCPlayerEv(void *c);
extern Fix12i Vec3_Dist(const void *a, const void *b);
extern unsigned int RandomIntInternal(int *seed);
extern void func_ov081_021265c8(void *c);
extern void func_ov081_0212777c(void *c, int v);
extern int RNG_STATE;

void func_ov081_021265c8(void *c) {
    Fix12i distC;
    Fix12i dist;
    int v;

    distC = _ZN5Actor13DistToCPlayerEv(c);
    dist = Vec3_Dist((char *)c + 0x5c, (char *)c + 0x3d0);
    if (dist > 0x5dc000) {
        *(int *)((char *)c + 0x3e4) = 5;
    } else if (distC > 0x3e8000) {
        *(int *)((char *)c + 0x3e4) = 0;
    } else if (distC < 0x1f4000) {
        *(int *)((char *)c + 0x3e4) = 3;
    } else {
        if (RandomIntInternal(&RNG_STATE) % 10 < 7)
            *(int *)((char *)c + 0x3e4) = 3;
        else
            *(int *)((char *)c + 0x3e4) = 1;

        if (*(int *)((char *)c + 0x3e0) == 1 && *(int *)((char *)c + 0x3e4) == 1) {
            func_ov081_021265c8(c);
            return;
        }
    }

    v = *(int *)((char *)c + 0x3e0);
    if (v == 1 && *(int *)((char *)c + 0x3e4) != 1) {
        func_ov081_0212777c(c, 2);
        return;
    }
    if (v == 3 && *(int *)((char *)c + 0x3e4) != 3) {
        func_ov081_0212777c(c, 4);
        return;
    }
    func_ov081_0212777c(c, *(int *)((char *)c + 0x3e4));
}
