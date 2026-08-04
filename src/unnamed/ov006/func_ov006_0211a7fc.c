void func_ov006_0211a7fc(char *c, int idx)
{
    int n = idx * 0x24;
    int f;
    *(int *)(c + 0x51b0 + n) = *(int *)(c + 0x51b0 + n) + *(int *)(c + 0x51b8 + n);
    *(int *)(c + 0x51b4 + n) = *(int *)(c + 0x51b4 + n) + *(int *)(c + 0x51bc + n);
    *(int *)(c + 0x51bc + n) = *(int *)(c + 0x51bc + n) - 0x40;
    f = *(unsigned char *)(c + 0x51d3 + n);
    if (f == 0) {
        *(int *)(c + 0x51b8 + n) -= 0x100;
        if (*(int *)(c + 0x51b8 + n) < -0xc00) {
            *(int *)(c + 0x51b8 + n) = -0xc00;
            *(unsigned char *)(c + 0x51d3 + n) = 1;
        }
    } else if (f != 0) {
        *(int *)(c + 0x51b8 + n) += 0x100;
        if (*(int *)(c + 0x51b8 + n) > 0xc00) {
            *(int *)(c + 0x51b8 + n) = 0xc00;
            *(unsigned char *)(c + 0x51d3 + n) = 0;
        }
    }
    if ((*(int *)(c + 0x51c0 + n) - *(int *)(c + 0x51b4 + n)) >> 12 >= 0x30) {
        *(unsigned char *)(c + 0x51cc + n) = 0;
        *(unsigned char *)(c + 0x51cd + n) = 0;
    }
}
