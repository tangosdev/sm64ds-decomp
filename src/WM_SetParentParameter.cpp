//cpp
#include "types.h"
#include "WmWork.h"
#include "CP15.h"
#include "private/wm_views.h"

extern "C" {

extern int WmCheckParentParameter(void *o);

/* NitroSDK WM_SetParentParameter(callback, pparaBuf): refuse user game info
 * without a buffer and MP sizes that, with the key-sharing overhead, pass
 * 0x200 bytes; then flush the parameter block (and the game info) and send
 * the command. */
int WM_SetParentParameter(void *callback, WmParentParam *param) {
    int r;
    u16 ksFlag;
    r = WM_CheckStateEx(1, 2);
    if (r != 0) return r;
    if (param == 0)
        return 6;
    if (param->userGameInfoLength != 0) {
        if (param->userGameInfo == 0)
            return 6;
    }
    ksFlag = param->KS_Flag;
    if (param->parentMaxSize + (ksFlag != 0 ? 0x2a : 0) > 0x200 ||
        param->childMaxSize + (ksFlag != 0 ? 6 : 0) > 0x200)
        return 6;
    WmCheckParentParameter(param);
    WM_SetCallbackTable(7, callback);
    CP15::FlushDataCache((u32)param, sizeof(WmParentParam));
    if (param->userGameInfoLength != 0) {
        CP15::FlushDataCache((u32)param->userGameInfo, param->userGameInfoLength);
    }
    r = WM_SendCommand(7, 1, param);
    if (r == 0) return 2;
    return r;
}
}
