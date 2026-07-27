typedef unsigned char u8;

extern u8 data_020a0e40[];
extern u8 data_020a0de8[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];

void func_ov006_020e5a0c(char *base, int idx)
{
    u8 i = data_020a0e40[0];
    int off = i * 4;
    if (data_020a0de8[i * 4] != 0) {
        int bp;
        int t;
        int av;
        int bv;
        int cv;
        int dv;
        bp = *(int *)(base + 0x4674 + idx * 0x30);
        *(int *)(base + 0x4660 + idx * 0x30) = bp + (data_020a0dea[off] << 12);
        t = *(int *)(base + 0x4660 + idx * 0x30) >> 12;
        if (t < 0xe) *(int *)(base + 0x4660 + idx * 0x30) = 0xe000;
        if (t > 0xf2) *(int *)(base + 0x4660 + idx * 0x30) = 0xf2000;
        av = *(int *)(base + 0x4660 + idx * 0x30) >> 12;
        bv = av - data_020a0dea[i * 4];
        cv = *(int *)(base + 0x4664 + idx * 0x30) >> 12;
        dv = cv - data_020a0deb[i * 4];
        *(int *)(base + 0x4674 + idx * 0x30) = bv << 12;
        *(int *)(base + 0x4678 + idx * 0x30) = dv << 12;
    } else {
        int dx;
        int cy;
        int dy;
        *(u8 *)(base + idx * 0x30 + 0x4688) = 0;
        dx = (*(int *)(base + 0x5584) - *(int *)(base + idx * 0x30 + 0x4660)) >> 12;
        cy = *(int *)(base + idx * 0x30 + 0x4664);
        dy = (*(int *)(base + 0x5588) - cy) >> 12;
        if (dx < -0x2e) return;
        if (dx > 0x2e) return;
        if (dy < -0x14) return;
        if (dy > 0x14) return;
        *(int *)(base + 0x5588) = cy + 0x15000;
    }
}
