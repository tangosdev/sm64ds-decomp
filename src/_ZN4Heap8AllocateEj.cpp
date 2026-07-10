//cpp
/* Heap::Allocate(unsigned) at 0x0203c28c -- convenience overload that
 * forwards to Heap::Allocate(unsigned, int) with default alignment 4. */

typedef unsigned int u32;

class Heap
{
public:
    void* Allocate(u32 size);
    void* Allocate(u32 size, int align);
};

void* Heap::Allocate(u32 size)
{
    return Allocate(size, 4);
}
