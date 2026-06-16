struct Actor;
extern struct Actor *_ZN5Actor13ClosestPlayerEv(struct Actor *self);
typedef int Fix12i;

Fix12i func_ov066_02116a68(struct Actor *self) {
    volatile int dummy[3];
    (void)dummy;
    struct Actor *p = _ZN5Actor13ClosestPlayerEv(self);
    Fix12i dist = *(Fix12i*)((char*)p + 0x64);
    Fix12i lo = (Fix12i)0xff3ae000;
    if (dist < lo) return lo;
    Fix12i lo2 = -(Fix12i)0xb50000;
    if (dist < lo2) return lo2;
    Fix12i hi = (Fix12i)0xff598000;
    if (dist >= hi) return 0;
    return hi;
}
