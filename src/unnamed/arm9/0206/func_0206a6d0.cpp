//cpp
#include "CP15.h"
#include "IRQ.h"

extern "C" {

typedef unsigned short u16;
typedef unsigned char u8;

extern void func_0206a3a4(int arg0, int *out);
extern void func_0206a458(int *out);
extern void DMASyncHalfTransfer(unsigned int channel, void *src, void *dst, unsigned int numHalfs);
extern void func_0206a424(int *self);
extern void func_0206a37c(int a, int *p);
extern void MultiCopy_Int(int *dst, int *src, int len);
extern void func_0206a318(void *arg);
extern void WaitByLoop(int n);

/* NitroSDK CTRDGHeader: the GBA cartridge header at 0x08000000. data_020a9e00
 * is a 0xc0-byte copy of it; the DMA below fills its last 0x40 bytes (+0x80)
 * from the cartridge, and the BIOS logo goes into nintendoLogo
 * (data_020a9e04). */
typedef struct {
    unsigned int startAddress; /* 0x00 */
    u8 nintendoLogo[0x9c];     /* 0x04 */
    char titleName[12];        /* 0xa0 */
    unsigned int gameCode;     /* 0xac */
    u16 makerCode;             /* 0xb0 */
    u8 isRomCode;              /* 0xb2 */
    u8 machineCode;            /* 0xb3 */
    u8 deviceType;             /* 0xb4 */
    u8 exLsiID[3];             /* 0xb5 */
    u8 reserved_A[4];          /* 0xb8 */
    u8 softVersion;            /* 0xbc */
    u8 complement;             /* 0xbd */
    u16 moduleID;              /* 0xbe */
} CTRDGHeader;

/* NitroSDK CTRDGModuleInfo, at HW_CTRDG_MODULE_INFO_BUF (0x027ffc30): the
 * header fields this function copies out of the CTRDGHeader above. */
typedef struct {
    u16 moduleID;          /* 0x0 */
    u8 exLsiID[3];         /* 0x2 */
    u8 flags;              /* 0x5 -- isAgbCartridge, detectPullOut bits */
    u16 makerCode;         /* 0x6 */
    unsigned int gameCode; /* 0x8 */
} CTRDGModuleInfo;

/* The cartridge work at data_020a9de0: func_0206a5c0 clears all four bytes
 * and stores the lock id at +2; the PXI callback func_0206a694 sets the
 * first halfword to 1 when the ARM7 answers. */
typedef struct {
    volatile u16 mSubpAck; /* 0x0 */
    u16 mLockId;           /* 0x2 */
} CtrdgWork;

extern int data_020a9de4;
extern unsigned short data_020a9de0;
extern int data_020a9e00;
extern int data_020a9e04;

#define CTRDG_WORK (*(CtrdgWork *)&data_020a9de0)
#define CTRDG_MODULE_INFO ((volatile CTRDGModuleInfo *)0x27ffc30)
#define CTRDG_HEADER (*(CTRDGHeader *)&data_020a9e00)

#pragma opt_strength_reduction off

void func_0206a6d0(void)
{
    int sp[4];
    unsigned int irq;
    int flag;
    u16 save208;
    int i;
    void *arg;

    if (data_020a9de4 != 0)
        return;
    data_020a9de4 = 1;
    if ((*(volatile u16 *)0x4000300 & 1) == 0)
        return;
    irq = IRQ::SetIRQs(0x40000);
    save208 = *(volatile u16 *)0x4000208;
    *(volatile u16 *)0x4000208 = 1;
    func_0206a3a4(CTRDG_WORK.mLockId, sp);
    flag = ((int)*(volatile u16 *)0x4000204 & 0x8000) >> 15;
    func_0206a458(sp + 2);
    *(volatile u16 *)0x4000204 &= ~0x8000;
    DMASyncHalfTransfer(0, (void *)0x8000080, (void *)((char *)&CTRDG_HEADER + 0x80), 0x40);
    *(volatile u16 *)0x4000204 = (*(volatile u16 *)0x4000204 & ~0x8000) | (flag << 15);
    func_0206a424(sp + 2);
    func_0206a37c(CTRDG_WORK.mLockId, sp);
    CTRDG_MODULE_INFO->moduleID = CTRDG_HEADER.moduleID;
    for (i = 0; i < 3; i++)
        /* Indexed off a typed pointer, not `*(volatile u8 *)(0x27ffc32 + i)`. Under
           2004/b56 the integer form parks the whole address in the literal pool and
           stores through a register offset, while the ROM builds 0x27ff000 from two
           immediates and puts 0xc32 in the store's 12-bit displacement -- one more
           instruction and one FEWER literal. That single pool entry shifts every
           pc-relative load in the function by 4, which is the whole 52-word diff. */
        CTRDG_MODULE_INFO->exLsiID[i] = CTRDG_HEADER.exLsiID[i];
    {
        /* Through a local: writing these two through CTRDG_MODULE_INFO as
           well moves the bytes. */
        volatile CTRDGModuleInfo *info = CTRDG_MODULE_INFO;
        info->makerCode = CTRDG_HEADER.makerCode;
        info->gameCode = CTRDG_HEADER.gameCode;
    }
    MultiCopy_Int((int *)0xffff0020, &data_020a9e04, 0x9c);
    CP15::FlushAndInvalidateDataCache();
    arg = (void *)((((unsigned int)&data_020a9e00 + 0xfe000000) >> 5) << 6 | 1);
    func_0206a318(arg);
    if (CTRDG_WORK.mSubpAck != 1) {
        do {
            WaitByLoop(1);
        } while (CTRDG_WORK.mSubpAck != 1);
    }
    (void)*(volatile u16 *)0x4000208;
    *(volatile u16 *)0x4000208 = save208;
    IRQ::SetIRQs(irq);
}
}
