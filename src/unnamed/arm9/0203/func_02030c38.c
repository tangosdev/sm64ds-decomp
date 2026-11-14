#include "types.h"
extern int _Z13func_0203d950i(int i);
extern void _Z13func_02030790v(void);

extern unsigned char data_0209fc5d[];
extern unsigned char data_0209fc58;

/* Session state 4's per-frame handler (data_020927ec[4]): a peer slot
   (1..3) that is seated but no longer present on the link (func_0203d950)
   gets its disconnect bit in data_0209fc58 and ends the session
   (func_02030790). */
void func_02030c38(void)
{
    int i;
    unsigned char *p = data_0209fc5d;
    for (i = 1; i < 4; i++) {
        if (*p != 0 && _Z13func_0203d950i(i) == 0) {
            data_0209fc58 |= 1 << i;
            _Z13func_02030790v();
        }
        p++;
    }
}
