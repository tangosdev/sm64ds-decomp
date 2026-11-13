//cpp
#include "Memory.h"



void Deallocate(void *ptr) {
    if (ptr)
        Memory::Deallocate(ptr);
}
