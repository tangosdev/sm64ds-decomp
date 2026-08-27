extern void func_ov006_0211f454(char *c, int i);
extern void func_ov006_0211f34c(char *c, int i);
extern short data_02082214[];

void func_ov006_0211f554(char *c, int i)
{
    int off;
    char *e;
    int v;
    unsigned short h;
    int t;

    off = i;
    off *= 0x24;
    e = c;
    e += off;
    *(unsigned short *)(e + 0x466e) = 0;
    func_ov006_0211f454(c, i);
    func_ov006_0211f34c(c, i);
    e = c;
    e += off;
    h = *(unsigned short *)(e + 0x466c);
    t = (int)(h >> 4);
    t = t * 2 + 1;
    v = data_02082214[t];
    if (v >= 0)
        *(unsigned char *)(e + 0x467e) = 1;
    else
        *(unsigned char *)(e + 0x467e) = 0;
}
