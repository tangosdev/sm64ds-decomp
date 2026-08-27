#pragma opt_propagation off
extern int data_ov006_0213d700;
extern short data_ov006_0213d6f8;
extern short data_ov006_0214255c;
extern int data_ov006_02142570;
extern int data_ov006_02142574;

int func_ov006_020f95f0(void) {
    int a = (data_ov006_0213d700 << 4) >> 16;
    int r = 0;
    int b = data_ov006_0213d6f8;
    if (b > 0x14) b = 0x14;
    if (a != b) return r;
    if (data_ov006_0214255c != 0) return r;
    if (data_ov006_02142570 != 0) {
        if (data_ov006_02142574 != 0) return r;
    }
    r = 1;
    return r;
}
