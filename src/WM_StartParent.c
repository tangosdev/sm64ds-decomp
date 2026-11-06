/* NitroSDK wm_standard.c WM_StartParent: IDLE to PARENT, with the parameters
 * WM_SetParentParameter set. Refuse unless WM is in state IDLE (2), register
 * callback for API 8 (START_PARENT) and send that command with no parameters;
 * WM_ERRCODE_OPERATING (2) once it is queued, the callback reporting the
 * result. */
#include "private/wm_views.h"

int WM_StartParent(void *callback) {
    int result;

    result = WM_CheckStateEx(1, 2);
    if (result != 0)
        return result;
    WM_SetCallbackTable(8, callback);
    result = WM_SendCommand(8, 0);
    if (result != 0)
        return result;
    return 2;
}
