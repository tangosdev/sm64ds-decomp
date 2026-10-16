/* storage 0x023949DC runtime 0x027C6954 size 0x54 + pool 0x58 main ARM.
 * Run fn_02394988 over the eight consecutive slots. */
extern int dword_380FFF4[];
extern void fn_02394988(int);
void fn_023949DC(void) {    int *v0 = (int *)((char *)*(int **)dword_380FFF4 + 320);
    fn_02394988((int)(v0 + 6));
    fn_02394988((int)(v0 + 9));
    fn_02394988((int)(v0 + 12));
    fn_02394988((int)(v0 + 15));
    fn_02394988((int)(v0 + 18));
    fn_02394988((int)(v0 + 21));
    fn_02394988((int)(v0 + 24));
    fn_02394988((int)(v0 + 27));
}
