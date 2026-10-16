extern char *unk_380FFF4;
extern void fn_023A07BC(int which);

void fn_023A1BAC(unsigned short *packet) {
    unsigned idx;
    char *base;
    char **pp;

    idx = packet[1];
    pp = &unk_380FFF4;
    base = *pp;
    if (*(unsigned short *)(*(int *)(base + 0x2E0) + idx * 0x1C) != 0x40)
        return;
    {
        int off = 0x4EC;
        unsigned short *flag = (unsigned short *)(base + off);
        *flag = (unsigned short)(*flag | (1u << idx));
    }
    if (*(unsigned short *)(*pp + 0x16C) != 0)
        fn_023A07BC(1);
    if (*(unsigned short *)(unk_380FFF4 + 0x160) != 0)
        fn_023A07BC(0);
}
