//cpp
#include "SnowmanBody.h"

int SnowmanBody::InitState5()
{
    char *c = (char *)this;
    *(int *)(((int)c + 0xb0)) &= ~1;
    *(int *)(c + 0x394) = 5;
    return 1;
}
