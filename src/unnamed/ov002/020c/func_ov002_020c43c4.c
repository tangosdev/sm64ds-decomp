extern unsigned char data_0209f2d8;
extern int data_0209caa0;

void func_ov002_020c43c4(char* self, unsigned int idx) {
    int b = (int)(data_0209f2d8 == 1);
    if (b != 0) return;
    switch (idx) {
    case 1:
        if ((&data_0209caa0)[2] & 0x400) return;
        (&data_0209caa0)[2] |= 0x400;
        break;
    case 2:
        if ((&data_0209caa0)[2] & 0x800) return;
        (&data_0209caa0)[2] |= 0x800;
        break;
    case 3:
        if ((&data_0209caa0)[2] & 0x1000) return;
        (&data_0209caa0)[2] |= 0x1000;
        break;
    case 4:
        if ((&data_0209caa0)[2] & 0x2000) return;
        (&data_0209caa0)[2] |= 0x2000;
        break;
    case 5:
        if ((&data_0209caa0)[2] & 0x4000) return;
        (&data_0209caa0)[2] |= 0x4000;
        break;
    case 6:
        if ((&data_0209caa0)[2] & 0x10000000) return;
        (&data_0209caa0)[2] |= 0x10000000;
        break;
    case 0:
        break;
    }
    *(unsigned char*)(self + 0x71e) = (unsigned char)idx;
    *(unsigned char*)(self + 0x71f) = 0;
}
