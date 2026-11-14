#include "types.h"
extern void _Z13func_0203da2ci(int v);
extern void _Z13func_0203da4cv(void);
extern u8 data_0209fc5c[];
extern signed char data_0209fc64[];
extern u8 data_0209fc50;
extern u8 data_0209fc58;
extern int data_0209fc68;
extern int data_0209fc60;
extern u8 data_0209fc54;
extern u8 gSoftResetDisabled;

/* Start a session in link mode arg (func_0203da2c; d_s_entry_methods.cpp
   passes 0, 1 and 3, d_s_mb.cpp 2) and clear the link error bits
   (func_0203da4c). No slot is present yet and no disconnect bit is set.
   The session state data_0209fc68 starts at 1 for mode 3 and at 3
   otherwise; data_020927ec holds the per-frame handler for each state,
   func_02030e4c calls it. The skip flag gSoftResetDisabled is raised. */
void func_02030aa4(int arg)
{
    int i;
    _Z13func_0203da2ci(arg);
    _Z13func_0203da4cv();
    for (i = 0; i < 4; i++) {
        data_0209fc5c[i] = 0;
        data_0209fc64[i] = -1;
    }
    data_0209fc50 = 0;
    data_0209fc58 = 0;
    if (arg == 3)
        data_0209fc68 = 1;
    else
        data_0209fc68 = 3;
    data_0209fc60 = 0;
    data_0209fc54 = 0;
    gSoftResetDisabled = 1;
}
