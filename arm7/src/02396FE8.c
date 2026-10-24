/* storage 0x02396FE8 runtime 0x027C8F60 size 0x94 main. */

extern void fn_023A4340(unsigned int a, unsigned int b, unsigned int *out);
extern unsigned int fn_023963F0(void);

void fn_02396FE8(void) {
    unsigned int value;

    value = 0xE2E2;
    fn_023A4340(0x58, 2, &value);
    value = value + 0x202;
    if (fn_023963F0() == 0x14) {
        unsigned int delta = 0x6161;
        value = value - delta;
        if ((*(unsigned short *)0x048080BC & 2) != 0) {
            unsigned int delta2 = 0x6060;
            value = value - delta2;
        }
    }
    *(unsigned short *)0x04808140 = (unsigned short)value;
}
