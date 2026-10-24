/* storage 0x023A4340 runtime 0x027D62B8 linked 0x60 main ARM.
 * Copy count bytes from the record at base+0x2DC, starting 0x2A before
 * off. No explicit return: r0 is left as the last call wrote it. */

extern int fn_02385334(unsigned char *p);
extern int fn_02385354(int dst, int byte);

void fn_023A4340(int off, int count, int dst) {
    int ptr = *(int *)(*(unsigned char **)0x0380FFF4 + 0x2DC);
    unsigned char *p;
    if (ptr != 0) {
        p = (unsigned char *)(ptr + off - 0x2A);
        while (count != 0) {
            fn_02385354(dst, fn_02385334(p++));
            dst += 1;
            count -= 1;
        }
    }
}
