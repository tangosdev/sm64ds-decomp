#include "private/wm_views.h"

extern WMArm9Buf *data_020a89ac;

void WM_SetCallbackTable(int apiid, void *callback) {
    data_020a89ac->callbackTable[apiid] = callback;
}
