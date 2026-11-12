#include "nitro/wm/wm.h"

extern int Wireless_Reset(void);
extern int func_0203fa50(void);
extern void func_02040bb0(void);
extern int data_020a0f5c;
extern int data_020a0f94;

/* The WM_StartMP callback func_0203faa8 installs. On MP_START: in link
   mode 0 reset; as parent (3) or child (4) start data sharing
   (func_0203fa50), and if that fails clear the mode and reset, else copy
   the mode into the link status data_020a0f94 and go on (func_02040bb0).
   TIMEOUT, INVALID_POLLBITMAP and SEND_FAILED are let pass; any other
   error clears the mode and resets. */
void func_0203fdac(void *arg) {
    WMStartMPCallback *cb = (WMStartMPCallback *)arg;
    unsigned short errcode = cb->errcode;
    if (errcode == WM_ERRCODE_SUCCESS) {
        switch (cb->state) {
        case WM_STATECODE_MP_START:
            switch (data_020a0f5c) {
            case 0:
                Wireless_Reset();
                return;
            case 3:
            case 4:
                if (func_0203fa50() == 0) {
                    data_020a0f5c = 0;
                    Wireless_Reset();
                } else {
                    data_020a0f94 = data_020a0f5c;
                    func_02040bb0();
                }
                return;
            default:
                return;
            }
        case WM_STATECODE_MPEND_IND:
        case WM_STATECODE_MP_IND:
            return;
        default:
            return;
        }
    } else {
        switch (errcode) {
        case WM_ERRCODE_TIMEOUT:
        case WM_ERRCODE_INVALID_POLLBITMAP:
        case WM_ERRCODE_SEND_FAILED:
            return;
        default:
            data_020a0f5c = 0;
            Wireless_Reset();
            return;
        }
    }
}
