extern int data_ov006_02137cd8[];
extern void func_ov004_020b1d60(void *glyph, int x, int a2, int a3, int a4);

void func_ov004_020b1ea4(int x, int a1, int val, int a3, int a4, int mode, int fb) {
    int d[6];
    int i;
    if (val >= 0xf423f) val = 0xf423f;
    for (i = 0; i < 6; i++) d[i] = 0;
    d[0] = val;
    while (d[0] >= 0x186a0) { d[5]++; d[0] -= 0x186a0; }
    while (d[0] >= 0x2710) { d[4]++; d[0] -= 0x2710; }
    while (d[0] >= 0x3e8) { d[3]++; d[0] -= 0x3e8; }
    while (d[0] >= 0x64) { d[2]++; d[0] -= 0x64; }
    while (d[0] >= 0xa) { d[1]++; d[0] -= 0xa; }
    if (fb == 0) fb = 0xa;
    else if (fb == 0) fb = 0xa;
    else if (fb == 0x14) fb = 0x1e;
    else if (fb == 0xa) fb = 0xa;

    if (d[5] != 0) {
        int r5;
        int k;
        if (mode == 0) x -= 0x14;
        else if (mode == 1) x -= 0x28;
        r5 = 5;
        for (k = 0; k < 5; k++) {
            func_ov004_020b1d60((void*)data_ov006_02137cd8[fb + d[r5]], x, a1, a3, a4);
            x += 8;
            r5--;
        }
    } else if (d[4] != 0) {
        int r5;
        int k;
        if (mode == 0) x -= 0x10;
        else if (mode == 1) x -= 0x20;
        r5 = 4;
        for (k = 0; k < 4; k++) {
            func_ov004_020b1d60((void*)data_ov006_02137cd8[fb + d[r5]], x, a1, a3, a4);
            x += 8;
            r5--;
        }
    } else if (d[3] != 0) {
        int r5;
        int k;
        if (mode == 0) x -= 0xc;
        else if (mode == 1) x -= 0x18;
        r5 = 3;
        for (k = 0; k < 3; k++) {
            func_ov004_020b1d60((void*)data_ov006_02137cd8[fb + d[r5]], x, a1, a3, a4);
            x += 8;
            r5--;
        }
    } else if (d[2] != 0) {
        int r5;
        int k;
        if (mode == 0) x -= 8;
        else if (mode == 1) x -= 0x10;
        r5 = 2;
        for (k = 0; k < 2; k++) {
            func_ov004_020b1d60((void*)data_ov006_02137cd8[fb + d[r5]], x, a1, a3, a4);
            x += 8;
            r5--;
        }
    } else if (d[1] != 0) {
        if (mode == 0) x -= 4;
        else if (mode == 1) x -= 8;
        func_ov004_020b1d60((void*)data_ov006_02137cd8[fb + d[1]], x, a1, a3, a4);
        x += 8;
    }
    func_ov004_020b1d60((void*)data_ov006_02137cd8[d[0] + fb], x, a1, a3, a4);
}
