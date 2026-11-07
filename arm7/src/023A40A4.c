/* storage 0x023A40A4 runtime 0x027D601C linked 0x38 main ARM.
 * Clear 200 bytes at base+0x200. The zero is a volatile halfword so it
 * is stored and reloaded. */

extern int fn_02382C80(int value, void *dst, int len);

int fn_023A40A4(void) {
    volatile unsigned short zero = 0;
    return fn_02382C80(zero, *(unsigned char **)0x0380FFF4 + 0x200, 0xC8);
}
