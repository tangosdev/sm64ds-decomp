extern void* data_ov006_02142aa4[4];
void func_ov006_02107b70(char* p) {
    int i;
    for (i = 0; i < 4; i++) {
        data_ov006_02142aa4[i] = p;
        p += 0x18;
    }
}
