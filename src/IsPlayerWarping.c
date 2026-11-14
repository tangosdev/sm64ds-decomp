extern unsigned char _Z17GetPlayerFlagByteii(int a0, int a1);
unsigned char IsPlayerWarping(int a0)
{
    return (_Z17GetPlayerFlagByteii(a0, 0) & 0x70) >> 4;
}
