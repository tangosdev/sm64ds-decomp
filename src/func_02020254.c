extern int GetPlayerFlagByte(int a0, int a1);
unsigned char func_02020254(void)
{
    return GetPlayerFlagByte(0, 2) & 0xf;
}
