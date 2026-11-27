extern unsigned char GetLinkByte(int slot, int byte);
unsigned char GetControllerMode(int a0)
{
    return GetLinkByte(a0, 4) & 3;
}
