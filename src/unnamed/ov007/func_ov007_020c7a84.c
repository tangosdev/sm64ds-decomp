#include "nitro/fx/fx.h"
void func_ov007_020c7a84(char *p, int f1, int f2)
{
    char *a = *(char **)p;
    int t = *(int *)(*(char **)a + 8) - *(int *)(*(char **)*(char **)(p + 4) + 8) - *(int *)(p + 0x14);
    int m1 = FX_MUL(f1, t);
    int m2 = FX_MUL(f2, *(int *)(a + 0xc));
    int nm1 = -m1;
    int *pa18 = (int *)(int)(a + 0x18);
    *pa18 = *pa18 + (nm1 - m2);
    {
        char *b = *(char **)(p + 4);
        int m3 = FX_MUL(f2, *(int *)(b + 0xc));
        int *pb18 = (int *)(int)(b + 0x18);
        *pb18 = *pb18 - (nm1 + m3);
    }
}
