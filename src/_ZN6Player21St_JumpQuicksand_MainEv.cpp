//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern char data_ov002_02110424;

extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(char *, char *);
extern void func_ov002_020bedd4(char *);

int _ZN6Player21St_JumpQuicksand_MainEv(char *c)
{
    u8 t = *(u8 *)(c + 0x6e5);
    (*(u8 *)(int)(((long long)(int)(c + 0x6e5)) & 0xFFFFFFFFFFFFFFFFLL))++;
    if (t < 6) {
        (*(int *)(int)(((long long)(int)(c + 0x68c)) & 0xFFFFFFFFFFFFFFFFLL)) -=
            (int)((((7 - *(u8 *)(c + 0x6e5)) << 12) * 0xcccLL + 0x800) >> 12);
        if (*(int *)(c + 0x68c) < 0x1000)
            *(int *)(c + 0x68c) = 0x1100;
    } else {
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_02110424);
    }
    func_ov002_020bedd4(c);
    return 1;
}
}
