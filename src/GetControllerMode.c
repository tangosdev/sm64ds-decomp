extern unsigned char _Z17GetPlayerFlagByteii(int a0, int a1);
unsigned char GetControllerMode(int a0)
{
    return _Z17GetPlayerFlagByteii(a0, 4) & 3;
}
