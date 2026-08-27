/* func_0203f604 at 0x0203f604
 *
 * Combined allocate/free helper. r0 (first arg) is unused.
 *   if (ptr == 0) return Memory::Allocate(size, 0x20);
 *   Memory::Deallocate(ptr);
 *   return 0;
 *
 *   0x0203c1d8 = Memory::Allocate(u32 size, s32 align)
 *   0x0203c1b4 = Memory::Deallocate(void *ptr)
 */

extern void *_ZN6Memory8AllocateEji(unsigned int size, int align); /* 0x0203c1d8 */
extern void _ZN6Memory10DeallocateEPv(void *ptr);                  /* 0x0203c1b4 */

void *func_0203f604(int unused, unsigned int size, void *ptr)
{
    if (ptr == 0)
        return _ZN6Memory8AllocateEji(size, 0x20);
    _ZN6Memory10DeallocateEPv(ptr);
    return 0;
}
