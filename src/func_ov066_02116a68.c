struct dActor_c;
extern struct dActor_c *_ZN8dActor_c13ClosestPlayerEv(struct dActor_c *self);
typedef int Fix12i;

Fix12i func_ov066_02116a68(struct dActor_c *self) {
    volatile int dummy[3];
    (void)dummy;
    struct dActor_c *p = _ZN8dActor_c13ClosestPlayerEv(self);
    Fix12i dist = *(Fix12i*)((char*)p + 0x64);
    Fix12i lo = (Fix12i)0xff3ae000;
    if (dist < lo) return lo;
    Fix12i lo2 = -(Fix12i)0xb50000;
    if (dist < lo2) return lo2;
    Fix12i hi = (Fix12i)0xff598000;
    if (dist >= hi) return 0;
    return hi;
}
