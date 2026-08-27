extern char data_ov006_02141fe4[];
void func_ov006_020ea5f0(int x, int y) {
    char *g = data_ov006_02141fe4;
    if (*(unsigned char *)(g + 0xd) != 0) return;
    *(unsigned char *)(g + 0xd) = 1;
    if (x < 0x10000) x = 0x10000;
    else if (x > 0xf4000) x = 0xf4000;
    *(int *)(g) = x;
    *(int *)(g + 4) = y;
    *(unsigned char *)(g + 0xc) = 0x18;
    *(unsigned char *)(g + 0xe) = 0;
    *(unsigned char *)(g + 0x10) = 1;
    *(unsigned char *)(g + 0xf) = 5;
    *(int *)(g + 8) = -0x800;
}
