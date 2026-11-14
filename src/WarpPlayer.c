extern int _Z13func_0203da9cv(void);
extern unsigned char _Z17GetPlayerFlagByteii(int i, int p);
extern void _Z13func_0203db3cii(int byte, int value);

void WarpPlayer(int i, int val)
{
    int v;
    if (i != _Z13func_0203da9cv())
        return;
    v = _Z17GetPlayerFlagByteii(i, 0);
    _Z13func_0203db3cii(0, (unsigned char)((v & ~0x70) | (val << 4)));
}
