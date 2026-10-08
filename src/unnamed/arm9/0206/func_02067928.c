#include "types.h"
extern void *data_020a9db8;
extern u8 data_020a9d28;

extern u32 _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 state);
extern int func_020687f8(void);
extern void func_020678a4(void *out, void *in, void *dst);
extern int func_02067604(void *a, void *b);
extern void func_02068680(void *dst, void *src, void *extra);
extern void func_020684d0(void *n);

#pragma opt_strength_reduction off

int func_02067928(void *a, void *b)
{
    void *dst2;
    int idx;
    char *end;
    u32 state;
    u8 i;

    end = (char*)b + 0x160 + *(int*)((char*)b + 0x2c) + *(int*)((char*)b + 0x3c);
    idx = 0xff;

    state = _ZN3IRQ7DisableEv();
    if (!func_020687f8()) {
        _ZN3IRQ7RestoreEj(state);
        return 0;
    }

    if (*(u8*)((char*)data_020a9db8 + 0x1524) + 1 > 0x10) {
        _ZN3IRQ7RestoreEj(state);
        return 0;
    }

    for (i = 0; i < 0x10; i++) {
        char *slot = (char*)data_020a9db8 + i * 0x5c4;
        if (*(void**)(slot + 0x1d38) == a) {
            _ZN3IRQ7RestoreEj(state);
            return 0;
        }
        if (*(u8*)(slot + 0x1d4a) == 0) {
            idx = i;
            break;
        }
    }
    if (i == 0x10) {
        _ZN3IRQ7RestoreEj(state);
        return 0;
    }

    *(void**)((char*)data_020a9db8 + idx * 0x5c4 + 0x1d38) = a;

    dst2 = (char*)data_020a9db8 + 0x1788 + idx * 0x5c4;
    func_020678a4(dst2, b, end);

    if (!func_02067604((char*)data_020a9db8 + 0x1d24 + idx * 0x5c4, dst2)) {
        _ZN3IRQ7RestoreEj(state);
        return 0;
    }

    func_02068680((char*)data_020a9db8 + 0x186c + idx * 0x5c4, a, (char*)data_020a9db8 + 0x1300);

    *(u8*)((char*)data_020a9db8 + idx * 0x5c4 + 0x1d19) = (u8)idx;

    func_020684d0((char*)data_020a9db8 + 0x186c + idx * 0x5c4);

    {
        u8 id = data_020a9d28;
        data_020a9d28 = id + 1;
        *(u8*)((char*)data_020a9db8 + idx * 0x5c4 + 0x1d17) = id;
    }

    *(u16*)((char*)data_020a9db8 + 0x1d46 + idx * 0x5c4) = 1;
    *(void**)((char*)data_020a9db8 + idx * 0x5c4 + 0x1d3c) = b;
    *(u8*)((char*)data_020a9db8 + idx * 0x5c4 + 0x1d4a) = 1;
    *(u8*)((((int)data_020a9db8) + 0x1524)) += 1;

    _ZN3IRQ7RestoreEj(state);
    return 1;
}
