void func_ov006_0211cc90(unsigned char* base) {
    base += 0x4000;
    if (base[0xbe0] != 0) {
        base[0xbe5] = 1;
    }
}
