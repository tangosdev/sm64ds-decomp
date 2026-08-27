extern int func_0203da3c(void);
extern void func_02060890(void);
extern void func_020603c8(int x);
extern void func_0205a588(void *p, int v, int n);
extern unsigned char data_020a4b40[];

void func_02042f68(int a0, unsigned char *src)
{
    int i;
    unsigned char *d;
    if (func_0203da3c() == 2)
        return;
    func_02060890();
    func_020603c8(a0);
    func_0205a588(data_020a4b40, 0, 8);
    d = data_020a4b40;
    for (i = 0; i < 8; i++) {
        unsigned char c = src[i];
        if (c == 0)
            return;
        *d++ = c;
    }
}
