typedef unsigned short u16;

typedef struct InitArrays {
    int ptrs[0x10];
    int zeros1[0x10];
    int zeros2[0x10];
} InitArrays;

extern int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(int);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(int, int);
extern void _ZN4CP1516DrainWriteBufferEv(void);
extern int func_02061c88(int a, int b, int c);
extern void Crash(void);
extern void func_0205d89c(int x);
extern void func_0205d874(int *s);
extern void func_020652fc(int a, int b, int c);
extern void func_020424c0(void);
extern void func_0205a588(void *p, int v, int n);
extern int func_020658c0(void *node, int a1, void *a2, int a3, int arg5, int arg6);
extern void func_02042200(void);
extern void func_02041224(char *thiz, int n);
extern void func_02065234(int arg);

extern void func_020417d0(void);
extern void func_020412f0(void);
extern void func_02040f30(void);

extern int data_020a0f84;
extern int data_020a0f88;
extern char data_020a1fc0[];
extern char data_02082158[];

#pragma opt_strength_reduction off
void func_02040c34(int a0, int a1, int a2, int a3, int a4)
{
    char *G = data_020a1fc0;
    int state = _ZN3IRQ7DisableEv();
    if (data_020a0f84 != 0) {
        _ZN3IRQ7RestoreEj(state);
    } else {
        data_020a0f84 = 1;
        _ZN3IRQ7RestoreEj(state);

        *(int*)(G) = a0;
        *(u16*)(G + 0x30) = (u16)a1;
        *(int*)(G + 4) = 0;
        *(int*)(G + 8) = 0;
        *(int*)(G + 0xc) = 1;
        *(int*)(G + 0x10) = a2;
        *(int*)(G + 0x14) = a3;
        *(int*)(G + 0x18) = a4;
        *(int*)(G + 0x1c) = 0;
        *(int*)(G + 0x24) = 0;
        *(int*)(G + 0x20) = 0;
        *(int*)(G + 0x28) = 0;
        *(int*)(G + 0x2c) = 0xe;
        data_020a0f88 = 0;
        if (func_02061c88((u16)a1, (int)func_020417d0, 0) != 0)
            Crash();
        func_0205d89c(-1);

        if (*(int*)(G) != 0) {
            {
                char *base = G + 0x440;
                char *node;
                char *next;
                char *saved;
                char *zero;
                char *end;
                _ZN4CP1527FlushAndInvalidateDataCacheEjj((int)base, 0x2730);
                _ZN4CP1516DrainWriteBufferEv();
                saved = base + 0xc0;
                zero = 0;
                node = saved;
                next = node + 0xcc0;
                end = ((int)next & 1) ? base + 0x2700 : base + 0x2700;
                *(void**)(base + 0x2700) = saved;
                if (next < end) {
                    do {
                        *(void**)node = node + 0xcc0;
                        func_0205d874((int*)(node + 0x38));
                        *(int*)(node + 0x80) = (int)zero;
                        *(int*)(node + 0x7c) = (int)zero;
                        *(int*)(node + 0x88) = (((int)node + 0xcc00cc0) - (int)saved) / 0xcc0;
                        node += 0xcc0;
                    } while (node + 0xcc0 < end);
                }
                *(void**)node = 0;
                *(int*)(base + 0x2710) = 0;
                *(int*)(base + 0x270c) = 0;
                *(int*)(base + 0x2714) = 0;
                *(int*)(base + 0x2704) = 0;
                *(int*)(base + 0x2708) = 0;
                *(int*)(G + 0x28) = 0x200;
                *(int*)(base + 0x2718) = 1;
                *(int*)(base + 0x271c) = 0;
                *(int*)(base + 0x2720) = 0;
                *(int*)(base + 0x2728) = 0;
                *(int*)(base + 0x2724) = 0;
                *(int*)(base + 0x272c) = *(int*)(G + 0x28);
                _ZN4CP1527FlushAndInvalidateDataCacheEjj((int)base, 0x2730);
                _ZN4CP1516DrainWriteBufferEv();
                func_020652fc(*(int*)(G + 0x28), *(int*)(G + 0x2c), (int)func_020412f0);
                func_020424c0();
            }
            {
                char *base = G + 0x440;
                char *nd;
                nd = *(char**)(base + 0x2700);
                *(void**)(base + 0x2700) = *(void**)nd;
                func_0205a588(nd, 0, 0xcc0);
                func_020658c0(nd + 4, 0x20000, data_02082158, *(int*)(G + 0x20), *(int*)(data_020a1fc0 + 0x24), 0);
                *(void**)nd = *(void**)(base + 0x2704);
                *(void**)(base + 0x2704) = nd;
                *(int*)(nd + 0x80) = 1;
                func_02042200();
            }
        } else {
            char *base = G + 0x440;
            InitArrays *arr = (InitArrays*)base;
            int i;
            char *p;
            _ZN4CP1527FlushAndInvalidateDataCacheEjj((int)base, 0xc7c);
            _ZN4CP1516DrainWriteBufferEv();
            p = base + 0xc0;
            for (i = 0; i < 0x10; i++) {
                arr->ptrs[i] = (int)p;
                arr->zeros1[i] = 0;
                arr->zeros2[i] = 0;
                p += 0x28;
            }
            *(int*)(base + 0xc70) = 0;
            *(int*)(base + 0xc78) = 0;
            *(int*)(G + 0x28) = 0x80;
            *(int*)(base + 0xc74) = 0x100000;
            func_02041224(base, -1);
            *(int*)(G + 0x28) = 0;
            func_020424c0();
            func_02041224(base, *(int*)(data_020a1fc0 + 0x24));
            _ZN4CP1527FlushAndInvalidateDataCacheEjj((int)base, 0xc7c);
            _ZN4CP1516DrainWriteBufferEv();
            func_02065234((int)func_02040f30);
        }
    }
    _ZN3IRQ7RestoreEj(state);
}
