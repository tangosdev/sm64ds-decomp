#include "types.h"
extern signed char data_0209f2f8;
extern u8 data_0209211c;
extern u8 data_0209f200;
extern void ExitLevel(void);
extern int _ZNK6Player14GetBodyModelIDEjb(void *thiz, unsigned int a, int b);
extern int _ZNK9Animation12WillHitFrameEi(void *thiz, int f);

int func_ov002_020c94a4(char *self)
{
    unsigned short st;

    *(int *)(self + 0xa8) = 0;

    st = *(unsigned short *)(self + 0x6a4);
    if (st != 0) {
        unsigned char sub;
        if (st != 1)
            goto ret1;

        sub = *(u8 *)(self + 0x70a);
        if (sub == 0) {
            if (data_0209f2f8 == 0x21) {
                data_0209211c = 4;
                data_0209f200 = 7;
            }
            ExitLevel();
            *(u8 *)(self + 0x6e3) = 6;
            goto ret1;
        }

        if (sub == 3) {
            switch (data_0209f2f8) {
            case 0x24:
                data_0209211c = 2;
                data_0209f200 = 0xa;
                break;
            case 0x26:
                data_0209211c = 4;
                data_0209f200 = 9;
                break;
            case 0x2d:
                data_0209211c = 0x32;
                data_0209f200 = 3;
                break;
            case 0x2f:
                data_0209211c = 3;
                data_0209f200 = 0;
                break;
            case 0x31:
                data_0209211c = 5;
                data_0209f200 = 0xb;
                break;
            }
            ExitLevel();
            *(u8 *)(self + 0x6e3) = 6;
        }
    ret1:
        return 1;
    }

    {
        int id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        void *anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 0x20))
            *(unsigned short *)(self + 0x6a4) = 0x3c;
    }
    return 0;
}
