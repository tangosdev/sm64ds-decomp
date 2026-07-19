typedef unsigned int u32;
extern int _ZN6Memory8AllocateEji(u32 sz, int align);
extern void func_02058200(void *c, int f, int a, int p, int s, int x);
extern void func_02057f38(void *c, int v);
extern void func_02058048(void *c);
extern unsigned char data_0209d4f8;
extern int data_0209d518;
extern char data_0209d5b8[];

#pragma opt_common_subs off
#pragma opt_propagation off
int func_0201a244(int a0, int a1, int a2, int a3, int arg5)
{
    int r3v;
    int s5;
    if (data_0209d4f8 != 0) {
        ((void (*)(int))a0)(a1);
        return 1;
    }
    data_0209d4f8 = 1;
    s5 = arg5;
    r3v = a3;
    if (a3 == 0) {
        r3v = _ZN6Memory8AllocateEji(s5, -4);
        data_0209d518 = r3v;
    }
    func_02058200(data_0209d5b8, a0, a1, r3v + s5, s5, a2);
    func_02057f38(data_0209d5b8, 0x400);
    func_02058048(data_0209d5b8);
    return 0;
}
