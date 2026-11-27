#pragma opt_common_subs off
#pragma opt_propagation off
extern int dword_380FFF4;
extern int fn_02380884(int a1);
extern void fn_023808C0(int a1);
extern void fn_02381600(int a1, int a2, int a3);
void fn_0239477C(int a1, int a2) {
    unsigned short *base;
    unsigned short *rows;
    int tok;
    int off;
    unsigned char *ent;
    unsigned short prev;
    int sc;
    int zero;
    int obj;
    base = (unsigned short *)dword_380FFF4;
    rows = base + 72;
    tok = fn_02380884(0x1000010);
    off = a2 << 3;
    ent = (unsigned char *)rows + (a2 << 3);
    if (*(unsigned short *)(ent + 2) == 0) {
        *(unsigned short *)(ent + 2) = 1;
        *(unsigned short *)((unsigned char *)rows + off) = 0xFFFF;
        sc = a1 << 1;
        prev = *(unsigned short *)((unsigned char *)base + sc + 8);
        if (prev == 0xFFFF)
            *(unsigned short *)((unsigned char *)base + sc) = (unsigned short)a2;
        else
            *(unsigned short *)((unsigned char *)base + (prev << 3) + 0x90) = (unsigned short)a2;
        *(unsigned short *)((unsigned char *)(base + a1) + 8) = (unsigned short)a2;
        if (a1 < base[8])
            base[8] = (unsigned short)a1;
    }
    fn_023808C0(tok);
    if (a1 != 3 && base[9] == 3) {
        obj = *(int *)((char *)dword_380FFF4 + 0x2CC);
        zero = 0;
        fn_02381600(obj, zero, zero);
    }
}
