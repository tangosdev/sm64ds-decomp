/* storage 0x0238CEE0 runtime 0x03804D78 linked 0x1A4 wram ARM.
 * SPI read with a dummy 0 before the payload. Command 0x8900 / 0xB. */

extern void fn_02388CD8(int arg);

void fn_0238CEE0(unsigned int addr, unsigned int len, unsigned char *dst) {
    if (len >= 1) {
        unsigned short parts[3];
        int j;
        unsigned int i;
        volatile unsigned short *ctrl = (volatile unsigned short *)0x040001C0;
        volatile unsigned short *data = (volatile unsigned short *)0x040001C2;
        parts[0] = (addr & 0xFF0000) >> 16;
        parts[1] = (addr & 0xFF00) >> 8;
        parts[2] = addr & 0xFF;
        while (*ctrl & 0x80)
            fn_02388CD8(1);
        *ctrl = 0x8900;
        *data = 0xB;
        for (j = 0; j < 3; j++) {
            while (*ctrl & 0x80)
                fn_02388CD8(1);
            *data = (unsigned char)parts[j];
        }
        while (*ctrl & 0x80)
            fn_02388CD8(1);
        *data = 0;
        while (*ctrl & 0x80)
            fn_02388CD8(1);
        for (i = 0; i < len - 1; i++) {
            unsigned int byte;
            *data = 0;
            while (*ctrl & 0x80)
                fn_02388CD8(1);
            byte = *data & 0xFF;
            dst[i] = byte;
        }
        *ctrl = 0x8100;
        *data = 0;
        while (*ctrl & 0x80)
            fn_02388CD8(1);
        {
            unsigned int byte = *data & 0xFF;
            dst[i] = byte;
        }
    }
}
