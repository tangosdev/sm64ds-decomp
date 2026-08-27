void func_ov006_0211b3ec(char *base, int index)
{
    int dir;

    *(int *)(base + 0x51b0 + index * 0x24) += *(int *)(base + 0x51b8 + index * 0x24);

    dir = *(unsigned char *)(base + 0x51d3 + index * 0x24);
    if (dir == 0) {
        *(int *)(base + 0x51b8 + index * 0x24) -= 0x100;
        if (*(int *)(base + 0x51b8 + index * 0x24) < -0xc00) {
            *(int *)(base + 0x51b8 + index * 0x24) = -0xc00;
            *(unsigned char *)(base + 0x51d3 + index * 0x24) = 1;
        }
    } else if (dir != 0) {
        *(int *)(base + 0x51b8 + index * 0x24) += 0x100;
        if (*(int *)(base + 0x51b8 + index * 0x24) > 0xc00) {
            *(int *)(base + 0x51b8 + index * 0x24) = 0xc00;
            *(unsigned char *)(base + 0x51d3 + index * 0x24) = 0;
        }
    }

    *(int *)(base + 0x51b4 + index * 0x24) += *(int *)(base + 0x51bc + index * 0x24);
    *(int *)(base + 0x51bc + index * 0x24) -= 0xc0;

    if ((*(int *)(base + 0x51c0 + index * 0x24) - *(int *)(base + 0x51b4 + index * 0x24)) >> 12 >= 0x40) {
        *(unsigned char *)(base + 0x51cc + index * 0x24) = 0;
        *(unsigned char *)(base + 0x51cd + index * 0x24) = 0;
    }
}
