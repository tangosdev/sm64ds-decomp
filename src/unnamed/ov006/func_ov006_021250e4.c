#pragma opt_strength_reduction off

extern int RandomIntInternal(int* seed);
extern void func_ov006_020c1604(char* c, int unused, short a2, int a3);
extern int data_0209e650;

void func_ov006_021250e4(char* base)
{
    unsigned char flags[5];
    unsigned char* p;
    int j;
    int i;
    unsigned char idx;

    p = flags;
    for (j = 0; j < 5; j++)
        *p++ = 0;

    for (i = 0; i < 2; i++) {
        *(int*)(base + i * 8 + 0x51a8) = 0x80000;
        *(int*)(base + i * 8 + 0x51ac) = -0x40000;
        idx = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) % 5u;
        for (;;) {
            if (flags[idx] == 0) {
                *(unsigned char*)(base + i + 0x51ca) = idx;
                flags[idx] = 1;
                break;
            }
            idx++;
            idx %= 5;
        }
        *(unsigned char*)(base + i + 0x51cc) = 0;
    }

    *(int*)(base + 0x51c4) = 0;
    *(int*)(base + 0x51c0) = 1;
    *(short*)(base + 0x51c8) = 0;
    func_ov006_020c1604(base + 0x4f38, 3, 2, (int)(base + 0x51c8));

    if (*(unsigned char*)(base + 0xc4) == 0) {
        *(unsigned char*)(base + 0xc3) = 1;
        *(unsigned char*)(base + 0xc4) = 1;
        *(unsigned short*)(base + 0xc0) = 0;
    }

    *(int*)(((int)base + 0x51b8)) += 1;
}
