typedef struct Pair { int a; int b; } Pair;

extern void FS_InitFile(void *s);
extern void func_0205e088(Pair *dst, void *src);
extern int func_02018dc4(void *a, Pair bc);
extern void func_0205e0b0(void *info);
extern int func_02018d48(void *a, int b, int c);
extern void func_0206d32c(int a, int b, int c, int d);
extern int FS_CloseFile(void *self);

int func_02018b64(char *self)
{
    char local[0x48];
    Pair v;

    FS_InitFile(local);
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

    FS_CloseFile(local);
    return 1;
}
