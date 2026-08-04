extern short data_ov006_02141314[];
extern short data_ov006_02141590[];
extern void func_ov006_020d01e0(short* g, short* a, short* b);
void func_ov006_020d0bd8(void) {
    short a[2];
    short b[2];
    a[0] = 0x18;
    a[1] = 0xb0;
    b[0] = 0xe8;
    b[1] = 0xb0;
    func_ov006_020d01e0(data_ov006_02141314, a, b);
    data_ov006_02141590[0x54] = 3;
    data_ov006_02141590[0x55] = 0;
    data_ov006_02141590[0x51] = 3;
}
