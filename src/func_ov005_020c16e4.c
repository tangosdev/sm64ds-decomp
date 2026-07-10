// NONMATCHING: tile MultiCopy/MultiStore layout; size 0x338 vs 0x33c (div=160)
typedef unsigned short u16;
typedef unsigned int u32;

extern int data_0208a170;
extern int data_ov005_020c2260[];
extern int data_ov005_020c2464[]; /* 9 ints, copied via LDM/STM */
extern int data_ov005_020c23a0[];
extern int data_ov005_020c2310[];

extern char* LoadFile(int id);
extern void Deallocate(void* p);
extern void DecompressLZ16(void* src, void* dst);
extern void* _ZN3G2S12GetBG2ScrPtrEv(void);
extern void* _ZN3G2S12GetBG3ScrPtrEv(void);
extern void* _ZN3G2S12GetBG0ScrPtrEv(void);
extern void* _ZN3G2S13GetBG0CharPtrEv(void);
extern void MultiStore16(u16 val, void* dst, int nbytes);
extern void MultiCopy_Int(void* dst, void* src, int nbytes);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
extern int func_ov005_020c00b4(void* self, int n);

void func_ov005_020c16e4(char* sl)
{
    char* f;
    int i;
    int base;
    int tbl[9];
    int sp0, sp4, sp8, spc, sp10, sp14, sp18, sp1c, sp20, sp24, sp28, sp2c;
    int row, col;
    int tile;
    int off;
    u16 tmp;
    int v64;
    int r, c;
    int val;
    void* scr;

    /* Load & decompress BG2 screen for current selection */
    f = LoadFile(data_ov005_020c2260[data_0208a170]);
    DecompressLZ16(f, _ZN3G2S12GetBG2ScrPtrEv());
    Deallocate(f);

    /* Load & decompress BG3 screen for selection+4 */
    f = LoadFile(data_ov005_020c2260[data_0208a170 + 4]);
    DecompressLZ16(f, _ZN3G2S12GetBG3ScrPtrEv());
    Deallocate(f);

    sp0 = 0;
    sp8 = 0;
    sp10 = 0;
    sp28 = 0;
    spc = 0;
    sp14 = 0;
    sp24 = 0;
    sp2c = 0;
    sp18 = 0xc0;
    sp1c = 0x200;
    sp20 = 0x20;

    base = data_0208a170;
    /* copy 9-int table from data_ov005_020c2464 */
    for (i = 0; i < 9; i++)
        tbl[i] = data_ov005_020c2464[i];

    do {
        if (func_ov005_020c00b4(sl, base + sp8) == 0) {
            /* clear tiles with MultiStore16 of 0x1f */
            tile = tbl[sp0];
            for (row = 0; row < 9; row++) {
                for (col = 0; col < 6; col++) {
                    scr = _ZN3G2S12GetBG0ScrPtrEv();
                    tmp = 0x1f;
                    MultiStore16(tmp, (char*)scr + (tile << 1), 2);
                    tile += 1;
                }
                tile += 0x1a;
            }
        } else {
            /* decompress char data for this slot */
            f = LoadFile(data_ov005_020c23a0[base + sp8]);
            DecompressLZ16(f, (char*)_ZN3G2S13GetBG0CharPtrEv() + 0x8000);
            Deallocate(f);

            /* copy rows via MultiCopy_Int */
            for (r = 0; r < 9; r++) {
                v64 = *(int*)(sl + 0x64);
                scr = _ZN3G2S13GetBG0CharPtrEv();
                /* complex offset from v64 */
                {
                    int q = v64 / 4;
                    int rem = v64 % 4;
                    int dest = (int)scr + 0x8000 + r * 0x400
                        + q * 0x2400 + rem * 0xc0;
                    MultiCopy_Int((void*)dest, (void*)((int)scr + 0x8000 + r * 0x400), sp18);
                }
            }

            f = LoadFile(data_ov005_020c2310[base + sp8]);
            _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)f, sp1c);
            _ZN3GXS10LoadBGPlttEPKvjj(f + (*(int*)(sl + 0x64) << 5), sp20, sp20);
            Deallocate(f);

            *(int*)(sl + 0x68 + sp0 * 4) = *(int*)(sl + 0x64);

            tile = tbl[sp0];
            sp4 = 0;
            for (row = 0; row < 9; row++) {
                for (col = 0; col < 6; col++) {
                    v64 = *(int*)(sl + 0x64);
                    scr = _ZN3G2S12GetBG0ScrPtrEv();
                    {
                        int q = v64 / 4;
                        int rem = v64 % 4;
                        val = sp4 * 0x20 + rem * 6 + q * 0x120 + (v64 << 12);
                        tmp = (u16)(val + col);
                        MultiStore16(tmp, (char*)scr + (tile << 1), 2);
                    }
                    tile += 1;
                }
                tile += 0x1a;
                sp4 += 1;
            }

            {
                int* p = (int*)(sl + 0x64);
                *p = *p + 1;
                if (*p >= 0xa)
                    *p = sp2c;
            }
        }
        sp8 += 4;
        sp0 += 1;
    } while (sp0 < 9);
}
