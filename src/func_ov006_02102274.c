#pragma opt_common_subs off
extern int RandomIntInternal(int *seed);
extern int func_020126e8(int a);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern int data_0209d4b8;

void func_ov006_02102274(char *c, int idx)
{
    char *slot;
    char *ip;
    char *base;
    int bit;
    int r;
    int off;

    slot = c + (idx << 6);
    off = idx << 6;

    if (*(unsigned short *)(slot + 0x5292) != 0) {
        base = c + 0x5292;
        *(unsigned short *)(base + off) =
            (unsigned short)(*(unsigned short *)(base + off) - 1);

        if (*(short *)(slot + 0x5292) < 0)
            *(unsigned short *)(slot + 0x5292) = 0;

        return;
    }

    ip = slot + 0x5000;
    *(unsigned char *)(ip + 0x296) = 1;

    r = RandomIntInternal(&data_0209d4b8);
    bit = (int)(((unsigned int)r >> 16 & 0x7fffu) * 2u >> 15);

    if (idx & 1)
        bit = *(unsigned char *)(c + 0x5298) ^ 1;

    ip = c + (idx << 6);
    ip += 0x5000;
    *(unsigned char *)(ip + 0x298) = bit;

    if (bit != 0) {
        *(unsigned char *)(ip + 0x295) = 1;
        *(int *)(ip + 0x260) = -0x10000;
        *(int *)(ip + 0x268) = 0x200;

        if (*(int *)(c + 0x5668) == 1) {
            *(int *)(ip + 0x268) = 0xf00;

            if (idx) {
                *(unsigned char *)(ip + 0x29b) =
                    *(unsigned char *)(c + 0x529b);
            } else {
                r = RandomIntInternal(&data_0209d4b8);
                bit = (int)(((unsigned int)r >> 16 & 0x7fffu) * 2u >> 15);
                ip = c + (idx << 6);
                ip += 0x5000;
                *(unsigned char *)(ip + 0x29b) = bit;
            }
        }
    } else {
        *(unsigned char *)(ip + 0x295) = 1;
        *(int *)(ip + 0x260) = 0x110000;
        *(int *)(ip + 0x268) = -0x200;

        if (*(int *)(c + 0x5668) == 1) {
            *(int *)(ip + 0x268) = -0xf00;

            if (idx) {
                *(unsigned char *)(ip + 0x29b) =
                    *(unsigned char *)(c + 0x529b);
            } else {
                r = RandomIntInternal(&data_0209d4b8);
                bit = (int)(((unsigned int)r >> 16 & 0x7fffu) * 2u >> 15);
                ip = c + (idx << 6);
                ip += 0x5000;
                *(unsigned char *)(ip + 0x29b) = bit;
            }
        }
    }

    *(int *)(c + (idx << 6) + 0x5264) = -0xf8000;
    ip = c + (idx << 6);
    ip += 0x5000;
    *(unsigned char *)(ip + 0x299) = 0;
    *(int *)(ip + 0x26c) = 0x2000;

    r = func_020126e8(*(int *)(ip + 0x260));
    *(int *)(c + (idx << 6) + 0x528c) = func_02012468(
        *(int *)(c + (idx << 6) + 0x528c),
        2, 0x198, 4, 0, 0, r, 0);
}
