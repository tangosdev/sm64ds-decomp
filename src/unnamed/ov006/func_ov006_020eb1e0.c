extern int data_ov006_0213c984[];
extern void _Z14ApproachLinearRiii(int *v, int target, int step);
extern int _Z15ApproachLinear2Rsss(short *v, short target, short step);
extern void _Z14ApproachLinearRsss(short *v, short target, short step);
extern void func_0203d388(int *p, int angle);
extern void func_0203d704(int *o, int *a, int *b);

void func_ov006_020eb1e0(char *c)
{
    int va[2];
    int vb[2];
    short tgt;

    _Z14ApproachLinearRiii((int *)(c + 0x48), 0, 0x200);
    if (_Z15ApproachLinear2Rsss((short *)(c + 0x78), 0, 1) != 0) {
        int *p10 = (int *)(((int)c + 0x10));
        int *pd = (int *)(((int)data_ov006_0213c984));
        if (p10[0] == pd[0] &&
            (p10[1] == pd[1] || *(int *)(c + 0x10) == 0)) {
            *(unsigned char *)(c + 0x94) = 1;
            *(int *)(c + 0x48) = 0x3000;
            *(unsigned char *)(c + 0x93) = 0;
            *(unsigned char *)(c + 0x95) = 0;
        }
        return;
    }
    {
        int z = 0;
        int n = 0x10000;
        n = -n;
        va[0] = z;
        va[1] = n;
    }
    tgt = *(short *)(c + 0x76) - 0x1000;
    _Z14ApproachLinearRsss((short *)(c + 0x7a), tgt, 0x400);
    func_0203d388(va, *(short *)(c + 0x7a));
    func_0203d704(vb, (int *)(c + 0x20), va);
    *(int *)(c + 0x18) = vb[0];
    *(int *)(c + 0x1c) = vb[1];
    {
        short cnt = *(short *)(c + 0x78);
        if ((cnt & 0x10) == 0 || cnt < 0x38)
            *(unsigned char *)(c + 0x95) = 1;
        else
            *(unsigned char *)(c + 0x95) = 0;
    }
}
