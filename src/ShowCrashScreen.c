typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

extern void *_ZN2G212GetBG1ScrPtrEv(void);
extern u64 func_020427c4(void);
extern void *func_0202e118(void);
extern int func_020441bc(int i);
extern int func_020233d4(unsigned int i);
extern int _ZN4Heap6IntactEv(void *heap);
extern int _ZN4Heap21MaxAllocationUnitSizeEv(void *heap);

extern void nds_printt(void *dst, const char *fmt, u32 v);
extern void nds_print(void *dst, const void *str);
extern void nds_printf(void *dst, const char *fmt, ...);

extern signed char data_0209f2f8;
extern u8 data_0209f264;
extern u8 data_0209f220;
extern signed char data_02092120;
extern int data_02099f24;
extern void *data_020a4b68;
extern u16 data_020a4b50;
extern u8 data_020a4b4c;
extern int data_0209d3bc;
extern int data_020a0f38;
extern int *data_0209cddc;
extern int data_0209cde0;
extern int data_0209cdd8;
extern void *data_0209cdd4;
extern void *data_020a0e9c;
extern void *data_020a0eac;
extern u8 data_0209cdd0;

extern char data_0208e538[], data_0208e548[], data_0208e54c[], data_0208e55c[];
extern char data_0208e56c[], data_0208e57c[], data_0208e58c[], data_0208e59c[];
extern char data_0208e5b0[], data_0208e5c0[], data_0208e5d4[], data_0208e5e4[];
extern char data_0208e5f0[], data_0208e5fc[], data_0208e608[], data_0208e610[];
extern char data_0208e624[], data_0208e634[], data_0208e648[], data_020a0d90[];

typedef struct { char c[52]; } Buf52;
extern Buf52 data_0208e504;

void ShowCrashScreen(void)
{
    char *scr = (char *)_ZN2G212GetBG1ScrPtrEv();
    int hp;
    int rr;
    int cc;

    nds_printt(scr + 0x5e8, data_0208e538, (u32)func_020427c4());
    nds_print(scr, data_0208e548);
    nds_print(scr + 6, data_020a0d90);
    cc = data_0209f2f8;
    nds_printf(scr + 0x40, data_0208e54c, cc, func_0202e118());
    nds_printf(scr + 0x80, data_0208e55c, data_0209f264);
    nds_printf(scr + 0xc0, data_0208e56c, data_0209f220);
    nds_printf(scr + 0x100, data_0208e57c, data_02092120);

    rr = data_02099f24;
    if (rr != 0 && rr < 6)
        nds_printf(scr + 0x180, data_0208e58c, rr, func_020441bc(rr));

    hp = (data_020a4b68 != 0)
        ? *(u16 *)((char *)(*(void **)((char *)data_020a4b68 + 8)) + 0xc)
        : *(u16 *)&data_020a4b50;
    if (hp != 0xffff) {
        int bb = data_020a4b4c;
        nds_printf(scr + 0x1c0, data_0208e59c, hp, bb, func_020233d4(hp));
    }

    if (data_0209d3bc != 0)
        nds_printf(scr + 0x200, data_0208e5b0, data_0209d3bc);

    {
        u32 dispb = *(volatile u32 *)0x4000214;
        u32 dispa = *(volatile u32 *)0x4000210;
        nds_printf(scr + 0x280, data_0208e5c0, dispa, dispb);
    }

    if (data_020a0f38 != 0)
        nds_printf(scr + 0x2c0, data_0208e5d4, data_020a0f38);

    {
        int *regs = data_0209cddc;
        if (regs != 0) {
            Buf52 buf = data_0208e504;
            int i;
            int *tail;
            { i = 0; if (i < 17) do { nds_printf(scr + (i + 3) * 0x40 + 0x28, data_0208e5e4,
                    &buf.c[i * 3], regs[i]); i++; } while (i < 17); }
            tail = (int *)(int)(((long long)(int)(data_0209cddc + 0x19)) & 0xFFFFFFFFFFFFFFFFLL);
            nds_printf(scr + 0x528, data_0208e5f0, tail[0]);
            nds_printf(scr + 0x568, data_0208e5fc, tail[1]);
        }
    }

    if (data_0209cde0 != 0) {
        nds_printf(scr + 0x3c0, data_0208e608, data_0209cde0, data_0209cdd8);
        nds_print(scr + 0x400, (const void *)data_0209cdd4);
    }

    if (_ZN4Heap6IntactEv(data_020a0e9c)) {
        int mx = _ZN4Heap21MaxAllocationUnitSizeEv(data_020a0e9c);
        nds_printf(scr + 0x500, data_0208e610,
            (*(int *)((char *)data_020a0e9c + 8) - mx) / 1024, mx / 1024);
    } else {
        nds_print(scr + 0x500, data_0208e624);
    }

    if (_ZN4Heap6IntactEv(data_020a0eac)) {
        int mx = _ZN4Heap21MaxAllocationUnitSizeEv(data_020a0eac);
        nds_printf(scr + 0x540, data_0208e634,
            (*(int *)((char *)data_020a0eac + 8) - mx) / 1024, mx / 1024);
    } else {
        nds_print(scr + 0x500, data_0208e648);
    }

    data_0209cdd0 = 1;
}