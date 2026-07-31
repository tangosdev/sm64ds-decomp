extern int GetPlayerFlagByte(int a0, int a1);
unsigned char func_020201f0(int a0)
{
    return GetPlayerFlagByte(a0, 2) & 0x10;
}
