extern unsigned func_02054de8(void);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern void func_ov006_02102dbc(char (*c)[64]);

void func_ov006_021004f4(char *c, int a)
{
    char *dst;
    volatile unsigned short v;
    *(int *)(c + 0x5660) = 3;
    *(short *)(c + 0x566e) = 0x40;
    dst = (char *)func_02054de8();
    v = 0;
    MultiStore16(v, dst, 0x6000);
    *(unsigned char *)(c + 0x5677) = (unsigned char)a;
    func_ov006_02102dbc((char (*)[64])c);
}
