extern int GetPlayerFlagByte(int a0, int a1);
unsigned char func_020202dc(int a0)
{
    return GetPlayerFlagByte(0, 1) & 0x7;
}
