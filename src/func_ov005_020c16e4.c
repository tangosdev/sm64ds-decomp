#include "types.h"
#include "math/Matrix.h"

extern int data_0208a170;
extern int data_ov005_020c2260[];
extern Matrix3x3 data_ov005_020c2464;
extern int data_ov005_020c23a0[];
extern int data_ov005_020c2310[];

extern char *LoadFile(int id);
extern void Deallocate(void *p);
extern void DecompressLZ16(void *src, void *dst);
extern void *_ZN3G2S12GetBG2ScrPtrEv(void);
extern void *_ZN3G2S12GetBG3ScrPtrEv(void);
extern void *_ZN3G2S12GetBG0ScrPtrEv(void);
extern void *_ZN3G2S13GetBG0CharPtrEv(void);
extern void MultiStore16(u16 val, void *dst, int nbytes);
extern void MultiCopy_Int(void *dst, void *src, int nbytes);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 destOff, u32 size);
extern int func_ov005_020c00b4(void *self, int n);

void func_ov005_020c16e4(char *self)
{
    int i;
    int sp4;
    int j;
    int n1;
    int n2;
    int m;
    int copyLen;
    int flushSize;
    int plttSize;
    int pInit;
    int qInit;
    int resetVal;
    volatile u16 valA;
    volatile u16 valB;
    int tbl[9];
    char *f;
    int fill;
    int nbytes;
    int t;

    f = LoadFile(data_ov005_020c2260[data_0208a170]);
    DecompressLZ16(f, _ZN3G2S12GetBG2ScrPtrEv());
    Deallocate(f);

    f = LoadFile(data_ov005_020c2260[data_0208a170 + 4]);
    DecompressLZ16(f, _ZN3G2S12GetBG3ScrPtrEv());
    Deallocate(f);

    i = 0;
    j = 0;
    n2 = 0;
    qInit = 0;
    n1 = 0;
    m = 0;
    pInit = 0;
    resetVal = 0;
    copyLen = 0xc0;
    flushSize = 0x200;
    t = 0x20;
    fill = 0x1f;
    nbytes = 2;
    plttSize = t;

    do {
        *(Matrix3x3 *)tbl = data_ov005_020c2464;
        if (func_ov005_020c00b4(self, data_0208a170 + j) == 0) {
            int off;
            int row;
            int col;
            int destOff;
            char *scr;

            off = tbl[i];
            row = n1;
            do {
                col = n2;
                destOff = off << 1;
                do {
                    char *dstp;
                    scr = (char *)_ZN3G2S12GetBG0ScrPtrEv();
                    dstp = scr + destOff;
                    dstp = destOff ? dstp : dstp;
                    valA = (u16)fill;
                    MultiStore16(valA, dstp, nbytes);
                    destOff += 2;
                    off += 1;
                    col += 1;
                } while (col < 6);
                off += 0x1a;
                row += 1;
            } while (row < 9);
        } else {
            int row;
            int destOff;
            int idx;
            char *ch;
            char *scr;
            int off;
            int q;
            int p;

            f = LoadFile(data_ov005_020c23a0[data_0208a170 + j]);
            DecompressLZ16(f, (char *)_ZN3G2S13GetBG0CharPtrEv() + 0x8000);
            Deallocate(f);

            row = m;
            destOff = row;
            do {
                idx = *(int *)(self + 0x64);
                ch = (char *)_ZN3G2S13GetBG0CharPtrEv();
                scr = ch + (idx / 4) * 0x2400 + (idx % 4) * 0xc0 + destOff;
                ch = (char *)_ZN3G2S13GetBG0CharPtrEv();
                MultiCopy_Int(ch + 0x8000 + destOff, scr, copyLen);
                destOff += 0x400;
            } while (++row < 9);

            f = LoadFile(data_ov005_020c2310[data_0208a170 + j]);
            _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)f, flushSize);
            _ZN3GXS10LoadBGPlttEPKvjj(f, *(int *)(self + 0x64) << 5, plttSize);
            Deallocate(f);

            ((int *)(self + 0x68))[i] = *(int *)(self + 0x64);

            off = tbl[i];
            sp4 = pInit;
            p = pInit;
            do {
                q = qInit;
                destOff = off << 1;
                do {
                    char *dstp;
                    idx = *(int *)(self + 0x64);
                    scr = (char *)_ZN3G2S12GetBG0ScrPtrEv();
                    dstp = scr + destOff;
                    dstp = idx ? dstp : dstp;
                    valB = (u16)(q + (p + ((idx % 4) * 6 + ((idx / 4) * 0x120 + (idx << 12)))));
                    MultiStore16(valB, dstp, nbytes);
                    destOff += 2;
                    off += 1;
                    q += 1;
                } while (q < 6);
                off += 0x1a;
                p += 0x20;
                sp4 += 1;
            } while (sp4 < 9);

            {
                int *slot = (int *)(self + 0x64);
                *slot = *slot + 1;
                if (*(int *)(self + 0x64) >= 10)
                    *(int *)(self + 0x64) = resetVal;
            }
        }
        j += 4;
        i += 1;
    } while (i < 9);
}
