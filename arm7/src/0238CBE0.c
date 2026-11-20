/* storage 0x0238CBE0 runtime 0x03804A78 linked 0x180 wram ARM.
 * Page-bounded SPI write. Command 0x8900 / 2. */

extern void fn_02388CD8(int arg);

void fn_0238CBE0(unsigned int addr, unsigned int len, unsigned char *src) {
    if (len >= 1) {
        unsigned short parts[3];
        int j;
        int n;
        int i;
        volatile unsigned short *ctrl = (volatile unsigned short *)0x040001C0;
        volatile unsigned short *data = (volatile unsigned short *)0x040001C2;

        if (((addr + len - 1) >> 8) > (addr >> 8))
            len = (unsigned short)(256 - (unsigned char)addr);
        parts[0] = (addr & 0xFF0000) >> 16;
        parts[1] = (addr & 0xFF00) >> 8;
        parts[2] = addr & 0xFF;
        while (*ctrl & 0x80)
            fn_02388CD8(1);
        *ctrl = 0x8900;
        *data = 2;
        j = 0;
        while (j < 3) {
            while (*ctrl & 0x80)
                fn_02388CD8(1);
            *data = (unsigned char)parts[j];
            j = j + 1;
        }
        i = 0;
        n = len - 1;
        while (i < n) {
            while (*ctrl & 0x80)
                fn_02388CD8(1);
            {
                unsigned int byte = src[i];
                *data = byte & 0xFF;
            }
            i = i + 1;
        }
        while (*ctrl & 0x80)
            fn_02388CD8(1);
        *ctrl = 0x8100;
        {
            unsigned int byte = src[i];
            *data = byte & 0xFF;
        }
        while (*ctrl & 0x80)
            fn_02388CD8(1);
    }
}
