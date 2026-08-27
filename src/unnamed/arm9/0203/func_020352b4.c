#include "types.h"
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN7fBase_cC2Ev(void *);

extern int data_0208e4b8[];
extern int _ZTV8dScene_c[];
extern int data_020943c4[];
extern int data_0208eafc[];
extern int data_0208eacc[];
extern int data_0208eb2c[];
void *func_020352b4(void)
{
    char *p = (char *)_ZN7fBase_cnwEj(0x68);
    if (p) {
        _ZN7fBase_cC2Ev(p);
        *(int **)p = data_0208e4b8;
        *(int **)p = _ZTV8dScene_c;
        {
            u8 *bp = (u8 *)((int)p + 0x13);
            *bp |= 1;
            *bp |= 4;
        }
        *(int **)p = data_020943c4;
        {
            int *fp = (int *)((int)p + 0x50);
            fp[0] = (int)data_0208eafc;
            fp[0] = (int)data_0208eacc;
            fp[1] = 0x1000;
            fp[2] = 0;
            fp[0] = (int)data_0208eb2c;
            *(short *)(fp + 3) = 0;
        }
    }
    return p;
}
