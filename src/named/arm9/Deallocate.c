extern void _ZN6Memory10DeallocateEPv(void *ptr);

void Deallocate(void *ptr)
{
    if (ptr)
        _ZN6Memory10DeallocateEPv(ptr);
}
