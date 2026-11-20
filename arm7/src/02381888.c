/* storage 0x02381888 runtime 0x037F9720 size 0x38 wram. Nine BL relocs, no pool.
 * r0 chains through every call — each callee gets the previous return in r0,
 * so no mov r0 is ever emitted. */

extern int fn_023819D8(void);
extern int fn_02382EF8(int);
extern int fn_02380CEC(void);
extern int fn_02380A20(void);
extern int fn_02381FF8(int);
extern int fn_0238246C(int);
extern int fn_02381210(void);
extern int fn_02382BC8(int);
extern int fn_0238C3C4(int);

int fn_02381888(void) {
    int v0 = fn_023819D8();
    fn_02382EF8(v0);
    fn_02380CEC();
    v0 = fn_02380A20();
    v0 = fn_02381FF8(v0);
    fn_0238246C(v0);
    v0 = fn_02381210();
    v0 = fn_02382BC8(v0);
    return fn_0238C3C4(v0);
}
