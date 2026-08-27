struct G9d2c {
    char pad0[4];
    unsigned char *p;
    char pad8[4];
    unsigned char c;
    unsigned char pad_d;
    unsigned char e;
};
extern struct G9d2c data_020a9d2c;

void func_02068188(void) {
    *(short *)(data_020a9d2c.p + 0x4a8) = 1;
    data_020a9d2c.e = data_020a9d2c.p[0x4ac];
    data_020a9d2c.c = 5;
}
