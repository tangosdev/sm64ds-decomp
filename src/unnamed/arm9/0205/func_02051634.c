int func_02050a5c(unsigned int i);
void* func_020511d4(void *owner, unsigned int elemSize, unsigned int p2, unsigned int p3, unsigned int p4);
int func_020509d8(unsigned int a, void* b, int c, int d);
void _ZN4CP1514FlushDataCacheEjj(unsigned int a, unsigned int b);

void* func_02051634(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, void* owner)
{
    int r5 = func_02050a5c(a0);
    void* r4;
    if (r5 == 0) return 0;
    r4 = func_020511d4(owner, r5 + 0x20, a1, a2, a3);
    if (r4 == 0) return 0;
    if (r5 != func_020509d8(a0, r4, r5, 0)) return 0;
    _ZN4CP1514FlushDataCacheEjj((unsigned int)r4, r5);
    return r4;
}
