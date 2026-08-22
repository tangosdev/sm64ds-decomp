// func_ov006_02106ca4 at 0x02106ca4 (ov006), 0x214 bytes.
// dScMgPanel_c: slot 6 of the Behavior's own table data_ov006_02142888 -- the
// top-level round-end state. Dispatches every panel through the sub-state table
// data_ov006_02142840 with the same open-coded ARM Itanium member-pointer
// decode func_ov006_02106bc0 uses.
struct Pmf { int off; int adj; };
typedef void (*PmfFn)(void *, int);

extern struct Pmf data_ov006_02142840[];
extern unsigned char data_020a0e40;
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern char *data_ov004_020beb68;

extern void func_ov006_021050bc(char *c);
extern void func_ov006_02104580(char *c);
extern void func_ov006_02104870(char *c);
extern void func_ov006_02104ea8(char *c);
extern void func_ov006_021068d8(char *c);
extern void func_ov004_020b0a54(int arg);
extern void func_ov004_020adb1c(int n);

void func_ov006_02106ca4(char *c) {
    int busy;
    int i;
    char *g;

    func_ov006_021050bc(c);
    busy = 0;
    for (i = 0; i < *(int *)(c + 0x4cb8); i++) {
        unsigned char *slot = (unsigned char *)((long long)(int)(c + i + 0x4efa));
        struct Pmf *e = &data_ov006_02142840[*slot];
        int adj = e->adj;
        char *thisp = c + (adj >> 1);
        PmfFn fn;
        if (adj & 1) {
            fn = *(PmfFn *)(*(char **)thisp + e->off);
        } else {
            fn = (PmfFn)e->off;
        }
        fn(thisp, i);
        if (*slot != 0)
            busy++;
    }
    if (busy != 0)
        return;
    if (*(unsigned short *)(c + 0x4ec0) != 0) {
        *(unsigned short *)(((long long)(int)(c + 0x4ec0))) -= 1;
        if (*(unsigned char *)(c + 0x4fe6) == 0) {
            int done = 0;
            if (data_020a0de8[data_020a0e40 * 4] != 0) {
                if (data_020a0de9[data_020a0e40 * 4] != 0)
                    done = 1;
            }
            if (done != 0)
                *(unsigned short *)(c + 0x4ec0) = 0;
        }
        if (*(short *)(c + 0x4ec0) > 0)
            return;
        *(unsigned short *)(c + 0x4ec0) = 0;
        if (*(unsigned char *)(c + 0x4fe6) != 0) {
            func_ov006_02104870(c);
            func_ov004_020b0a54(0);
            func_ov006_02104ea8(c);
            *(unsigned char *)(c + 0x4fe3) = 0;
            *(unsigned char *)(c + 0xc3) = 0;
            g = data_ov004_020beb68;
            if (g != 0) {
                if (*(int *)(g + 0xb4) < 9999)
                    *(int *)(((long long)(int)(g + 0xb4))) += 1;
                if (*(int *)(g + 0xb4) > *(int *)(g + 0xb8))
                    *(int *)(g + 0xb8) = *(int *)(g + 0xb4);
            }
            func_ov004_020adb1c(data_ov004_020beb68 != 0
                                    ? *(int *)(data_ov004_020beb68 + 0xb4)
                                    : 0);
        } else {
            *(unsigned short *)(c + 0x4ec0) = 0x10;
            *(int *)(c + 0x4ca8) = 7;
            func_ov006_021068d8(c);
        }
    } else {
        func_ov006_02104580(c);
        *(unsigned short *)(((long long)(int)(c + 0x4ec6))) += 1;
    }
}
