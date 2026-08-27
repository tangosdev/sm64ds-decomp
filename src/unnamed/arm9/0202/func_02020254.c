extern int GetPlayerFlagByte(int a0, int a1);
unsigned char func_02020254(int a0)
{
    return GetPlayerFlagByte(0, 2) & 0xf;
}
