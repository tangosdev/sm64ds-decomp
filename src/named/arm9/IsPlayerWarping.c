extern int GetPlayerFlagByte(int a0, int a1);
unsigned char IsPlayerWarping(int a0)
{
    return (GetPlayerFlagByte(a0, 0) & 0x70) >> 4;
}
