extern void func_ov006_020c2924(char *c);

#pragma opt_strength_reduction off
void func_ov006_0211c478(char *base)
{
    int i;
    int cnt1;
    int cnt2;
    char *b = base;

    for (cnt1 = 0; cnt1 < 10; cnt1++) {
        *(int *)(b + 0x50e8) = 0;
        *(int *)(b + 0x50ec) = 0;
        *(short *)(b + 0x50f0) = 0;
        *(unsigned char *)(b + 0x50f4) = 0;
        *(unsigned char *)(b + 0x50f5) = 0;
        *(unsigned char *)(b + 0x50f6) = 0;
        *(unsigned char *)(b + 0x50f7) = 0;
        *(unsigned char *)(b + 0x50f8) = 0;
        b += 0x14;
    }

    for (cnt2 = 0; cnt2 < 5; cnt2++) {
        *(unsigned char *)(base + 0x561a + cnt2) = 0;
        *(unsigned char *)(base + 0x561f + cnt2) = 0;
    }

    b = base;
    for (i = 0; i < 0x1e; i++) {
        *(int *)(b + 0x51b0) = 0;
        *(int *)(b + 0x51b4) = 0;
        *(int *)(b + 0x51b8) = 0;
        *(int *)(b + 0x51bc) = 0;
        *(int *)(b + 0x51c4) = 0;
        *(short *)(b + 0x51c8) = 0;
        *(unsigned char *)(b + 0x51cc) = 0;
        *(unsigned char *)(b + 0x51cd) = 0;
        *(unsigned char *)(b + 0x51ce) = 0;
        *(unsigned char *)(b + 0x51cf) = 0;
        *(unsigned char *)(b + 0x51cc) = 0;
        *(unsigned char *)(b + 0x51d0) = 0;
        *(unsigned char *)(b + 0x51d1) = 0;
        *(unsigned char *)(b + 0x51d2) = 0;
        *(unsigned char *)(b + 0x51d3) = 0;
        b += 0x24;
    }

    *(unsigned char *)(base + 0x5604) = 0;
    *(unsigned char *)(base + 0x5605) = 1;
    *(int *)(base + 0x55f8) = 0xbd000;
    *(int *)(base + 0x55fc) = 0x97000;
    *(unsigned char *)(base + 0x5606) = 0;
    *(unsigned char *)(base + 0x55f4) = 0;
    *(unsigned char *)(base + 0x55f5) = 1;
    *(int *)(base + 0x55e8) = 0xa0000;
    *(int *)(base + 0x55ec) = 0x9d000;
    *(unsigned char *)(base + 0x55f6) = 0;

    *(short *)(base + 0x5612) = 0;
    *(short *)(base + 0x5610) = *(unsigned short *)(base + 0x5612);
    *(short *)(base + 0x5614) = 0;
    *(unsigned char *)(base + 0x5625) = 0;
    *(unsigned char *)(base + 0x5624) = 0;
    *(short *)(base + 0x5616) = 0;

    func_ov006_020c2924(base + 0x4f38);
}
