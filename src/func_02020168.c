extern int GetPlayerFlagByte(int a0, int a1);
unsigned char func_02020168(void)
{
    return GetPlayerFlagByte(0, 2) & 0x20;
}
