typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern int func_0203d8fc(void);
extern int func_0200f0bc(void);
extern void DecompressLZ16(void *src, int dst);
extern int func_0201a1bc(void);
extern int func_0201a244(int a0, int a1, int a2, int a3, int a4);
extern int func_0203d7b8(void);
extern void func_0200f220(void);
extern void func_0200f13c(void);
extern void func_0203d930(void);
extern void func_020308b4(void);
extern int func_020308a8(void);
extern void _ZN5Scene14StartSceneFadeEjjt(u32 a, u32 b, u16 c);
extern void func_02012790(int x);
extern void func_02034fbc(void);

extern void *data_0208a0e4[];
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern u8 data_020a0c64[];
extern u8 data_0209fc54;

int func_02034da4(void *arg0)
{
    char *self = (char *)arg0;

    switch (*(int *)(self + 0x60)) {
    case 0:
        if (func_0203d8fc() == 0) break;
        DecompressLZ16(data_0208a0e4[func_0200f0bc()], 0x6400000);
        DecompressLZ16(data_0208a0e4[func_0200f0bc()], 0x6600000);
        data_0209d45c = 0x10;
        data_0209d454 = 0x10;
        *(volatile u32 *)0x4000000 = (*(volatile u32 *)0x4000000 & ~0x1f00) | 0x1000;
        *(volatile u32 *)0x4001000 = (*(volatile u32 *)0x4001000 & ~0x1f00) | 0x1000;
        (*(int *)(((int)self + 0x60) & 0xFFFFFFFFFFFFFFFFLL))++;
        break;

    case 1:
        {
            int r = func_0201a1bc();
            *(int *)&data_020a0c64 = r;
            if (r == 0) break;
        }
        (*(int *)(((int)self + 0x60) & 0xFFFFFFFFFFFFFFFFLL))++;
        /* fallthrough */
    case 2:
        (*(int *)(((int)self + 0x60) & 0xFFFFFFFFFFFFFFFFLL))++;
        /* fallthrough */
    case 3:
        (*(int *)(((int)self + 0x60) & 0xFFFFFFFFFFFFFFFFLL))++;
        /* fallthrough */
    case 4:
        func_0201a244((int)&func_02034fbc, 0, 0xf, 0, 0x1000);
        (*(int *)(((int)self + 0x60) & 0xFFFFFFFFFFFFFFFFLL))++;
        /* fallthrough */
    case 5:
        if (func_0201a1bc() == 0) break;
        (*(int *)(((int)self + 0x60) & 0xFFFFFFFFFFFFFFFFLL))++;
        /* fallthrough */
    case 6:
        if (func_0203d7b8() != 0) break;
        func_0200f220();
        func_0200f13c();
        func_0203d930();
        func_020308b4();
        (*(int *)(((int)self + 0x60) & 0xFFFFFFFFFFFFFFFFLL))++;
        /* fallthrough */
    case 7:
        if (func_020308a8() == 0) break;
        _ZN5Scene14StartSceneFadeEjjt(6, 0, 0x7fff);
        func_02012790(0x11f);
        (*(int *)(((int)self + 0x60) & 0xFFFFFFFFFFFFFFFFLL))++;
        break;
    }

    if (*(int *)(self + 0x60) < 8) {
        (*(int *)(((int)self + 0x64) & 0xFFFFFFFFFFFFFFFFLL))++;
        if (*(int *)(self + 0x64) >= 0x1518) {
            data_0209fc54 = 1;
        }
    }

    return 1;
}
