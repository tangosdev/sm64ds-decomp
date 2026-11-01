/* storage 0x0238780C runtime 0x037FF6A4 size 0x2C + pool 0x34 wram ARM.
 * Publish fn_02387C98's return and its +14 halfword into the two data words. */
extern int fn_02387C98(void);
extern int dword_3808D80;
extern int dword_3808D84;
int *fn_0238780C(void) {
    int v0 = fn_02387C98();
    dword_3808D80 = v0;
    dword_3808D84 = *(unsigned short *)(v0 + 14);
    return &dword_3808D84;
}
