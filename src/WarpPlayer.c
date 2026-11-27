extern int func_0203da9c(void);
extern unsigned char GetLinkByte(int slot, int byte);
extern void SetLinkByte(int byte, int value);

void WarpPlayer(int i, int val)
{
    int v;
    if (i != func_0203da9c())
        return;
    v = GetLinkByte(i, 0);
    SetLinkByte(0, (unsigned char)((v & ~0x70) | (val << 4)));
}
