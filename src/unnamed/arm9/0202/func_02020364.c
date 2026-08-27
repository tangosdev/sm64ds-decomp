extern int GetPlayerFlagByte(int a0, int a1);
unsigned char func_02020364(int a0)
{
    return GetPlayerFlagByte(a0, 0) & 8;
}
