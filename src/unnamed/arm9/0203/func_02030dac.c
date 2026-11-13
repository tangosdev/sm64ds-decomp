#include "types.h"
extern int _Z13func_0203d890v(void);
extern u8 data_0209fc5c[];
extern s8 data_0209fc64[];
extern u8 data_0209fc50;

/* Session state 1's per-frame handler (data_020927ec[1]): seat the
   players the parent posts (func_0203d890) in order, as func_020308d0
   does for a local game. */
void func_02030dac(void)
{
    int n = _Z13func_0203d890v();
    int i;
    int j;
    for (i = 0; i < n; i++) {
        data_0209fc5c[i] = 1;
        data_0209fc64[i] = i;
    }
    for (j = n; j < 4; j++) {
        data_0209fc5c[j] = 0;
        data_0209fc64[j] = -1;
    }
    data_0209fc50 = n;
}
