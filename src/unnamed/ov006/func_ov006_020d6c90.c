extern void func_ov006_020d68a8(void *c, int idx);

void func_ov006_020d6c90(char *c, int idx)
{
    char *slot = c + (idx << 6);
    unsigned char flag = *(unsigned char *)(slot + 0x4696);
    int x = *(int *)(slot + 0x4660) >> 12;
    int y = *(int *)(slot + 0x4664) >> 12;

    if (flag != 0) {
        if (x >= 0x40)
            return;
        if (y <= 0x40)
            return;
        if (y >= 0x80)
            return;
        *(short *)(slot + 0x4692) = 0;
        *(unsigned char *)(slot + 0x4697) = 4;
        *(unsigned char *)(slot + 0x4694) = 3;
        *(unsigned char *)(slot + 0x4695) = 0;
        *(short *)(slot + 0x468e) = 0;
        *(unsigned char *)(slot + 0x469a) = 1;
        *(unsigned char *)(slot + 0x469d) = 1;
        func_ov006_020d68a8(c, idx);
    } else {
        if (x <= 0xc0)
            return;
        if (y <= 0x40)
            return;
        if (y >= 0x80)
            return;
        *(short *)(slot + 0x4692) = 0;
        *(unsigned char *)(slot + 0x4697) = 4;
        *(unsigned char *)(slot + 0x4694) = 3;
        *(unsigned char *)(slot + 0x4695) = 0;
        *(short *)(slot + 0x468e) = 0;
        *(unsigned char *)(slot + 0x469a) = 2;
        *(unsigned char *)(slot + 0x469d) = 1;
        func_ov006_020d68a8(c, idx);
    }
}
