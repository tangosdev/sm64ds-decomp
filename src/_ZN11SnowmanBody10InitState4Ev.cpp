//cpp
#include "SnowmanBody.h"

int SnowmanBody::InitState4()
{
    char *c = (char *)this;
    *(int *)(((int)c + 0xb0)) &= ~1;
    *(int *)(c + 0x394) = 4;
    return 1;
}
