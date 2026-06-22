extern int func_0205d644(void *p, int b);
extern void func_0205d874(void *s);
struct TwoInt { int a, b; };
extern void func_0205d568(void *p, struct TwoInt v);
extern void func_0205d3d4(void *dst, void *src, int n);
extern void func_02050d54(void *self, int a, int b);
extern char *data_020a5bb8;

void func_02050f34(char *self, int a1, int a2, int a3)
{
    *(int *)(self + 0x84) = 0;
    *(int *)(self + 0x7c) = 0;
    *(int *)(self + 0x80) = 0;
    func_0205d644(self + 0x74, a1);
    func_0205d874(self + 0x30);
    func_0205d568(self + 0x30, *(struct TwoInt *)(self + 0x74));
    func_0205d3d4(self + 0x30, self, 0x30);
    if (a2 != 0)
        func_02050d54(self, a2, a3);
    data_020a5bb8 = self;
}
