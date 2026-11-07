#pragma opt_propagation off
#pragma opt_common_subs off
#pragma scheduling off
extern int dword_380FFF4;
extern int fn_02380884(int a1);
extern void fn_023808C0(int a1);
int fn_02394704(int a1) {
    unsigned short *base;
    int tok;
    unsigned short cur;
    unsigned char *tbl;
    unsigned short z;
    int sh;
    unsigned short prev;
    base = (unsigned short *)dword_380FFF4;
    tok = fn_02380884(0x1000010);
    cur = base[a1];
    if (cur != 0xFFFF) {
        tbl = (unsigned char *)base + 0x90;
        z = 0;
        sh = cur << 3;
        *(unsigned short *)(tbl + (cur << 3) + 2) = z;
        prev = *(unsigned short *)(tbl + sh);
        if (prev == 0xFFFF) {
            base[a1] = 0xFFFF;
            *(unsigned short *)((unsigned char *)(base + a1) + 8) = 0xFFFF;
        } else {
            base[a1] = prev;
            *(unsigned short *)(tbl + sh) = 0xFFFF;
        }
    }
    fn_023808C0(tok);
    return cur;
}
