/* storage 0x023981D0 runtime 0x027CA148 linked 0x188 main ARM.
 * RAM test over the halfword block at 0x4804000: countdown fill, two
 * alternating fills, each verified after writing; past 0x20 errors bail
 * to the flag writeback (bit 1 ORed at [*(0x380FFF4)+0x302]). */
unsigned int fn_023981D0(void) {
    unsigned int result = 0;
    {
        unsigned int i;
        unsigned short *p = (unsigned short *)0x4804000;
        unsigned short v = 0xFFFF;
        unsigned short t;
        for (i = 0; i < 0x2000; i += 2) {
            t = v--;
            *p++ = t;
        }
    }
    {
        unsigned short *p = (unsigned short *)0x4804000;
        unsigned short e = 0xFFFF;
        unsigned int j = 0;
        while (j < 0x2000) {
            if (*p != e) {
                if (result++ > 0x20)
                    goto fail;
            }
            j += 2;
            p++;
            e--;
        }
    }
    {
        unsigned short *p = (unsigned short *)0x4804000;
        unsigned short v = 0x5A5A;
        unsigned int i = 0;
        while (i < 0x2000) {
            *p++ = v;
            i += 2;
            v = ~v;
        }
    }
    {
        unsigned short *p = (unsigned short *)0x4804000;
        unsigned short e = 0x5A5A;
        unsigned int j = 0;
        while (j < 0x2000) {
            if (*p != e) {
                if (result++ > 0x20)
                    goto fail;
            }
            j += 2;
            p++;
            e = ~e;
        }
    }
    {
        unsigned short *p = (unsigned short *)0x4804000;
        unsigned short v = 0xA5A5;
        unsigned int i = 0;
        while (i < 0x2000) {
            *p++ = v;
            i += 2;
            v = ~v;
        }
    }
    {
        unsigned short *p = (unsigned short *)0x4804000;
        unsigned short e = 0xA5A5;
        unsigned int j = 0;
        while (j < 0x2000) {
            if (*p != e) {
                if (result++ > 0x20)
                    goto fail;
            }
            j += 2;
            p++;
            e = ~e;
        }
    }
fail:
    if (result != 0) {
        unsigned char *base = *(unsigned char **)0x380FFF4;
        unsigned short *reg = (unsigned short *)(base + 0x302);
        result = *reg | 2;
        *reg = (unsigned short)result;
    }
    return result;
}
