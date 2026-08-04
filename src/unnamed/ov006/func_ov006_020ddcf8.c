struct E68 {
    char pad0[0x4660];
    int f60;       /* 0x4660 */
    char pad1[4];  /* 0x4664 */
    int f68;       /* 0x4668 */
};

void func_ov006_020ddcf8(char* c, int idx) {
    int* f60 = (int*)(c + 0x4660 + idx * 0x1c);
    int v = *f60 >> 12;
    if (v < 8) {
        *f60 = 0x8000;
        *(int*)(c + 0x4668 + idx * 0x1c) = -*(int*)(c + 0x4668 + idx * 0x1c);
        return;
    }
    if (v > 0xf8) {
        *f60 = 0xf8000;
        *(int*)(c + 0x4668 + idx * 0x1c) = -*(int*)(c + 0x4668 + idx * 0x1c);
    }
}
