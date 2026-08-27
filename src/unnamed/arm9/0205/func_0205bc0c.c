struct Pair { void *a; void *b; };
extern void func_0205c528(struct Pair *in, struct Pair *out, int n);
extern int func_0205c5e4(void *self, int x);

int func_0205bc0c(char *self)
{
    char *p = *(char **)(self + 8);
    int n = *(int *)(self + 0x30);
    struct Pair out;
    struct Pair in;
    if ((unsigned)(n << 3) >= *(unsigned *)(p + 0x24))
        return 1;
    in.a = p;
    in.b = *(char **)(p + 0x20) + (n << 3);
    func_0205c528(&in, &out, 8);
    *(void **)(self + 0x2c) = out.a;
    *(void **)(self + 0x30) = out.b;
    *(int *)(self + 0x34) = n;
    return func_0205c5e4(self, 7);
}
