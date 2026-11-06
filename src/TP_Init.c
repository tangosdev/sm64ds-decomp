#include "types.h"
#include "nitro/spi/tp.h"
extern void PXI_Init(void);
extern int PXI_IsCallbackReady(int tag, int proc);
extern void PXI_SetFifoRecvCallback(int tag, void *callback);
extern void TPi_TpCallback(int tag, unsigned int data, int err);

extern char data_020a80c8[];

void TP_Init(void) {
    if (*(u16 *)data_020a80c8 != 0)
        return;
    *(u16 *)data_020a80c8 = 1;
    PXI_Init();
    data_020a80cc.mState = 0;
    data_020a80cc.mCommandFlags = 0;
    data_020a80cc.mIndex = 0;
    data_020a80cc.mCallback = 0;
    data_020a80cc.mSamplingBufs = 0;
    data_020a80cc.mCalibrated = 0;
    data_020a80cc.mErrFlags = 0;
    while (PXI_IsCallbackReady(6, 1) == 0)
        ;
    PXI_SetFifoRecvCallback(6, (void *)TPi_TpCallback);
}
