#include "private/wm_views.h"

extern WMDataSharingInfo *data_020a0f74;
extern WMDataSet *data_020a0f80;

/* Step the data sharing: WM_StepDataSharing sends sendData and receives into
   data_020a0f80.
   On success store the received set's receivedBitmap in *out and return 1;
   else 0. */
int func_020406b4(unsigned char *sendData, unsigned short *out) {
    if (WM_StepDataSharing(data_020a0f74, sendData, data_020a0f80) != 0)
        return 0;
    *out = data_020a0f80->receivedBitmap;
    return 1;
}
