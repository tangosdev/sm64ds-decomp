extern void func_0205d874(int *s);
extern int func_0205d518(int *s, int a);
extern void func_0205d4cc(int *s);
extern void FS_ReadFile(int *s, int a, int b);

int func_020685cc(int a, int b, int flag)
{
    int local[0x12];
    int sz;
    int param;

    sz = flag ? 0x200 : 0x20;
    if (flag)
        b += 0x20;

    if (a == 0)
        return 0;

    func_0205d874(local);

    if (func_0205d518(local, a) == 0)
        return 0;

    param = local[9] - local[8];
    if (sz != param) {
        func_0205d4cc(local);
        return 0;
    }

    FS_ReadFile(local, b, sz);
    func_0205d4cc(local);
    return 1;
}
