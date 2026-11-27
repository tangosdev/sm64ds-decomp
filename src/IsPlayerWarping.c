extern unsigned char GetLinkByte(int slot, int byte);
unsigned char IsPlayerWarping(int a0)
{
    return (GetLinkByte(a0, 0) & 0x70) >> 4;
}
