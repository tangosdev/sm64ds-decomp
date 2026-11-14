#ifndef NITRO_CTRDG_COMMON_H
#define NITRO_CTRDG_COMMON_H

/*
 * The cartridge-slot common routines, under the NitroSDK's own names. The
 * bodies are ctrdg_common.c, in src/engine/ctrdg/CTRDG_Common.cpp.
 *
 * The SDK's CTRDGLockByProc {locked, irq} and CTRDGRomCycle {c1, c2} records
 * are two words each, which every caller keeps in an int array, so they are
 * int * here.
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

#include "types.h"
#include "nitro/hw/mmap_global.h"
#include "nitro/hw/mmap_shared.h"

#ifdef __cplusplus
extern "C" {
#endif

/* NitroSDK CTRDGHeader: the GBA cartridge header at HW_CTRDG_ROM (0x08000000). */
typedef struct CTRDGHeader {
    u32 startAddress;      /* 0x00 */
    u8 nintendoLogo[0x9c]; /* 0x04 */
    char titleName[12];    /* 0xa0 */
    u32 gameCode;          /* 0xac */
    u16 makerCode;         /* 0xb0 */
    u8 isRomCode;          /* 0xb2 */
    u8 machineCode;        /* 0xb3 */
    u8 deviceType;         /* 0xb4 */
    u8 exLsiID[3];         /* 0xb5 */
    u8 reserved_A[4];      /* 0xb8 */
    u8 softVersion;        /* 0xbc */
    u8 complement;         /* 0xbd */
    u16 moduleID;          /* 0xbe */
} CTRDGHeader;

/* NitroSDK CTRDGModuleInfo, at HW_CTRDG_MODULE_INFO_BUF (0x027ffc30): the
 * header fields CTRDG_Init's probe copies out. */
typedef struct CTRDGModuleInfo {
    u16 moduleID;          /* 0x0 */
    u8 exLsiID[3];         /* 0x2 */
    u8 isAgbCartridge : 1; /* 0x5 */
    u8 detectPullOut : 1;
    u8 : 6;
    u16 makerCode; /* 0x6 */
    u32 gameCode;  /* 0x8 */
} CTRDGModuleInfo;

#define CTRDG_IS_ROM_CODE 0x96

/* Where a cartridge that is not ROM code keeps its module ID, and the copy of
 * the module information the ARM7 shares in the system work area. */
#define CTRDGi_GetModuleIDImageAddr() ((u16 *)(HW_CTRDG_ROM + 0x0001fffe))
#define CTRDGi_GetModuleInfoAddr()    ((CTRDGModuleInfo *)HW_CTRDG_MODULE_INFO_BUF)

/* NitroSDK CTRDGWork (CTRDGi_Work, 0x020a9de0): CTRDGi_InitCommon clears
 * it and stores the lock ID; the PXI callback sets subpState to 1 when the
 * ARM7 answers. */
typedef struct CTRDGWork {
    volatile u16 subpState; /* 0x0 */
    u16 lockID;             /* 0x2 */
} CTRDGWork;

void CTRDGi_InitCommon(void);
int CTRDG_IsExisting(void);
void CTRDGi_ChangeLatestAccessCycle(int *);
void CTRDGi_RestoreAccessCycle(int *);
void CTRDGi_LockByProcessor(u16, int *);
void CTRDGi_UnlockByProcessor(u16, int *);
void CTRDGi_SendtoPxi(u32);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_CTRDG_COMMON_H */
