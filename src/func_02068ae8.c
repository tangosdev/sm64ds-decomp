#pragma opt_strength_reduction off

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;

extern u32 _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 prev);

extern char *data_020a9db4;
extern char *data_020a9db8;

extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void func_020672a4(u32 val);
extern void func_0205a588(void *dst, int val, u32 size);
extern void func_020684a8(void);
extern int func_02068d08(void);

extern void func_02066a94(int arg0, char *arg1);
extern void func_02069994(char *arg0);

int func_02068ae8(int param0)
{
    u32 saved;
    volatile u16 sp0;
    volatile u16 sp2;
    u32 cb;
    int i;
    int result;

    saved = _ZN3IRQ7DisableEv();

    *(u16 *)(data_020a9db4 + 0x1300 + 0x30) = param0;

    cb = *(u32 *)(data_020a9db8 + 0x1000 + 0x4e4);

    sp0 = 0;
    MultiStore16(sp0, data_020a9db8 + 0x1340, 0x60a0);

    func_020672a4(cb);

    *(int *)(data_020a9db8 + 0x1000 + 0x318) = *(u16 *)(data_020a9db4 + 0x1400) - 6;
    *(int *)(data_020a9db8 + 0x1000 + 0x31c) = *(u16 *)(data_020a9db4 + 0x1400 + 2) - 2;
    *(int *)(data_020a9db8 + 0x1000 + 0x320) = 30 / (*(u16 *)(data_020a9db4 + 0x1400 + 2) - 2);

    for (i = 0; i < 0xf; i++) {
        *(int *)(data_020a9db8 + 0x1000 + 0x4e8 + i * 4) = 0;
        *(s8 *)(data_020a9db8 + 0x1000 + 0x526 + i) = -1;
    }
    *(u8 *)(data_020a9db8 + 0x1000 + 0x524) = 0;

    sp2 = 0;
    MultiStore16(sp2, data_020a9db8 + 0x1788, 0x5c40);

    func_0205a588(data_020a9db8 + 0x1754, 0, 0x1e);

    *(u16 *)(data_020a9db4 + 0x1e00 + 0x24) = 1;
    *(void (**)(int, void *))(data_020a9db4 + 0x1000 + 0xe1c) = (void (*)(int, void *))func_02066a94;
    *(void (**)(char *))(data_020a9db4 + 0x1000 + 0x40c) = func_02069994;

    *(u16 *)(data_020a9db4 + 0x1300 + 0x32) = *(u16 *)(data_020a9db4 + 0x1400);
    *(u16 *)(data_020a9db4 + 0x1e00 + 0x18) =
        (*(u16 *)(data_020a9db4 + 0x1300 + 0x32) + 0x23) & ~0x1f;

    *(u16 *)(data_020a9db4 + 0x1300 + 0x34) = *(u16 *)(data_020a9db4 + 0x1400 + 2);
    *(u16 *)(data_020a9db4 + 0x1e00 + 0x1a) =
        (((*(u16 *)(data_020a9db4 + 0x1300 + 0x34) + 0xe) * 0xf + 0x29) & ~0x1f) << 1;

    func_020684a8();
    result = func_02068d08();

    _ZN3IRQ7RestoreEj(saved);

    return result;
}
