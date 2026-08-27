extern unsigned char data_020a9d2c;

#define G ((unsigned char *)&data_020a9d2c)

void func_0206834c(void)
{
    unsigned char *p;
    if (G[0xc] != 2) return;
    p = *(unsigned char **)(G + 4);
    if (p[0x4aa] == 0) {
        G[0x10] = 9;
        *(unsigned char **)(G + 8) = p;
    } else {
        G[0x10] = 4;
        *(unsigned char **)(G + 8) = p + 0x220;
    }
    G[0xc] = 3;
}
