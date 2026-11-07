/* storage 0x02395434 runtime 0x027C73AC size 0x88 main. Pool +0x88: 04805F60, 0380FFF4. */

void fn_02395434(unsigned short *dst, unsigned short *src, int count) {
    unsigned int total;
    unsigned int first;
    unsigned int rest;
    unsigned int i;
    unsigned short *wrapped;
    unsigned int end;

    total = (unsigned int)(count + 1) >> 1;
    end = (unsigned int)src + (total << 1);
    if (end > 0x04805F60) {
        first = (0x04805F60 - (unsigned int)src) >> 1;
        rest = total - first;
    } else {
        first = total;
        rest = 0;
    }
    i = 0;
    while (i < first) {
        *dst++ = *src++;
        i = i + 1;
    }
    wrapped = (unsigned short *)((char *)src - *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0x96));
    i = 0;
    while (i < rest) {
        *dst++ = *wrapped++;
        i = i + 1;
    }
}
