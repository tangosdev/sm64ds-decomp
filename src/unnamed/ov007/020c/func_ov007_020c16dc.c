void func_ov007_020c16dc(int sel, int a, int b, int c, int *out1, int *out2) {
    switch (sel) {
    case 0:
        *out1 = 0;
        *out2 = 0;
        break;
    case 1:
        *out1 = a * b;
        *out2 = c << 1;
        break;
    case 2:
        *out1 = (unsigned)(a * b) >> 2;
        *out2 = c << 1;
        break;
    case 3:
        *out1 = (unsigned)(a * b) >> 1;
        *out2 = c << 1;
        break;
    case 4:
        *out1 = a * b;
        *out2 = c << 1;
        break;
    case 5:
        *out1 = 0;
        *out2 = 0;
        break;
    case 6:
        *out1 = a * b;
        *out2 = c << 1;
        break;
    case 7:
        *out1 = 0;
        *out2 = 0;
        break;
    default:
        break;
    }
}
