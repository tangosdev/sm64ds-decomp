struct E { unsigned char b[0x14]; };
void func_ov006_0211bc68(char* c, int idx) {
    if (*(unsigned char*)(c+0x5624) == 0) {
        struct E* a = (struct E*)(c+0x5000);
        *(unsigned char*)((char*)&a[idx]+0xf4) = 3;
    }
}
