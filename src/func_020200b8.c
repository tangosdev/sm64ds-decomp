extern int GetPlayerFlagByte(int a0, int a1);

unsigned char func_020200b8(void)
{
    return GetPlayerFlagByte(0, 3) & 3;
}
