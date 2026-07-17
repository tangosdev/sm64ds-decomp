typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern u16 DecIfAbove0_Short(u16 *p);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void *v);
extern int _Z14ApproachLinearRsss(s16 *val, int target, int step);
extern int RandomIntInternal(int *seed);
extern void func_ov065_0211990c(char *c);
extern void func_ov065_02119794(char *c);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern void func_ov065_021198a0(char *self);

extern u8 data_0209f2c0;
extern u8 data_ov065_0211cfa4[];
extern int data_0209e650;
extern s16 data_ov065_0211cfa8[];

int _ZN15TtcRotatingCube8BehaviorEv(char *self) {
    if (data_0209f2c0 != 3) {
        switch (*(u8 *)(self + 0x376)) {
        case 0:
            if (DecIfAbove0_Short((u16 *)(self + 0x374)) != 0)
                break;
            _ZN5Sound9PlayBank3EjRK7Vector3(0x5b, self + 0x74);
            (*(u8 *)(((int)self + 0x376) & 0xFFFFFFFFFFFFFFFF))++;
            *(int *)(self + 0xa8) = -0x5000;
            break;
        case 1:
            *(int *)(((int)self + 0xa8) & 0xFFFFFFFFFFFFFFFF) += 0x800;
            *(int *)(((int)self + 0x370) & 0xFFFFFFFFFFFFFFFF) += *(int *)(self + 0xa8);
            if (*(int *)(self + 0x370) < 0)
                break;
            *(int *)(self + 0x370) = 0;
            *(s16 *)(self + 0x374) = 6;
            (*(u8 *)(((int)self + 0x376) & 0xFFFFFFFFFFFFFFFF))++;
            break;
        case 2:
            if (DecIfAbove0_Short((u16 *)(self + 0x374)) != 0)
                break;
            if (_Z14ApproachLinearRsss((s16 *)(self + 0x90), *(s16 *)(self + 0x378), 0x4b0) == 0)
                break;
            _ZN5Sound9PlayBank3EjRK7Vector3(0x40, self + 0x74);
            *(u8 *)(self + 0x376) = 0;
            *(s16 *)(self + 0x374) = data_ov065_0211cfa4[data_0209f2c0];
            if (data_0209f2c0 == 2)
                *(s16 *)(self + 0x374) = (unsigned int)RandomIntInternal(&data_0209e650) % 7 * 0x14 + 5;
            *(s16 *)(((int)self + 0x378) & 0xFFFFFFFFFFFFFFFF) += data_ov065_0211cfa8[*(u8 *)(self + 0x377)];
            break;
        }
    }
    func_ov065_0211990c(self);
    func_ov065_02119794(self);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(self, 0, 0))
        func_ov065_021198a0(self);
    return 1;
}
