extern int func_0203da9c(void);
extern unsigned char GetPlayerFlagByte(int i, int p);
extern void func_0203db3c(int i, unsigned char v);

void WarpPlayer(int i, int val)
{
    int v;
    if (i != func_0203da9c()) return;
    v = GetPlayerFlagByte(i, 0);
    func_0203db3c(0, (unsigned char)((v & ~0x70) | (val << 4)));
}
