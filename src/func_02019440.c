typedef unsigned short u16;

extern int SCENE_RELATED;
extern int data_0209d4ac;
extern int KS_FADER;

extern void func_0203bd24(void);
extern int func_02053be0(int enable);
extern void func_02054430(int a);
extern void MultiStore32Bytes(unsigned val, int *dst, int len);
extern void func_02053f58(void);
extern void func_0200f4f4(void);
extern void func_0200f46c(void);

void func_02019440(void)
{
    volatile int b;
    volatile int a;
    volatile int c;
    func_0203bd24();
    *(volatile u16 *)0x4000304 = (*(volatile u16 *)0x4000304 & 0xfffffdf1) | 0x20e;
    func_02053be0(1);
    func_02054430(0x1ff);
    a = 0;
    MultiStore32Bytes(a, (int *)0x6800000, 0xa4000);
    func_02053f58();
    b = 0xc0;
    MultiStore32Bytes(b, (int *)0x7000000, 0x400);
    c = 0;
    MultiStore32Bytes(c, (int *)0x5000000, 0x400);
    SCENE_RELATED = 0;
    data_0209d4ac = 0;
    KS_FADER = 0;
    func_0200f4f4();
    func_0200f46c();
}
