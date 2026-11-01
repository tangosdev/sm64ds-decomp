/* storage 0x02398500 runtime 0x027CA478 size 0x24 + pool 0x28 main ARM.
 * Notify then poll, both with the 0x1000000 token. */
extern void fn_02380978(int, int);
extern int fn_023808C0(int);
extern void fn_023998E0(void);
int fn_02398500(void) {
    fn_02380978(0x1000000, (int)fn_023998E0);
    return fn_023808C0(0x1000000);
}
