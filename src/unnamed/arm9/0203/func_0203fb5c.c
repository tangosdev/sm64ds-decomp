#include "types.h"
#include "nitro/wm/wm_api.h"

struct WmStartConnectCallback;
extern void func_0203fec4(struct WmStartConnectCallback *cb);
extern WMBssDesc *data_020a0f54;

/* Connect to the chosen parent: WM_StartConnect on the
   WMBssDesc in data_020a0f54, no SSID, with func_0203fec4 as the callback.
   1 when the request is accepted (2). */
int func_0203fb5c(void) {
    return WM_StartConnect((WMCallbackFunc)func_0203fec4, data_020a0f54, 0) == WM_ERRCODE_OPERATING;
}
