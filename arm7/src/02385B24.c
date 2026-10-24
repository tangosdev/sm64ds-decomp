/* storage 0x02385B24 runtime 0x037FD9BC linked 0xA4 wram ARM. */

extern int *volatile dat_38075C0;

int fn_02385B24(unsigned char *result, int arg) {
    int i;
    int *table;

    i = 0;
    result[1] = 0;
    *(int *)(result + 0x20) = arg;
    *(unsigned short *)(result + 0x18) = 120;
    *(unsigned short *)(result + 0x1a) = 0x100;
    *(unsigned short *)(result + 0x1c) = 0;
    result[5] = 127;
    *(unsigned short *)(result + 6) = 0;
    result[4] = 64;
    do {
        unsigned char *slot = result + i;
        slot[8] = 0xFF;
        i++;
    } while (i < 16);
    table = dat_38075C0;
    if (table != 0) {
        int j = 0;
        *(int *)((char *)table + 36 * result[2] + 0x40) = 0;
        do {
            *(short *)((char *)dat_38075C0 + 36 * result[2] + 2 * j + 0x20) = -1;
            j++;
        } while (j < 16);
    }
    return (int)result;
}
