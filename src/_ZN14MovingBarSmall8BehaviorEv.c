// NONMATCHING: case-1 DecIf body predicated (need bne+add r2 form); size 0x1c4 vs 0x1cc; flag path close.
extern int DecIfAbove0_Byte(unsigned char *p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
extern int Vec3_Dist(void *a, void *b);
extern unsigned _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned a, unsigned b, unsigned c, void *pos, unsigned e);
extern void func_020393a4(void *p, int v);
extern void func_02039394(void *p, int v);
extern void func_ov015_021123c8(void *self);

int _ZN14MovingBarSmall8BehaviorEv(char *self)
{
    int kind = *(int *)(self + 8) & 0xff;

    if (kind == 1) {
        if (DecIfAbove0_Byte((unsigned char *)(self + 0x390)) != 0)
            goto c1_after_dec;
        {
            short *p = (short *)(self + 0x94);
            short v = *p;
            unsigned char b = 0x87;
            v = (short)(v + 0x8000);
            *p = v;
            *(unsigned char *)(self + 0x390) = b;
        }
    c1_after_dec:
        _ZN5Actor9UpdatePosEP12CylinderClsn(self, 0);
        _ZN8Platform21UpdateModelPosAndRotYEv(self);
        if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(self, 0x150000, 0x1000) != 0)
            _ZN8Platform19UpdateClsnPosAndRotEv(self);
        goto done;
    }
    if (kind == 2) {
        if (DecIfAbove0_Byte((unsigned char *)(self + 0x390)) != 0)
            goto c2_after;
        if (*(unsigned char *)(self + 0x392) == 0)
            goto c2_after;
        {
            int y;
            int bound;
            int flag;

            _ZN5Actor9UpdatePosEP12CylinderClsn(self, 0);
            bound = *(int *)(self + 0x37c);
            y = *(int *)(self + 0x60);
            flag = 0;
            if (y >= bound)
                flag = 1;
            if (flag != 0)
                goto c2_hi;
            flag = 0;
            if (y >= bound)
                bound = y;
            *(int *)(self + 0x60) = bound;
            *(int *)(self + 0xa8) = 0xa000;
            if (*(unsigned char *)(self + 0x391) == 0) {
                flag = 0;
                *(unsigned char *)(self + 0x392) = (unsigned char)flag;
            }
            goto c2_after;
        c2_hi:
            bound = *(int *)(self + 0x380);
            flag = 0;
            if (y <= bound)
                flag = 1;
            if (flag != 0)
                goto c2_after;
            if (y <= bound)
                bound = y;
            *(int *)(self + 0x60) = bound;
            *(int *)(self + 0xa8) = -0xa000;
            *(unsigned char *)(self + 0x390) = 0x5a;
        }
    c2_after:
        _ZN8Platform21UpdateModelPosAndRotYEv(self);
        if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(self, 0x150000, 0x1000) != 0)
            _ZN8Platform19UpdateClsnPosAndRotEv(self);
        if (Vec3_Dist(self + 0x5c, self + 0x68) != 0) {
            unsigned z = 0;
            unsigned r = _ZN5Sound8PlayLongEjjjRK7Vector3j(
                *(unsigned *)(self + 0x38c), 3, 0x82, self + 0x74, z);
            *(unsigned *)(self + 0x38c) = r;
        }
        *(unsigned char *)(self + 0x391) = 0;
        goto done;
    }
    _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(self, 0x150000, 0x1000);
done:
    func_020393a4(self + 0x124, 0x150000);
    func_02039394(self + 0x124, 0x1000);
    func_ov015_021123c8(self);
    return 1;
}
