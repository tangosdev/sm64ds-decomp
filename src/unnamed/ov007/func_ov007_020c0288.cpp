//cpp

extern "C" {
/* Sets the palette (bits 12-15) of count BG screen entries, keeping their
 * tile and flip bits. */
void func_ov007_020c0288(unsigned short *entries, int count, int palette) {
    int i;
    for (i = 0; i < count; i++) {
        *entries = (palette << 12) | (*entries & 0xfff);
        entries++;
    }
}
}
