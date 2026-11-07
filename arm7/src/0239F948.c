/* storage 0x0239F948 runtime 0x027D18C0 size 0x64 main. */

extern unsigned int fn_0239AD50(unsigned short *arg);
extern void fn_0239F9AC(unsigned int index);

void fn_0239F948(unsigned short *arg) {
    char **pp;
    unsigned int i;
    unsigned int found;

    if ((arg[0] & 1) != 0) {
        i = 1;
        pp = (char **)0x0380FFF4;
        while (i < *(unsigned short *)(*pp + 0x200 + 0xE6)) {
            fn_0239F9AC(i);
            i = i + 1;
        }
    } else {
        found = fn_0239AD50(arg);
        if (found != 0xFF)
            fn_0239F9AC(found);
    }
}
