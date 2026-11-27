/* storage 0x023946C0 runtime 0x027C6638 size 0x3C + pool 0x40 main ARM.
 * Three calls with a stack slot: probe, check, queue (3, 12). */
extern int dword_380FFF4[];
extern int fn_0238155C(int, int *, int);
extern int fn_02394680(int *);
extern int fn_0239477C(int, int);
int fn_023946C0(void) {
    int v1;
    fn_0238155C(*(int *)(*(int *)dword_380FFF4 + 716), &v1, 1);
    fn_02394680(&v1);
    return fn_0239477C(3, 12);
}
