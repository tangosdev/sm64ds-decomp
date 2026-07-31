//cpp
extern "C" {
extern void CpuCopy8(int dst, int src, unsigned int n);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(unsigned int a, unsigned int b);
extern void _ZN4CP1516DrainWriteBufferEv(void);
extern int func_02060f60(void *o, int a, int b);
extern void func_0206081c(void *o);

void func_0206062c(char *self) {
    char *e0 = self + 0xe0;
    do {
        unsigned int n = *(unsigned int*)(self + 0x20);
        int dst = *(int*)(self + 0x18);
        if (n > 0x100) n = 0x100;
        CpuCopy8(dst, (int)e0, n);
        _ZN4CP1527FlushAndInvalidateDataCacheEjj((unsigned int)e0, n);
        _ZN4CP1516DrainWriteBufferEv();
        *(int*)(*(char**)self + 0xc) = (int)e0;
        *(int*)(*(char**)self + 0x10) = *(int*)(self + 0x1c);
        *(int*)(*(char**)self + 0x14) = n;
        if (func_02060f60(self, 8, 0xa) == 0) break;
        if (func_02060f60(self, 9, 1) == 0) break;
        *(int*)(self + 0x18) += n;
        *(int*)(self + 0x1c) += n;
        *(int*)(self + 0x20) -= n;
    } while (*(int*)(self + 0x20) != 0);
    func_0206081c(self);
}
}
