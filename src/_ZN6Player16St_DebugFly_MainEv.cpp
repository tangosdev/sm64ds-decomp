//cpp
typedef unsigned int u32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern "C" int func_ov002_020bf224(int a, int b, int c);
extern "C" void _ZN6Player11ChangeStateERNS_5StateE(char *self, void *st);
extern "C" void func_ov002_020bedd4(char *self);

extern u8 data_020a0e40;
extern char data_0209f4a0[];
extern char data_0209f49c[];
extern char data_020a0e5a[];
extern int data_ov002_0211013c;

extern "C" int _ZN6Player16St_DebugFly_MainEv(char *self)
{
    u32 idx;
    u16 flags;

    *(int *)(self + 0x98) = 0;
    *(int *)(self + 0xa8) = 0;
    *(u8 *)(self + 0x6e9) = 0;

    if (*(s16 *)(data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
        *(u16 *)(self + 0x94) = *(s16 *)(self + 0x6d2);
        *(int *)(self + 0x98) = func_ov002_020bf224((int)self, 0x50000, 0);
    }

    idx = data_020a0e40;
    flags = *(u16 *)(data_0209f49c + idx * 0x18);
    if (flags & 2) {
        *(int *)(((long long)(int)(self + 0x60)) & 0xFFFFFFFFFFFFFFFFLL) += 0x28000;
    } else if (flags & 1) {
        *(int *)(((long long)(int)(self + 0x60)) & 0xFFFFFFFFFFFFFFFFLL) -= 0x28000;
    }

    if (*(u16 *)(data_020a0e5a + idx * 4) & 0x400) {
        _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211013c);
    }

    func_ov002_020bedd4(self);
    return 1;
}
