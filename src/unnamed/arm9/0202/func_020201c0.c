int func_0203da9c(void);
int GetPlayerFlagByte(int handle, int i);
void func_0203db3c(int i, unsigned char v);

void func_020201c0(void)
{
    int v = GetPlayerFlagByte(func_0203da9c(), 2);
    func_0203db3c(2, (unsigned char)(v | 0x10));
}
