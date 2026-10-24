/* storage 0x023878E8 runtime 0x037FF780 size 0x3C + pool 0x44 wram ARM.
 * Straight-line: three calls, one numeric pool const, one callback pool. */
extern void fn_02387BC4(int);
extern int fn_02387A30(int *, int, int, void *);
extern int fn_02387AE4(int, int);
extern void fn_02387998(void);
int fn_023878E8(void) {
    fn_02387BC4(1);
    fn_02387A30((int *)0x03805988, 0, 1, fn_02387998);
    return fn_02387AE4(0, 0);
}
