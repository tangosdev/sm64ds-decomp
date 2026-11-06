/* NitroSDK wm_standard.c WM_Disable: STOP back to READY, telling ARM7 the
 * hardware is no longer in use. Refuse unless WM is in state STOP (1), register
 * callback for API 4 (DISABLE) and send that command with no parameters;
 * WM_ERRCODE_OPERATING (2) once it is queued, the callback reporting the
 * result. */
#include "private/wm_views.h"

int WM_Disable(void *callback) {
    int result;

    result = WM_CheckStateEx(1, 1);
    if (result != 0)
        return result;
    WM_SetCallbackTable(4, callback);
    result = WM_SendCommand(4, 0);
    if (result != 0)
        return result;
    return 2;
}
