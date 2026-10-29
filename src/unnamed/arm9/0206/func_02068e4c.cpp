//cpp
#include "types.h"
#include "WmWork.h"
#include "IRQ.h"

extern "C" {

extern void func_02059f2c(void *p);
extern void MultiStore_Int(int val, int *dst, int len);
extern void MultiStore16(int val, char *dst, int nbytes);
extern void CpuCopy8(u16 *a, char *b, int c);
extern char *data_020a9db8;
extern int data_020a9db4;
extern short data_020a9db0;
extern u16 *data_0209a084;

int func_02069038(void) {
    unsigned char buf[12];
    int i;
    int sum;
    func_02059f2c(buf);
    for (i = 0, sum = 0; i < 6; i++) {
        sum += buf[i];
    }
    {
        unsigned int n = (sum + *(int *)0x27ffc3c) * 7;
        return n % 20;
    }
}

int func_02068e4c(int a0, u16 *a1, int a2, int a3, u16 a4, int a5) {
    volatile unsigned short fill_s;
    volatile int fill_i;
    /* base1 (the WmWork) and base2 are byte pointers, not ints: every WmWork
     store below (parentMaxSize, sendBufSize, sendBuf, ...) is add-high /
     small-offset on the cartridge (add r0,r7,#0x1e00 / strh r3,[r0,#0x18]),
     which mwcc only emits for pointer arithmetic. With int bases each offset
     is materialized from the literal pool and stored through a register
     index, 15 words longer. */
    char *base1;
    char *base2;
    int irq;
    if ((data_020a9db8 != 0) && ((*((u16 *)(data_020a9db8 + 0x1316))) != 0)) {
        return 2;
    }
    base1 = (char *)((a0 + 0x1f) & (~0x1f));
    base2 = (char *)(((int)base1 + sizeof(WmWork) + 0x1f) & (~0x1f));
    irq = IRQ::Disable();
    fill_i = 0;
    data_020a9db0 = (short)a5;
    data_020a9db4 = (int)base1;
    data_020a9db8 = base2;
    MultiStore_Int(fill_i, (int *)base1, sizeof(WmWork));
    fill_s = 0;
    MultiStore16(fill_s, base2, 0x1340);
    /* 0x1e30 and 0x1e38 sit in WmWork.pad_1e2e: nothing else in this file
     reads them back (and the two copies overlap), so they stay raw */
    {
        int i = 0;
        u16 *dst = (u16 *)(base1 + 0x1e30);
        u16 *rp = a1;
        u16 v;
        do {
            v = *((rp + i) + 1);
            if (v == 0) {
                break;
            }
            i++;
            *(dst++) = v;
        } while (i < 0xa);
    }
    {
        u16 *dst = (u16 *)(base1 + 0x1e38);
        int j = 0;
        do {
            u16 *ptr = data_0209a084;
            if ((*ptr) == 0) {
                break;
            }
            data_0209a084 = ptr + 1;
            j++;
            *(dst++) = *ptr;
        } while (j < 0x10);
    }
    CpuCopy8(a1, base2 + 0x1300, 0x16);
    ((WmWork *)base1)->parentMaxSize = 0x100;
    ((WmWork *)base1)->childMaxSize = 8;
    ((WmWork *)base1)->sendBufSize = 0;
    ((WmWork *)base1)->recvBufSize = 0;
    ((WmWork *)base1)->mpContinuous = 1;
    ((WmWork *)base1)->sendBuf = base2;
    ((WmWork *)base1)->recvBuf = base2 + 0x400;
    {
        WmParentParam *hdr = &((WmWork *)base1)->parentParam;
        hdr->CS_Flag = 1;
        hdr->KS_Flag = 0;
        hdr->ggid[0] = (short)a2;
        hdr->ggid[1] = (short)a3;
        hdr->tgid = (short)a4;
        hdr->entryFlag = 1;
        hdr->multiBootFlag = 0;
        hdr->beaconPeriod = (u16)(func_02069038() + 0xc8);
        hdr->maxEntry = 0xf;
    }
    ((WmWork *)base1)->mpSending = 0;
    ((WmWork *)base1)->initialized = 0;
    *((int *)(base2 + 0x1324)) = 0;
    IRQ::Restore(irq);
    *((u16 *)(base2 + 0x1316)) = 1;
    return 0;
}
}
