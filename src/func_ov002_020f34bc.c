void func_ov002_020f34bc(unsigned char *s, int idx)
{
    int k = idx * 0x30;
    int flag;
    int a, b, c, d;
    int bs, cs, ds, as_;
    *(int *)(s + 0x140 + k) -= 0x1800;
    *(int *)(s + 0x144 + k) -= 0x1800;
    *(int *)(s + 0x148 + k) += 0x1800;
    *(int *)(s + 0x14c + k) += 0x1800;
    flag = 0;
    if (*(int *)(s + 0x140 + k) <= 0x80000) *(int *)(s + 0x140 + k) = 0x80000;
    if (*(int *)(s + 0x144 + k) <= 0x56000) *(int *)(s + 0x144 + k) = 0x56000;
    if (*(int *)(s + 0x148 + k) >= 0xfc000) *(int *)(s + 0x148 + k) = 0xfc000;
    if (*(int *)(s + 0x14c + k) >= 0xbc000) { *(int *)(s + 0x14c + k) = 0xbc000; flag++; }
    if (flag != 0) (*(unsigned char *)(s + 0x15e + k))++;
    a = *(int *)(s + 0x144 + k);
    b = *(int *)(s + 0x140 + k);
    c = *(int *)(s + 0x148 + k);
    d = *(int *)(s + 0x14c + k);
    bs = b >> 12;
    cs = c >> 12;
    ds = d >> 12;
    as_ = a >> 12;
    *(unsigned short *)0x4001042 = ((bs << 8) & 0xff00) | (cs & 0xff);
    *(unsigned short *)0x4001046 = ((as_ << 8) & 0xff00) | (ds & 0xff);
}
