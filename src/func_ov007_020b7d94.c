struct Foo {
    int p0;
    int p4;            /* 0x4 */
    int p8;            /* 0x8 */
    unsigned short pc; /* 0xc */
    unsigned short pe; /* 0xe */
};

struct Bar {
    char pad[0x18];
    int p18;           /* 0x18 */
};

extern struct Foo *data_ov007_02103460;
extern struct Bar data_ov007_02102ddc;
extern unsigned char data_ov007_020d7b04[];
extern unsigned char *data_ov007_02103474;

extern void func_ov007_020b8860(int a, int b);
extern void func_ov007_020b88c0(void);

void func_ov007_020b7d94(int kind)
{
    switch (kind) {
        case 0:
            return;
        case 1:
        case 2:
        case 6:
        case 7:
        case 8:
            data_ov007_02103460->p4 = 0x80000;
            data_ov007_02103460->p8 = 0x62000;
            data_ov007_02103460->pc = 0xc0;
            data_ov007_02103460->pe = 0x58;
            break;
        case 3:
        case 4:
        case 5:
        case 10:
        case 11:
            data_ov007_02103460->p4 = 0x80000;
            data_ov007_02103460->p8 = 0x5a000;
            data_ov007_02103460->pc = 0xc0;
            data_ov007_02103460->pe = 0x40;
            data_ov007_02102ddc.p18 = 0xa;
            break;
        case 9:
        case 12:
            data_ov007_02103460->p4 = 0x80000;
            data_ov007_02103460->p8 = 0x6e000;
            data_ov007_02103460->pc = 0xc8;
            data_ov007_02103460->pe = 0x68;
            data_ov007_02102ddc.p18 = 0xa;
            break;
    }
    data_ov007_02102ddc.p18 = data_ov007_02103474[(int)&data_ov007_020d7b04[kind * 6]];
    func_ov007_020b8860(kind, 0);
    func_ov007_020b88c0();
}
