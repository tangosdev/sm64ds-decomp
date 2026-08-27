extern int DecIfAbove0_Byte(void *p);
extern int _Z14ApproachLinearRsss(short *r, short t, short s);
extern int func_ov027_02111d70(void *c, int a);
extern char data_ov027_02113a26[];
int func_ov027_02111c48(char *c) {
    if (DecIfAbove0_Byte(c+0x3d8) == 0) {
        unsigned char idx = *(unsigned char*)(c+0x3d9);
        short val = *(short*)(data_ov027_02113a26 + idx*0xc);
        if (_Z14ApproachLinearRsss((short*)(c+0x8e), val, 0x514) != 0) {
            func_ov027_02111d70(c, 1);
        }
    }
    return 1;
}
