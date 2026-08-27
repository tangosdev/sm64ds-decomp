extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

#pragma opt_strength_reduction off
#pragma opt_common_subs off
void func_ov006_020ff534(char *c, int k)
{
    int found = 0;
    int i = 0;
    int *px = (int *)(c + k * 32 + 0x5620);
    int *py = (int *)(c + k * 32 + 0x5624);
    for (; i < 0x30; i++) {
        int dx, dy;
        if (*(unsigned char *)(c + i * 64 + 0x4698) == 0) continue;
        if (*(unsigned char *)(c + i * 64 + 0x4699) < 3) continue;
        dx = (*(int *)(c + i * 64 + 0x4660) - *px) >> 12;
        dy = (*(int *)(c + i * 64 + 0x4664) - *py) >> 12;
        if (dx < -0x18) continue;
        if (dx > 0x18) continue;
        if (dy < 0) continue;
        if (dy <= 0x40) { found++; break; }
    }
    if (found == 0) return;
    if ((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16 & 0x7fff) * 2) >> 15) {
        *(unsigned char *)(c + k * 32 + 0x5635) = 3;
        *(unsigned char *)(c + k * 32 + 0x5636) = 0;
        *(int *)(c + k * 32 + 0x5628) = 0x1800;
        *(unsigned char *)(c + k * 32 + 0x5637) = 0x10;
        *(unsigned char *)(c + k * 32 + 0x5638) = 0;
        *(unsigned short *)(c + k * 32 + 0x5630) = 0;
        *(unsigned char *)(c + k * 32 + 0x563a) = 1;
    } else {
        *(unsigned char *)(c + k * 32 + 0x5635) = 2;
        *(unsigned char *)(c + k * 32 + 0x5636) = 0;
        *(int *)(c + k * 32 + 0x5628) = -0x1800;
        *(unsigned char *)(c + k * 32 + 0x5637) = 0x10;
        *(unsigned char *)(c + k * 32 + 0x5638) = 0;
        *(unsigned short *)(c + k * 32 + 0x5630) = 0;
        *(unsigned char *)(c + k * 32 + 0x563a) = 0;
    }
}
