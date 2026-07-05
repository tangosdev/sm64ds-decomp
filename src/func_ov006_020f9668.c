extern int data_ov006_0213d700;
extern short data_ov006_0213d6f8;
extern short data_ov006_0214255c;
extern int data_ov006_02142570;
extern int data_ov006_02142574;

int func_ov006_020f9668(void) {
    int ret = 1;
    int a = (data_ov006_0213d700 << 4) >> 0x10;
    int b = data_ov006_0213d6f8;
    if (b > 0x14) b = 0x14;
    if (a == b && data_ov006_0214255c == 0) {
        if (data_ov006_02142570 == 0 || data_ov006_02142574 == 0) {
            ret = 0;
        }
    }
    return ret;
}
