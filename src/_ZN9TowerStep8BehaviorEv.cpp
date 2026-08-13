//cpp
#include "types.h"

extern "C" {
int DecIfAbove0_Byte(u8 *p);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
int Vec3_Dist(void *a, void *b);
unsigned _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned a, unsigned b, unsigned c, void *pos, unsigned e);
void func_020393a4(void *p, int v);
void func_02039394(void *p, int v);
void func_ov015_021123c8(void *self);
}

extern "C" int _ZN9TowerStep8BehaviorEv(char *self)
{
    int kind = *(int *)(self + 8) & 0xff;

    if (kind == 1) {
        if (DecIfAbove0_Byte((u8 *)(self + 0x390)) == 0) {
            s16 *p = (s16 *)((int)self + 0x94);
            s16 v = *p;
            u8 b = 0x87;
            v = (s16)(v + 0x8000);
            *p = v;
            *(u8 *)(self + 0x390) = b;
        }
        _ZN5Actor9UpdatePosEP12CylinderClsn(self, 0);
        _ZN8Platform21UpdateModelPosAndRotYEv(self);
        if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(self, 0x150000, 0x1000) != 0)
            _ZN8Platform19UpdateClsnPosAndRotEv(self);
    } else if (kind == 2) {
        if (DecIfAbove0_Byte((u8 *)(self + 0x390)) == 0) {
            if (*(u8 *)(self + 0x392) != 0) {
                int flag;
                int bound;
                int y;

                _ZN5Actor9UpdatePosEP12CylinderClsn(self, 0);
                bound = *(int *)(self + 0x37c);
                y = *(int *)(self + 0x60);
                flag = (y >= bound);
                if (flag != 0)
                    goto c2_hi;
                *(int *)(self + 0x60) = (y < bound) ? bound : y;
                *(int *)(self + 0xa8) = 0xa000;
                if (*(u8 *)(self + 0x391) == 0) {
                    flag = 0;
                    *(u8 *)(self + 0x392) = (u8)flag;
                }
                goto c2_after;
            c2_hi:
                bound = *(int *)(self + 0x380);
                flag = (y <= bound);
                if (flag != 0)
                    goto c2_after;
                *(int *)(self + 0x60) = (y > bound) ? bound : y;
                *(int *)(self + 0xa8) = -0xa000;
                *(u8 *)(self + 0x390) = 0x5a;
            }
        }
    c2_after:
        _ZN8Platform21UpdateModelPosAndRotYEv(self);
        if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(self, 0x150000, 0x1000) != 0)
            _ZN8Platform19UpdateClsnPosAndRotEv(self);
        if (Vec3_Dist(self + 0x5c, self + 0x68) != 0) {
            unsigned z = 0;
            unsigned r = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                *(unsigned *)(self + 0x38c), 3, 0x82, self + 0x74, z);
            *(unsigned *)(self + 0x38c) = r;
        }
        *(u8 *)(self + 0x391) = 0;
    } else {
        _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(self, 0x150000, 0x1000);
    }

    func_020393a4(self + 0x124, 0x150000);
    func_02039394(self + 0x124, 0x1000);
    func_ov015_021123c8(self);
    return 1;
}
