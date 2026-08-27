void func_0204d104(int obj, int src, int a)
{
    int t;
    int q;
    t = *(short*)(*(int*)(src + 0x14) + 4);
    q = (t - 0x1000) * (a - 0xff) / 255;
    *(short*)(obj + 0x34) = (short)(t + q);
}
