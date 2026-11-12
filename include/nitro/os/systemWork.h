#ifndef NITRO_OS_SYSTEMWORK_H
#define NITRO_OS_SYSTEMWORK_H

/* The 0x400-byte system work area at HW_MAIN_MEM_SYSTEM (0x027ffc00), which
 * the ARM9, the ARM7 and the boot code share, under NitroSDK's names
 * (nitro/os/common/systemWork.h). Each field sits at the HW_*_BUF address
 * nitro/hw/mmap_shared.h gives it. */

#include "types.h"
#include "nitro/hw/mmap_shared.h"
#include "nitro/os/thread.h"
#include "nitro/os/spinLock.h"

typedef struct {
    u8 bootCheckInfo[0x20];             /* 0x000 */
    u32 resetParameter;                 /* 0x020 */
    u8 padding5[0x8];                   /* 0x024 */
    u32 romBaseOffset;                  /* 0x02c */
    u8 cartridgeModuleInfo[12];         /* 0x030 */
    u32 vblankCount;                    /* 0x03c */
    u8 wmBootBuf[0x40];                 /* 0x040 */
    u8 nvramUserInfo[0x100];            /* 0x080 */
    u8 isd_reserved1[0x20];             /* 0x180 */
    u8 arenaInfo[0x48];                 /* 0x1a0 */
    u8 real_time_clock[8];              /* 0x1e8 */
    u32 dmaClearBuf[4];                 /* 0x1f0 */
    u8 rom_header[0x160];               /* 0x200 */
    u8 isd_reserved2[32];               /* 0x360 */
    u32 pxiSignalParam[2];              /* 0x380 */
    u32 pxiHandleChecker[2];            /* 0x388 */
    u32 mic_last_address;               /* 0x390 */
    u16 mic_sampling_data;              /* 0x394 */
    u16 wm_callback_control;            /* 0x396 */
    u16 wm_rssi_pool;                   /* 0x398 */
    u8 ctrdg_SetModuleInfoFlag;         /* 0x39a */
    u8 ctrdg_IsExisting;                /* 0x39b */
    u32 component_param;                /* 0x39c */
    OSThreadInfo *threadinfo_mainp;     /* 0x3a0 */
    OSThreadInfo *threadinfo_subp;      /* 0x3a4 */
    u16 button_XY;                      /* 0x3a8 */
    u8 touch_panel[4];                  /* 0x3aa */
    u16 autoloadSync;                   /* 0x3ae */
    u32 lockIDFlag_mainp[2];            /* 0x3b0 */
    u32 lockIDFlag_subp[2];             /* 0x3b8 */
    struct OSLockWord lock_VRAM_C;      /* 0x3c0 */
    struct OSLockWord lock_VRAM_D;      /* 0x3c8 */
    struct OSLockWord lock_WRAM_BLOCK0; /* 0x3d0 */
    struct OSLockWord lock_WRAM_BLOCK1; /* 0x3d8 */
    struct OSLockWord lock_CARD;        /* 0x3e0 */
    struct OSLockWord lock_CARTRIDGE;   /* 0x3e8 */
    struct OSLockWord lock_INIT;        /* 0x3f0 */
    u16 mmem_checker_mainp;             /* 0x3f8 */
    u16 mmem_checker_subp;              /* 0x3fa */
    u8 padding4[2];                     /* 0x3fc */
    u16 command_area;                   /* 0x3fe */
} OSSystemWork;

#define OS_GetSystemWork() ((OSSystemWork *)HW_MAIN_MEM_SYSTEM)

#endif /* NITRO_OS_SYSTEMWORK_H */
