extern int data_ov006_0212ee40[];
void func_ov006_0211ad00(char *c, int idx) {
    c += idx * 0x24;
    *(int*)(c + 0x51c4) = 0x3000;
    *(unsigned char*)(c + 0x51cd) = 1;
    *(int*)(c + 0x51bc) = -0x1000;
    *(int*)(c + 0x51b8) = data_ov006_0212ee40[*(unsigned char*)(c + 0x51d2)];
    *(unsigned char*)(c + 0x51d1) = 1;
}
