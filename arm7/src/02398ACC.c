/* storage 0x02398ACC runtime 0x027CAA44 size 0xF4 + pool 0x108 main ARM.
 * Status-gated popcount then queue (derived width, table); else queue (0, 16). */
extern int dword_380FFF4[];
extern void fn_023954E8(int, int, void *, int);
extern void fn_0239477C(int, int);
void fn_02398ACC(int a1) {
    int *pool = (int *)dword_380FFF4;
    int v1 = *(int *)pool + 996;
    unsigned short tB6, t8214;
    unsigned short *tab;
    unsigned int bits;
    int cnt;
    *(unsigned short *)0x4808010 = 4096;
    if (*(unsigned short *)(v1 + 60) != 0) {
        if ((*(unsigned short *)((char *)*(int *)pool + 1564) & 0x10) != 0
            && a1 != 0
            && ((tB6 = *(unsigned short *)0x48080B6, t8214 = *(unsigned short *)0x4808214), (t8214 == 3 || t8214 == 5))
            && tB6 == 0) {
            tab = *(unsigned short **)(v1 + 68);
            bits = tab[1];
            cnt = 0;
            while (bits != 0) {
                cnt += bits & 1;
                bits = (bits << 15) >> 16;
            }
            fn_023954E8((unsigned short)(cnt * (tab[18] + 10) + 192 + 4 * tab[5]), 0x027CA600, tab, cnt);
            ++*(unsigned short *)((char *)*(int **)dword_380FFF4 + 952);
        } else {
            fn_0239477C(0, 16);
        }
    }
}
