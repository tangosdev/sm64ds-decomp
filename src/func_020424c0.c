#pragma opt_propagation off
#pragma opt_loop_invariants off
typedef unsigned int u32;

typedef struct {
    int flag;
    char pad0[0x10];
    void* (*allocFunc)(void*, u32, u32);
    void* allocArg;
    char pad1[4];
    void* outPtr;
    u32 outSize;
} WorkInfo;

typedef struct {
    int start;
    int end;
} Entry;

extern WorkInfo data_020a1fc0;
extern int data_02099e6c;

extern u32 _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 irq);
extern void* func_0205d23c(void* a, int b);
extern void func_0205d874(int* s);
extern int func_0205d5e8(char* self, int a1, int a2, int a3, int a4);
extern int func_0205d3d4(int a, int b, int c);
extern int func_0205d4cc(char* self);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 a, u32 b);
extern void _ZN4CP1516DrainWriteBufferEv(void);

void func_020424c0(void)
{
    int sizes[4];
    void* ptrs[4];
    int local[18];
    u32 sl;
    u32 irq;
    void* fp;
    int i;

    ptrs[0] = (void*)0x27ffe48;
    ptrs[1] = (void*)0x27ffe40;
    ptrs[2] = (void*)0x27ffe50;
    ptrs[3] = (void*)0x27ffe58;

    i = 0;
    sl = i + 4;
    do {
        sizes[i] = (((int*)ptrs[i])[1] + 0x1f) & ~0x1f;
        sl += sizes[i] + 4;
        i++;
    } while (i < 4);

    irq = _ZN3IRQ7DisableEv();
    fp = data_020a1fc0.allocFunc(data_020a1fc0.allocArg, sl, 0);
    _ZN3IRQ7RestoreEj(irq);

    if (data_020a1fc0.flag != 0) {
        void* sb;
        char* buf;
        int roundedSize;
        int j;

        sb = func_0205d23c(&data_02099e6c, 3);
        buf = (char*)fp + 4;
        func_0205d874(local);
        j = 0;

        do {
            int* p = (int*)ptrs[j];
            int origSize = p[1];
            int data0;
            int end;

            roundedSize = sizes[j];
            *(int*)buf = origSize;
            buf += 4;
            data0 = p[0];
            end = roundedSize + data0;
            func_0205d5e8((char*)local, (int)sb, data0, end, -1);
            func_0205d3d4((int)local, (int)buf, roundedSize);
            func_0205d4cc((char*)local);
            buf += roundedSize;
            j++;
        } while (j < 4);

        {
            Entry* entries = (Entry*)((char*)fp + 8);
            int count = (int)((u32)(((int*)ptrs[0])[1]) >> 3);
            u32 max = sl;
            int k;
            k = 0;
            if (count > 0) {
                do {
                    u32 diff = (u32)entries[k].end - (u32)entries[k].start;
                    if (max < diff) max = diff;
                    k++;
                } while (k < count);
            }
            *(int*)fp = max;
        }

        _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)fp, sl);
        _ZN4CP1516DrainWriteBufferEv();
    }

    data_020a1fc0.outPtr = fp;
    data_020a1fc0.outSize = sl;
}
