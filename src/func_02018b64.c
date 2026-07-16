typedef struct Pair { int a; int b; } Pair;

extern void func_0205d874(void *s);
extern void func_0205e088(Pair *dst, void *src);
extern int func_02018dc4(void *a, Pair bc);
extern void func_0205e0b0(void *info);
extern int func_02018d48(void *a, int b, int c);
extern void func_0206d32c(int a, int b, int c, int d);
extern int func_0205d4cc(void *self);

int func_02018b64(char *self)
{
    char local[0x48];
    Pair v;

    func_0205d874(local);
    func_0205e088(&v, self);
    if (func_02018dc4(local, v) == 0)
        return 0;

    func_0205e0b0(self);
    func_02018d48(local, *(int *)(self + 4),
                  *(int *)(local + 0x24) - *(int *)(local + 0x20));

    {
        int d = *(int *)(self + 8);
        int c = *(int *)(local + 0x20);
        int b = *(int *)(self + 4);
        int flag = (int)((*(int *)(self + 0x20) == 0) ? (char *)0 : (char *)1);
        func_0206d32c(flag, b, c, d);
    }

    func_0205d4cc(local);
    return 1;
}
