#pragma opt_propagation off

extern unsigned char unk_3807B04[];
extern int dword_38075C0;
extern void fn_023859F8(unsigned char *);
extern void fn_0238579C(unsigned char *);

int fn_023863F0(int a1) {
    unsigned char *e;
    int i;
    int v2 = 0;
    i = v2;
    do {
        e = &unk_3807B04[i * 36];
        if (unk_3807B04[i * 36] != 0) {
            if (a1 != 0 && e[1] == 0)
                fn_023859F8(e);
            fn_0238579C(e);
            if (e[0] != 0)
                v2 |= 1 << i;
        }
        ++i;
    } while (i < 16);
    int result = dword_38075C0;
    if (result != 0)
        *(int *)(result + 4) = v2;
    return result;
}
