struct B4 {
    unsigned char v;
    unsigned char pad[3];
};

extern volatile unsigned char data_020a0e40[];
extern struct B4 data_020a0de8[];
extern struct B4 data_020a0de9[];
extern struct B4 data_020a0dea[];
extern struct B4 data_020a0deb[];

extern int func_ov004_020ae5c4(int a, int b, int c, int d, int e, int f, int g);
extern int func_020126e8(int a);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);

void func_ov006_0212157c(char *c)
{
    int i;
    int b;

    if (*(short *)(c + 0x5db8) == 0) {
        *(unsigned char *)(c + 0x5dc4) = 0;
        return;
    }

    i = data_020a0e40[0];
    b = (data_020a0de8[i].v != 0 && data_020a0de9[i].v != 0);
    if (b) {
        *(short *)(c + 0x5db0) = data_020a0dea[i].v;
        *(short *)(c + 0x5db4) = *(short *)(c + 0x5db0);
        *(short *)(c + 0x5db2) = data_020a0deb[i].v;
        *(short *)(c + 0x5db6) = *(short *)(c + 0x5db2);
        *(unsigned char *)(c + 0x5dc4) = 1;
        *(int *)(c + 0x5dac) = 0;
    }

    if (*(unsigned char *)(c + 0x5dc4) != 1) return;

    i = data_020a0e40[0];
    if (data_020a0de8[i].v != 0) {
        func_ov004_020ae5c4((int)c, *(short *)(c + 0x5db0), *(short *)(c + 0x5db2),
                            data_020a0dea[i].v, data_020a0deb[i].v, 2, 4);
        i = data_020a0e40[0];
        *(short *)(c + 0x5db0) = data_020a0dea[i].v;
        *(short *)(c + 0x5db2) = data_020a0deb[i].v;
        *(int *)(c + 0x5dac) = func_02012468(*(int *)(c + 0x5dac), 2, 0x1b0, 2, 0,
                                             func_020126e8(*(short *)(c + 0x5db0) << 12), 0, 0);
    } else {
        b = (int)(data_020a0de8[i].v == 0 && data_020a0de9[i].v != 0);
        if (b != 0) { *(unsigned char *)(c + 0x5dc5) = 1; }
    }
}
