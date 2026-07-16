typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern void func_0202f290(void *o);

extern u8 data_0209f5f8;
extern volatile u32 data_0209f608;
extern volatile u32 data_0209f60c;
extern volatile u8 data_0209f5fc;

void func_0202f58c(char *self)
{
    *(u8 *)(self + 0xe) = 1;

    { u32 v = *(volatile u16 *)0x4000048; v = (v & ~0x3f) | 0x1f; v = v | 0x20; *(volatile u16 *)0x4000048 = v; }
    { u32 v = *(volatile u16 *)0x4001048; v = (v & ~0x3f) | 0x1f; v = v | 0x20; *(volatile u16 *)0x4001048 = v; }
    { u32 v = *(volatile u16 *)0x4000048; v = (v & ~0x3f00) | 0x1f00; v = v | 0x2000; *(volatile u16 *)0x4000048 = v; }
    { u32 v = *(volatile u16 *)0x4001048; v = (v & ~0x3f00) | 0x1f00; v = v | 0x2000; *(volatile u16 *)0x4001048 = v; }

    *(volatile u16 *)0x400004a &= ~0x3f;
    *(volatile u16 *)0x400104a &= ~0x3f;

    *(volatile u32 *)0x4000000 = (*(volatile u32 *)0x4000000 & ~0xe000) | 0x6000;

    if (data_0209f5f8 != 0) {
        *(volatile u32 *)0x4001000 &= ~0xe000;
    } else {
        *(volatile u32 *)0x4001000 = (*(volatile u32 *)0x4001000 & ~0xe000) | 0x6000;
    }

    data_0209f608 = 0;
    data_0209f60c = 0;
    func_0202f290(self);

    data_0209f608 = 1;
    data_0209f60c = 1;
    data_0209f5fc = 0;
    func_0202f290(self);

    if ((*(volatile u16 *)0x4000050 & 0x80) == 0x80) *(volatile u16 *)0x4000050 = 0;
    if ((*(volatile u16 *)0x4001050 & 0x80) == 0x80) *(volatile u16 *)0x4001050 = 0;
}
