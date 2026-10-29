//cpp
#include "Memory.h"

extern "C" {

void Deallocate(void *ptr) {
    if (ptr)
        Memory::Deallocate(ptr);
}
}
