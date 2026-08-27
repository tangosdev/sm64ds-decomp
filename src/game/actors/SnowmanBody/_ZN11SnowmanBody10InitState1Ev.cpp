//cpp
#include "SnowmanBody.h"

int SnowmanBody::InitState1()
{
    char *c = (char *)this;
    c[0x3a2] = 0;
    *(unsigned short *)(c + 0x3a0) = 0x15;
    *(int *)(c + 0x394) = 1;
    return 1;
}
