/* storage 0x0239AE1C runtime 0x027CCD94 size 0x68 main. */

extern int fn_02381600(int a, int b, int c);
extern void fn_02394B88(char *a, int b);

void fn_0239AE1C(void) {
    char **pp;
    char *base;
    int slot;

    pp = (char **)0x0380FFF4;
    base = *pp;
    slot = *(int *)(base + 0x1B8);
    while (slot != -1) {
        if (fn_02381600(*(int *)(base + 0x2C8), slot, 0) == 0)
            break;
        fn_02394B88(*pp + 0x1B8, slot);
        base = *pp;
        slot = *(int *)(base + 0x1B8);
    }
}
