/* storage 0x02389874 runtime 0x0380170C size 0x24 wram ARM, no pools.
 * Two BLs (both in-pack). Explicit `v | a1`, no &0xFF per ROM. */

extern int fn_02389898(int a1);
extern volatile unsigned short *fn_02389958(int a1, int a2);

int fn_02389874(int a1) {
    int save = a1;
    int idx = fn_02389898(0);
    idx |= save;
    return (int)fn_02389958(0, idx);
}
