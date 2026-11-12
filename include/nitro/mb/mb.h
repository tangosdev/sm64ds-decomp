#ifndef NITRO_MB_MB_H
#define NITRO_MB_MB_H

/* The download-play boot parameters, under NitroSDK's names (nitro/mb/mb.h;
 * pret/pokediamond MB_mb.h has the same layout). A download-play child's
 * boot loader leaves an MBParam at HW_WM_BOOT_BUF: the boot type, then the
 * parent's beacon, which MB_ReadMultiBootParentBssDesc rebuilds into a
 * WMBssDesc.
 *
 * Only the files that read the parameters include this. Its inline bodies
 * would renumber the @NNN symbols of every C++ includer of a wider header. */

#include "types.h"
#include "nitro/hw/mmap_shared.h"

#define MB_TYPE_ILLEGAL   0
#define MB_TYPE_NORMAL    1
#define MB_TYPE_MULTIBOOT 2

typedef struct {
    u16 length;
    u16 rssi;
    u16 bssid[3];
    u16 ssidLength;
    u8 ssid[32];
    u16 capaInfo;
    struct {
        u16 basic;
        u16 support;
    } rateSet;
    u16 beaconPeriod;
    u16 dtimPeriod;
    u16 channel;
    u16 cfpPeriod;
    u16 cfpMaxDuration;
} MBParentBssDesc; /* 0x3c */

typedef struct {
    u16 boot_type;                   /* 0x00 */
    MBParentBssDesc parent_bss_desc; /* 0x02 */
} MBParam;

#define MB_BSSDESC_SIZE (sizeof(MBParentBssDesc))

static inline const MBParam *MB_GetMultiBootParam(void) {
    return (const MBParam *)HW_WM_BOOT_BUF;
}

static inline int MB_IsMultiBootChild(void) {
    return MB_GetMultiBootParam()->boot_type == MB_TYPE_MULTIBOOT;
}

static inline const MBParentBssDesc *MB_GetMultiBootParentBssDesc(void) {
    return MB_IsMultiBootChild() ? &MB_GetMultiBootParam()->parent_bss_desc : 0;
}

#ifdef __cplusplus
extern "C" {
#endif

struct WMBssDesc;

void MB_ReadMultiBootParentBssDesc(struct WMBssDesc *, u16, u16, int, int);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_MB_MB_H */
