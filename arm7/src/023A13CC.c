extern volatile unsigned short half_4808044;
extern char *unk_380FFF4;
extern void fn_023952E4(unsigned short value);
extern unsigned fn_02395334(void *bytes);
extern unsigned short fn_023952B0(void);

void fn_023A13CC(int index, char *packet) {
    unsigned i;
    unsigned short mixed;
    unsigned count;
    unsigned short *dst;

    mixed = (unsigned short)(half_4808044 + (half_4808044 << 8));
    if (mixed == 0)
        mixed = 1;
    fn_023952E4(mixed);
    *(unsigned short *)(*(int *)(unk_380FFF4 + 0x2E0) + index * 0x1C + 0xE) = mixed;
    dst = (unsigned short *)(packet + 0x34);
    count = fn_02395334(packet + 0x33);
    for (i = 0; i < count; i += 2)
        *dst++ = fn_023952B0();
}
