/* storage 0x023875FC runtime 0x037FF494 linked 0xD4 wram ARM. */

extern int dword_3808D80;
extern int dword_3808D84;
extern int dword_38087A0;
extern int fn_02387ACC(void);
extern void fn_023878E8(int masked);
extern void fn_02387BC4(int addr);
extern void fn_02387840(int a1, int cmd);
extern void fn_02387A30(int src, int mode, unsigned len, void (*step)(void));
extern void fn_02387998(void);
extern void fn_02387AE4(int a, int b);

int fn_023875FC(int a1, int a2, unsigned int a3) {
    unsigned int half = *(unsigned short *)(dword_3808D80 + 0xC);
    unsigned int chunk;
    int result = fn_02387ACC();
    unsigned int mask = half - 1;

    while (a3 != 0) {
        unsigned int masked = a1 & mask;
        chunk = half - masked;
        if (chunk > a3)
            chunk = a3;
        fn_023878E8(masked);
        fn_02387BC4(dword_3808D84 + 1 + chunk);
        fn_02387840(a1, 10);
        fn_02387A30(a2, 0, chunk, fn_02387998);
        fn_02387AE4(*(int *)(dword_3808D80 + 0x14), *(int *)(dword_3808D80 + 0x18));
        result = *(int *)dword_38087A0;
        if (result != 0)
            break;
        a2 += chunk;
        a1 += chunk;
        a3 -= chunk;
    }
    return result;
}
