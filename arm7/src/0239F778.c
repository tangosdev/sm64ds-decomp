/* storage 0x0239F778 runtime 0x027D16F0 size 0x80 main. */

extern unsigned int fn_0239AD50(unsigned short *arg);
extern void fn_0239AB20(unsigned short *arg);
extern void fn_02394D84(char *a, char *b, char *c);
extern void fn_023A07BC(unsigned int arg);

void fn_0239F778(unsigned short *arg) {
    char *base;
    unsigned short *field;

    arg[1] = (unsigned short)fn_0239AD50(arg + 12);
    if (arg[1] == 0xFF)
        arg[1] = 0;
    arg[2] = (unsigned short)*(unsigned int *)(*(char **)0x0380FFF4 + 0x3A4);
    if ((arg[10] & 0x4000) != 0) {
        field = arg + 9;
        *field = (unsigned short)(*field + 8);
    }
    fn_0239AB20(arg);
    base = *(char **)0x0380FFF4;
    fn_02394D84(base + 0x14C, base + 0x164, (char *)arg - 0x10);
    fn_023A07BC(1);
}
