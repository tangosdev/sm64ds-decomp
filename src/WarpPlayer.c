extern int func_0203da9c(void);
extern unsigned char func_0203db4c(int i, int p);
extern void func_0203db3c(int i, unsigned char v);

void WarpPlayer(int i, int val)
{
    int v;
    if (i != func_0203da9c()) return;
    v = func_0203db4c(i, 0);
    func_0203db3c(0, (unsigned char)((v & ~0x70) | (val << 4)));
}
