extern void func_ov006_0211eb90(char *c, int i);
extern void func_ov006_0211ea70(char *c, int i);
extern void func_ov006_0211e8a8(char *c, int i);
extern short data_02082214[];

void func_ov006_0211f1a4(char *c, int i)
{
    char *e;
    int v;
    func_ov006_0211eb90(c, i);
    func_ov006_0211ea70(c, i);
    func_ov006_0211e8a8(c, i);
    e = c + i * 0x24;
    v = data_02082214[(*(unsigned short *)(e + 0x466c) >> 4) * 2 + 1];
    if (v >= 0)
        *(unsigned char *)(e + 0x467e) = 1;
    else
        *(unsigned char *)(e + 0x467e) = 0;
}
