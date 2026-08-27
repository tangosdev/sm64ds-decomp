int func_0203da9c(void);
int GetPlayerFlagByte(int handle, int i);
void func_0203db3c(int i, unsigned char v);

void func_02020214(unsigned int flags)
{
    if (func_0203da9c() != 0) return;
    int v = GetPlayerFlagByte(0, 2);
    func_0203db3c(2, (unsigned char)(flags | (v & ~0xf)));
}
