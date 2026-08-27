extern int GetPlayerFlagByte(int i, int n);
extern void func_0203db3c(int i, int v);

void func_02020028(int a, int b)
{
    int x = GetPlayerFlagByte(a, 4);
    x = b | (x & ~3);
    func_0203db3c(4, x & 0xff);
}
