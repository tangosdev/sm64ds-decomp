extern unsigned char data_0209f2c4;
extern unsigned char data_0209f20c;
extern unsigned char data_0209f294;
extern unsigned char data_0209f204;
extern unsigned char data_0209caa0[];
extern unsigned char data_020a0e40;
extern unsigned short data_020a0e5a;
extern unsigned char data_0209f27c;
extern void func_02012790(int arg);

int IsButtonInputValid(void)
{
    int mode;
    int val;

    if (((data_0209f2c4 | data_0209f20c | data_0209f294) & 0xff) != 0)
        goto special;
    if (data_0209f204 != 0)
        goto special;
    if (data_0209f294 != 0)
        goto special;
    mode = data_0209caa0[0x42];
    if (mode == 0) {
        val = *(unsigned short *)((char *)&data_020a0e5a + (data_020a0e40 << 2));
        if (val & 0xf)
            return 1;
    }
    if (mode == 1) {
        val = *(unsigned short *)((char *)&data_020a0e5a + (data_020a0e40 << 2));
        if (val & 0xc0f)
            return 1;
    }
    if (mode == 2) {
        val = *(unsigned short *)((char *)&data_020a0e5a + (data_020a0e40 << 2));
        if (val & 0xcff)
            return 1;
    }
    return 0;
special:
    val = *(unsigned short *)((char *)&data_020a0e5a + (data_020a0e40 << 2));
    if (val & 0xc) {
        data_0209f27c = 0;
        return 1;
    }
    if (val & ~0xc) {
        if (data_0209f27c == 0) {
            func_02012790(0xe);
            data_0209f27c = 1;
        }
    } else {
        data_0209f27c = 0;
    }
    return 0;
}
