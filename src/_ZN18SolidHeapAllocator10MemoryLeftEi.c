typedef unsigned int u32;

extern int _ZN4cstd3absEi(int x);

int _ZN18SolidHeapAllocator10MemoryLeftEi(void *c, int align)
{
    u32 a;
    char *new_var2;
    u32 mask;
    u32 new_var;
    int **new_var3;
    int *fb;
    u32 aligned;
    u32 end;

    a = (u32)_ZN4cstd3absEi(align);
    mask = a - 1u;
    fb = (int *)(((long long)(int)((char *)c + 0x24)) & 0xFFFFFFFFFFFFFFFFLL);
    aligned = (mask + (u32)*(int *)(((long long)(int)((char *)c + 0x24)) & 0xFFFFFFFFFFFFFFFFLL)) & ~(a - 1u);
    end = (u32)fb[1];
    if (!(a - 1u)) {
    }
    new_var = end;
    if (aligned > new_var) {
        return 0;
    }
    new_var2 = (char *)c;
    new_var3 = &fb;
    fb = *new_var3;
    return (int)(end - ((mask + (u32)*(int *)(((long long)(int)(new_var2 + 0x24)) & 0xFFFFFFFFFFFFFFFFLL)) & ~(a - 1u)));
}