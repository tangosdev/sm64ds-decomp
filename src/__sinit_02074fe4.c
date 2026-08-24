#include "dBgPc.h"

extern void func_020731dc();
extern struct dBgPc data_020a0cec[];
extern int data_020a0ce0[];
void __sinit_02074fe4(void)
{
    dBgPc_Construct(data_020a0cec);
    func_020731dc(data_020a0cec, dBgPc_Destroy, data_020a0ce0);
}
