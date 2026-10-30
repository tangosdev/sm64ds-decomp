/* storage 0x02397980 runtime 0x027C98F8 size 0x2C main ARM.
 * Copy 16 halfwords from src into the table at *0x0380FFF4 + 828; return 0. */

extern int word_380FFF4;

int fn_02397980(unsigned short *src) {
    unsigned int i;
    unsigned short *dst = (unsigned short *)(word_380FFF4 + 828);
    for (i = 0; i < 16; i++)
        *dst++ = *src++;
    return 0;
}
