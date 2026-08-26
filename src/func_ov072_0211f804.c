//cpp
#include "SnowmanBody.h"

int SnowmanBody::InitState3()
{
    char *c = (char *)this;
    *(char *)(c + 0x3a2) = 0;
    *(int *)(c + 0x394) = 3;
    return 1;
}
