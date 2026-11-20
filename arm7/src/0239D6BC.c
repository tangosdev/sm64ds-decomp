/* storage 0x0239D6BC runtime 0x027CF634 linked 0x64 main ARM.
 * Command 1. Seed a record, then three calls. */

extern void fn_0238AB20(unsigned short *rec);
extern void fn_02384D84(void *a, void *b, int c);
extern void fn_023907BC(int arg);

int fn_0239D6BC(unsigned char *rec, unsigned short *out) {
    unsigned short *words;
    unsigned char *base;
    out[1] = 1;
    *(unsigned short *)(rec + 0xC) = 0xFFFF;
    words = (unsigned short *)(rec + 0x10);
    words[1] = 0;
    words[9] = words[3];
    fn_0238AB20(words);
    base = *(unsigned char **)0x0380FFF4;
    fn_02384D84(base + 0x1C4, base + 0x158, (int)rec);
    fn_023907BC(0);
    return 0;
}
