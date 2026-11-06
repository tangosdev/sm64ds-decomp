/* NitroSDK wm_standard.c WM_Reset: drop any parent or child connection
 * and return to IDLE. Refuse unless WM is initialized and past READY and
 * STOP (WMi_CheckIdle), register callback for API 1 (RESET) and send that
 * command with no parameters; WM_ERRCODE_OPERATING (2) once it is queued. */
#include "private/wm_views.h"

extern int WMi_CheckIdle(void);

int WM_Reset(void *callback) {
    int result;

    result = WMi_CheckIdle();
    if (result != 0)
        return result;
    WM_SetCallbackTable(1, callback);
    result = WM_SendCommand(1, 0);
    if (result != 0)
        return result;
    return 2;
}
