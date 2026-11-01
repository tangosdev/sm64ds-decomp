extern char *unk_380FFF4;
extern void fn_023952E4(unsigned short value);
extern unsigned fn_02395334(void *bytes);
extern int fn_023952B0(void);

int fn_023A132C(unsigned short *packet) {
    unsigned i;
    unsigned count;
    unsigned short *cursor;

    fn_023952E4(*(unsigned short *)(*(int *)(unk_380FFF4 + 0x2E0) + packet[1] * 0x1C + 0xE));
    cursor = (unsigned short *)((char *)packet + 0x34);
    count = fn_02395334((char *)packet + 0x33);
    for (i = 0; i < (count >> 1); i++) {
        if (*cursor++ != fn_023952B0())
            return 0;
    }
    if ((count & 1) != 0 && (unsigned char)*cursor != (unsigned char)fn_023952B0())
        return 0;
    return 1;
}
