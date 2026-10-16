/* storage 0x02395414 runtime 0x027C738C size 0x20 main. No pool. */

void fn_02395414(unsigned short *dst, unsigned short *src, unsigned int count) {
    unsigned int i;

    i = 0;
    while (i < count) {
        *dst++ = *src++;
        i = i + 1;
    }
}
