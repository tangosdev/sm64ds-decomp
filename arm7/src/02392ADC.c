/* storage 0x02392ADC runtime 0x027C4A54 size 0x48 main.
 * Pool +0x48: 04000006, 048080F8, 048080FA, 0380FFF0. */

int fn_02392ADC(void) {
    volatile unsigned short *factor_reg;
    volatile unsigned short *low;
    volatile unsigned short *high_reg;
    volatile unsigned short *out;
    unsigned short factor;
    unsigned short first;
    unsigned short high;
    unsigned short second;
    unsigned int combined;
    unsigned int value;

    factor_reg = (volatile unsigned short *)0x04000006;
    low = (volatile unsigned short *)0x048080F8;
    high_reg = (volatile unsigned short *)0x048080FA;
    out = (volatile unsigned short *)0x0380FFF0;
    factor = *factor_reg;
    first = *low;
    high = *high_reg;
    second = *low;
    if (first > second)
        high = *high_reg;
    combined = second | ((unsigned int)high << 16);
    value = (combined << 1) - 127 * (unsigned int)factor;
    *out = (unsigned short)(value >> 7);
    return (int)out;
}
