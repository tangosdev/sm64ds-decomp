int func_0203da9c(void);
int GetPlayerFlagByte(int handle, int i);
void func_0203db3c(int i, unsigned char v);

void func_02020388(int handle)
{
    if(handle != func_0203da9c()) return;
    int v = GetPlayerFlagByte(handle, 0);
    func_0203db3c(0, (unsigned char)(v & ~0x70));
}
