int func_0203da9c(void);
int GetPlayerFlagByte(int handle, int i);
void func_0203db3c(int i, unsigned char v);

void func_0202029c(unsigned int flags)
{
    if (func_0203da9c() != 0) return;
    int v = GetPlayerFlagByte(0, 1);
    func_0203db3c(1, (unsigned char)(flags | (v & ~7)));
}
