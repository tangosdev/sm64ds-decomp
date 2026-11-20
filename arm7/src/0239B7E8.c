/* storage 0x0239B7E8 runtime 0x027CD760 size 0x64 main ARM.
 * Pool: =0x380FFF4. Callees fn_0239AE84, fn_0239477C. */

extern int fn_0239AE84(void *a1, int a2);
extern void fn_0239477C(int a1, int a2);

void fn_0239B7E8(void)
{
    int **v0 = (int **)0x380FFF4;
    *(unsigned short *)((char *)*v0 + 0x3E0) &= ~1;
    fn_0239AE84((char *)*v0 + 0x1C4, *(int *)((char *)*v0 + 0x3DC));
    if (*(unsigned short *)((char *)*v0 + 0x1CC) != 0)
        fn_0239477C(2, 11);
}
