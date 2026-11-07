/* storage 0x02387840 runtime 0x037FF6D8 size 0xA0 + pool 0xA8 wram ARM.
 * Pack (a1, a2) into a stack word per the mode in dword_3808D84, then queue it.
 * The mode dispatch is a switch: b56 checks each case and defaults to the
 * shared tail, giving the BEQ/B outline shape. */
extern int fn_02387A30(int *, int, int, void *);
extern void fn_02387998(void);
extern int dword_3808D84;
int fn_02387840(unsigned a1, int a2) {
    int v2 = dword_3808D84;
    int v4;
    switch (v2) {
        case 1:
            v4 = a2 | ((a1 >> 5) & 8) | ((a1 & 0xFF) << 8);
            break;
        case 2:
            v4 = a2 | (a1 & 0xFF00) | ((a1 & 0xFF) << 16);
            break;
        case 3:
            v4 = a2 | ((a1 >> 8) & 0xFF00) | ((a1 & 0xFF00) << 8) | ((a1 & 0xFF) << 24);
            break;
        default:
            break;
    }
    return fn_02387A30(&v4, 0, v2 + 1, fn_02387998);
}
