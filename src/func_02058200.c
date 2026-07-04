typedef unsigned u32;
typedef unsigned short u16;
extern u32 _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 s);
extern int func_02058538(void);
extern void func_020584d0(void *node);
extern void func_02058568(void *a, int b, int c);
extern void MultiStore_Int(int val, int *dst, int len);
extern void func_020581a8(void);
extern int data_020a6148[];

void func_02058200(char *self, int a1, int a2, int end, int arg4, int arg5)
{
    u32 saved;
    int id;
    char *base;
    volatile int zero;
    saved = _ZN3IRQ7DisableEv();
    id = func_02058538();
    if (id < 0) {
        _ZN3IRQ7RestoreEj(saved);
        return;
    }
    *(int*)(self + 0x70) = arg5;
    *(int*)(self + 0x6c) = id;
    *(int*)(self + 0x64) = 0;
    *(int*)(self + 0x74) = 0;
    data_020a6148[id] = (int)self;
    func_020584d0(self);
    base = (char*)(end - arg4);
    *(int*)(self + 0x88) = end;
    *(int*)(self + 0x84) = (int)base;
    *(int*)(self + 0x8c) = 0;
    *(int*)(*(int*)(self + 0x88) - 4) = (int)0xfddb597d;
    *(int*)(*(int*)(self + 0x84)) = 0x7bf9dd5b;
    *(u16*)(self + 0x90) = 0;
    func_02058568(self, a1, end - 4);
    *(int*)(self + 4) = a2;
    *(int*)(self + 0x3c) = (int)&func_020581a8;
    zero = 0;
    MultiStore_Int(zero, (int*)(base + 4), arg4 - 8);
    *(int*)(self + 0x78) = 0;
    *(int*)(self + 0x7c) = 0;
    *(int*)(self + 0x80) = 0;
    _ZN3IRQ7RestoreEj(saved);
}
