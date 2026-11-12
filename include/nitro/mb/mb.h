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

/* The download-play parent's public interface (the SDK's mb_parent.c).
 * The SDK writes these typedefs untagged; they carry their own names as
 * tags here, because an anonymous type in a shared header takes an @NNN
 * number in every C++ includer. */

/* the parent's view of each child, as the state callback
 * reports it. */
typedef enum MBCommPState {
    MB_COMM_PSTATE_NONE,
    MB_COMM_PSTATE_INIT_COMPLETE,
    MB_COMM_PSTATE_CONNECTED,
    MB_COMM_PSTATE_DISCONNECTED,
    MB_COMM_PSTATE_KICKED,
    MB_COMM_PSTATE_REQ_ACCEPTED,
    MB_COMM_PSTATE_SEND_PROCEED,
    MB_COMM_PSTATE_SEND_COMPLETE,
    MB_COMM_PSTATE_BOOT_REQUEST,
    MB_COMM_PSTATE_BOOT_STARTABLE,
    MB_COMM_PSTATE_REQUESTED,
    MB_COMM_PSTATE_MEMBER_FULL,
    MB_COMM_PSTATE_END,
    MB_COMM_PSTATE_ERROR,
    MB_COMM_PSTATE_WAIT_TO_SEND
} MBCommPState;

/* the application's answers to MB_CommResponseRequest. */
typedef enum MBCommResponseRequestType {
    MB_COMM_RESPONSE_REQUEST_KICK,
    MB_COMM_RESPONSE_REQUEST_ACCEPT,
    MB_COMM_RESPONSE_REQUEST_DOWNLOAD,
    MB_COMM_RESPONSE_REQUEST_BOOT
} MBCommResponseRequestType;

/* the errors an MB_COMM_PSTATE_ERROR report carries. */
typedef enum MBErrCode {
    MB_ERRCODE_SUCCESS = 0,
    MB_ERRCODE_INVALID_PARAM,
    MB_ERRCODE_INVALID_STATE,
    MB_ERRCODE_INVALID_DLFILEINFO,
    MB_ERRCODE_INVALID_BLOCK_NO,
    MB_ERRCODE_INVALID_BLOCK_NUM,
    MB_ERRCODE_INVALID_FILE,
    MB_ERRCODE_INVALID_RECV_ADDR,
    MB_ERRCODE_WM_FAILURE,
    MB_ERRCODE_FATAL
} MBErrCode;

/* the argument of an MB_COMM_PSTATE_ERROR report. */
typedef struct MBErrorStatus {
    u16 errcode;
} MBErrorStatus;

/* The state callback MB_CommSetParentStateCallback stores: the child's
 * AID, its new MBCommPState, and that state's argument. */
typedef void (*MBCommPStateCallback)(u16, u32, void *);

#ifdef __cplusplus
extern "C" {
#endif

struct WMBssDesc;
struct MBUserInfo;

void MB_ReadMultiBootParentBssDesc(struct WMBssDesc *, u16, u16, int, int);

/* The SDK returns BOOL from the last two; this tree has no BOOL yet. */
void MB_CommSetParentStateCallback(MBCommPStateCallback);
int MB_CommGetParentState(u16);
const struct MBUserInfo *MB_CommGetChildUser(u16);
u8 MB_CommGetChildrenNumber(void);
int MB_CommIsBootable(u16);
int MB_CommResponseRequest(u16, MBCommResponseRequestType);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_MB_MB_H */
