/* storage 0x02394680 runtime 0x027C65F8 size 0x38 + pool 0x3C main ARM.
 * Nonzero check: notify and queue (2, 11), else return the block. */
extern int dword_380FFF4[];
extern void fn_02394D08(int);
extern int fn_0239477C(int, int);
int *fn_02394680(int *result) {
    if (*result != 0) {
        fn_02394D08((int)(*(int **)dword_380FFF4 + 113));
        return (int *)fn_0239477C(2, 11);
    }
    return result;
}
