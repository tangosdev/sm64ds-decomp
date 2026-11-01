/* storage 0x0238BEE0 runtime 0x03803D78 size 0x100 wram.
 * Pool +0x100: 027FFC30, 0000FFFF, 038098B8, 0801FFFE. */

extern int fn_0238BDF4(int, int *);
extern void fn_0238BE98(int *);
extern void fn_0238BE64(volatile int *);
extern int fn_0238BDCC(int, int *);
extern unsigned short unk_38098B8[];

int fn_0238BEE0(void) {
    int result;
    int state[2];
    int fields[2];
    unsigned char *base;
    unsigned char *rom;
    unsigned int bit;
    unsigned char kind;
    unsigned int header;

    result = 1;
    base = (unsigned char *)0x027FFC30;
    if (*(unsigned short *)base == 0xFFFF)
        return 0;
    bit = (unsigned int)base[5] << 30;
    bit >>= 31;
    if (bit == 1)
        return 0;
    fn_0238BDF4((int)unk_38098B8[1], state);
    fn_0238BE98(fields);
    rom = (unsigned char *)0x08000000;
    kind = rom[0xB2];
    if (kind == 0x96) {
        if (*(unsigned short *)base != *(unsigned short *)(rom + 0xBE))
            goto fail;
    }
    if (kind != 0x96) {
        if (*(unsigned short *)base != *(unsigned short *)0x0801FFFE)
            goto fail;
    }
    header = *(unsigned int *)(base + 8);
    if (header != *(unsigned int *)(rom + 0xAC)) {
        bit = (unsigned int)base[5] << 31;
        bit >>= 31;
        if (bit != 0)
            goto fail;
    }
    goto restore;
fail:
    {
        unsigned char *flag;

        flag = base + 5;
        *flag = (unsigned char)(*flag | 2);
    }
    result = 0;
restore:
    fn_0238BE64(fields);
    fn_0238BDCC((int)unk_38098B8[1], state);
    return result;
}
