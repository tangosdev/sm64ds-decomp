extern unsigned char func_0205b4d0(int idx, int a, int b);
extern void Snd_SendCommand(int n, int a, int b, int c, int r);

void func_0205ab28(int a, int b, int c, int d, int e)
{
    unsigned char r = func_0205b4d0(a, d, e);
    Snd_SendCommand(0xf, a, b, c, r);
}
