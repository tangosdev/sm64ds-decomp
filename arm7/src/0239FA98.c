extern int *dat_0380FFF4;
extern unsigned short unk_27D6A7C[];
extern int fn_02380884(int);
extern int fn_023808C0(int);

int fn_0239FA98(int a1)
{
    int v3;
    int v2 = 20 * a1;
    v3 = (int)dat_0380FFF4 + 0x3E4 + v2;
    int v4 = fn_02380884(0x1000000);
    *(unsigned short *)0x48080B4 = unk_27D6A7C[a1];
    if (((unsigned short *)((int)dat_0380FFF4 + v2 + 0x300))[0x72] != 0) {
        int v5 = *(int *)(v3 + 0xC);
        if ((*(unsigned short *)(v5 + 0x14) & 0x4000) == 0)
            *(unsigned short *)(v5 + 0xC) = *(unsigned short *)(*(int *)(v3 + 8) + 4);
    }
    return fn_023808C0(v4);
}
