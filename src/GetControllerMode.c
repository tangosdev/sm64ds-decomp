extern int GetPlayerFlagByte(int a0, int a1);
unsigned char GetControllerMode(int a0)
{
    return GetPlayerFlagByte(a0, 4) & 3;
}
