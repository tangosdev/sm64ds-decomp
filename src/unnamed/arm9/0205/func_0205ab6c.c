extern void Snd_SendCommand(int n, int c, int d, int packed, int z);

void func_0205ab6c(unsigned a, unsigned b, int c, int d, unsigned e, unsigned f, unsigned g)
{
    int packed = (a << 31) | (b << 30) | (e << 29) | (f << 28) | (g << 27);
    Snd_SendCommand(0xe, c, d, packed, 0);
}
