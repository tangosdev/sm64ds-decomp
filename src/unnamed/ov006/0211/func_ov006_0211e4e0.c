#define A(p) ((unsigned char *)(int)(p))

void func_ov006_0211e4e0(char *base)
{
    int i;

    for (i = 0; i < 0x10; i++) {
        if (*(unsigned char *)(base + 0x48a9) != 0) {
            if (*(unsigned char *)(base + 0x48a8) != 0) {
                unsigned char *p = A(base + 0x48a8);
                int v;
                *p -= 1;
                v = *(unsigned char *)(base + 0x48a8);
                if (v < 0)
                    *(unsigned char *)(base + 0x48a8) = 0;
            } else {
                *(unsigned char *)(base + 0x48a9) = 0;
                *(unsigned char *)(base + 0x48aa) = 0;
            }
        }
        base += 0xc;
    }
}
