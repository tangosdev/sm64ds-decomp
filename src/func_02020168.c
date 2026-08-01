extern int GetPlayerFlagByte(int a0, int a1);
unsigned char func_02020168(int a0)
{
    return GetPlayerFlagByte(0, 2) & 0x20;
}
