/* storage 0x023986F4 runtime 0x027CA66C size 0x84 + pool 0x88 main ARM.
 * Probe both device words, clear the idle halves, queue (0,14) if either
 * answered, then always queue (0,20). */
extern int dword_380FFF4[];
extern void fn_0239FA98(int);
extern void fn_0239477C(int, int);
void fn_023986F4(void) {
    int v0 = 0;
    fn_0239FA98(1);
    fn_0239FA98(v0);
    if (*(unsigned short *)*(int **)((char *)*(int **)dword_380FFF4 + 1024) != 0)
        v0 = 1;
    else
        *(unsigned short *)((char *)*(int **)dword_380FFF4 + 1016) = 0;
    if (*(unsigned short *)*(int **)((char *)*(int **)dword_380FFF4 + 1004) != 0)
        v0 = 1;
    else
        *(unsigned short *)((char *)*(int **)dword_380FFF4 + 996) = 0;
    if (v0 != 0)
        fn_0239477C(0, 14);
    fn_0239477C(0, 20);
}
