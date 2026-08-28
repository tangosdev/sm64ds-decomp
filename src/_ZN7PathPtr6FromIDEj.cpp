//cpp
#include "PathPtr.h"

extern "C" {
extern int data_020a0d84;
void func_0203ad6c(PathPtr *path, int defAddr);
}

void PathPtr::FromID(unsigned int id)
{
    func_0203ad6c(this, data_020a0d84 + id * 6);
}
