int func_0203da9c(void);
int GetPlayerFlagByte(int handle, int i);
void func_0203db3c(int i, unsigned char v);

void func_02020304(void)
{
    int v = GetPlayerFlagByte(func_0203da9c(), 0);
    func_0203db3c(0, (unsigned char)(v & ~8));
}
