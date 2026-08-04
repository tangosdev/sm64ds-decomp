int func_ov006_021259d8(char *p0, int *p1) {
    int a = p1[0];
    int b;
    int fa;
    int fb;
    int row;
    int col;
    unsigned int v;
    int sum;
    int diff;

    if (a < 0) {
        return 1;
    }
    if (a >= 0x100000) {
        return 1;
    }

    b = p1[1];
    if (b < 0) {
        return 0;
    }
    if (b >= (*(int *)(p0 + 0xba08) << 16)) {
        return 0;
    }
    if (b >= (*(int *)(p0 + 0xab6c) + 0x1d0000)) {
        return 0;
    }

    fa = a >> 12;
    fb = b >> 12;
    row = fa / 16;
    col = fb / 16;
    v = *(unsigned short *)(p0 + row * 0x5c0 + col * 2 + 0x4f38);
    sum = (fa & 0xf) + (fb & 0xf);
    diff = (fa & 0xf) - (fb & 0xf);

    switch (v) {
    case 0:
    case 0xd:
        return 0;
    case 0x10:
        return 1;
    case 0x11:
    case 0x1b:
        return (fa & 8) == 0;
    case 0x12:
        return sum < 0x18;
    case 0x13:
        return sum < 8;
    case 0x14:
        return diff < 8;
    case 0x15:
        return diff < -8;
    case 0x16:
    case 0x1c:
        return (fa & 8) != 0;
    case 0x17:
        return diff >= 8;
    case 0x18:
        return diff >= -8;
    case 0x19:
        return sum >= 0x18;
    case 0x1a:
        return sum >= 8;
    case 0xc:
        return 0;
    default:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 0xa:
    case 0xb:
    case 0xe:
    case 0xf:
        return 0;
    }
}
