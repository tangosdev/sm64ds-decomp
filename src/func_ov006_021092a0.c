void func_ov006_021092a0(int *o) {
    o[0] = 0xc000;
    o[1] = 0xc000;
    o[6] = o[0];
    o[7] = o[1];
    o[4] = 0;
    o[5] = 0;
    *(unsigned char*)((char*)o + 0x32) = 6;
    *(short*)((char*)o + 0x2c) = 0x25;
    *(short*)((char*)o + 0x2e) = 0;
    o[10] = 1;
}
