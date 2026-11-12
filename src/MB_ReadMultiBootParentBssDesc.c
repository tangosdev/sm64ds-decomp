#include "types.h"
#include "nitro/mb/mb.h"
#include "nitro/wm/wm.h"
#include "private/wm_views.h"
#include "nitro/mi/memory.h"

/* On a download-play child, rebuild the parent's WMBssDesc from the
 * MBParentBssDesc the boot loader left in MBParam (nitro/mb/mb.h). This is
 * the SDK's MB_ReadMultiBootParentBssDesc, whose body ntrtwl's NitroSDK has
 * in libraries/mb/src/mb_child.c. The beacon's first MB_BSSDESC_SIZE bytes
 * come over as-is; the game info is then filled in from the arguments, with
 * the parent's ggid and tgid taken from the first six bytes of its SSID.
 * Field names follow the SDK's WMBssDesc/WMGameInfo; this SDK version writes
 * ggid as two halfwords, so the shared view splits it. */

void MB_ReadMultiBootParentBssDesc(WMBssDesc *p_desc, u16 parent_max_size, u16 child_max_size,
                                   int ks_flag, int cs_flag) {
    const MBParentBssDesc *parentInfo = MB_GetMultiBootParentBssDesc();
    int flags;
    MI_CpuCopy8(parentInfo, p_desc, sizeof(MBParentBssDesc));
    p_desc->gameInfoLength = 0x10;
    p_desc->gameInfo.ggid[0] = p_desc->ssid[0];
    p_desc->gameInfo.ggid[1] = p_desc->ssid[1];
    p_desc->gameInfo.tgid = p_desc->ssid[2];
    p_desc->gameInfo.userGameInfoLength = 0;
    flags = (ks_flag ? WM_ATTR_FLAG_KS : 0) | (cs_flag ? WM_ATTR_FLAG_CS : 0);
    p_desc->gameInfo.parentMaxSize = parent_max_size;
    p_desc->gameInfo.childMaxSize = child_max_size;
    flags |= WM_ATTR_FLAG_ENTRY;
    p_desc->gameInfo.attribute = (u8)flags;
}
