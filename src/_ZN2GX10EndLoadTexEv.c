#include "types.h"
extern s32 data_02099fd0;    /* 0x02099fd0 */
extern u32 data_020a60b8;            /* 0x020a60b8 */
extern u32 data_020a60ac;           /* 0x020a60ac */
extern u32 data_020a60bc;           /* 0x020a60bc */
extern u32 data_020a60c0;           /* 0x020a60c0 */

extern void func_02059fa8(s32 channel);
extern void _ZN2GX13SetBankForTexEt(u32 tex);

void _ZN2GX10EndLoadTexEv(void)
{
    if (data_02099fd0 != -1) {
        func_02059fa8(data_02099fd0);
    }
    _ZN2GX13SetBankForTexEt(data_020a60b8);
    data_020a60c0 = 0;
    data_020a60bc = 0;
    data_020a60ac = 0;
    data_020a60b8  = 0;
}
