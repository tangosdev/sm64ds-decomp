struct S2 { int w[2]; };
extern struct S2 data_ov006_0213cb54;

void func_ov006_020ee658(char *c) {
    *(short *)(c + 0x5014) = 0x78;
    *(int *)(c + 0x500c) = 0;
    *(int *)(c + 0x5010) = 0x4000;
    *(struct S2 *)(c + 0x5004) = data_ov006_0213cb54;
}
