#include "types.h"
#include "WmWork.h"
extern char* data_020a9db4;

#define PARENT_PARAM (&((WmWork *)data_020a9db4)->parentParam)

int func_02068844(void)
{
    int a = PARENT_PARAM->multiBootFlag ? 2 : 0;
    int b = PARENT_PARAM->entryFlag ? 1 : 0;
    int c = PARENT_PARAM->KS_Flag ? 4 : 0;
    int d = PARENT_PARAM->CS_Flag ? 8 : 0;
    return (d | (c | (b | a))) & 0xff;
}
