#include "types.h"
#include "nitro/wm/wm.h"
extern void func_0203f650(int x);
extern void func_0203f8a4(void);
struct WMMeasureChannelCallback;
extern void func_0203f714(struct WMMeasureChannelCallback *s);
extern void func_0203f6a4(int v);
extern s16 data_020a0f18;
extern int data_020a0f90;

/* The WM callback for the channel measurement, by the API id of the
   result: WM_Initialize fails with -2 or starts measuring,
   WM_MeasureChannel goes to func_0203f714, and WM_End returns to idle,
   reports data_020a0f18 and drops the caller's callback. Anything else
   just reports. */
void func_0203f920(void *arg) {
    WMCallback *cb = (WMCallback *)arg;

    switch (cb->apiid) {
    case WM_APIID_INITIALIZE:
        if (cb->errcode != WM_ERRCODE_SUCCESS)
            func_0203f650(-2);
        else
            func_0203f8a4();
        break;
    case WM_APIID_MEASURE_CHANNEL:
        func_0203f714((struct WMMeasureChannelCallback *)arg);
        break;
    case WM_APIID_END:
        func_0203f6a4(3);
        func_0203f650(data_020a0f18);
        data_020a0f90 = 0;
        break;
    default:
        func_0203f650(data_020a0f18);
        break;
    }
}
