struct E6 { unsigned char pad[0xe]; unsigned char e; unsigned char f; unsigned char pad2[4]; };
extern struct E6 data_ov006_02142044[];
void func_ov006_020ea8e0(void) {
    int i;
    for (i = 0; i < 0x10; i++) {
        if (data_ov006_02142044[i].e == 0) {
            data_ov006_02142044[i].e = 0;
            data_ov006_02142044[i].f = 0;
        }
    }
}
