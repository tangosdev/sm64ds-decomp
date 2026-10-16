/* storage 0x0238C0B4 runtime 0x03803F4C size 0x100 wram.
 * Pool +0x100: 03805F1C, 027FFC3C, 038098BC, 038098C0, 03805F18. */

extern int fn_0238BFF0(void);
extern int fn_0238BEE0(void);
extern void fn_02380DB0(int);
extern int fn_02383024(int, int, int);

extern int dword_3805F1C;
extern int dword_3805F18;
extern int dword_38098BC;
extern int dword_38098C0;

void fn_0238C0B4(void) {
    int saved;
    unsigned int counter;

    saved = dword_3805F1C;
    if (saved == -1) {
        dword_3805F1C = *(int *)0x027FFC3C + 10;
        return;
    }
    if (dword_38098BC != 0)
        return;
    if (dword_38098C0 != 0)
        return;
    counter = *(unsigned int *)0x027FFC3C;
    if (counter < (unsigned int)saved)
        return;
    dword_3805F1C = *(int *)0x027FFC3C + 10;
    dword_38098C0 = fn_0238BFF0();
    if (fn_0238BEE0() == 0) {
        if (dword_3805F18 != 0) {
            dword_38098BC = 1;
            return;
        }
        dword_38098C0 = 1;
    }
    dword_3805F18 = 0;
    if (dword_38098C0 != 0) {
        while (fn_02383024(13, 17, 0) != 0)
            fn_02380DB0(100);
    }
}
