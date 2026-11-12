#include "types.h"
#include "nitro/wm/wm.h"

extern void func_02040820(void);
extern void func_02040790(void);
extern int data_020a0f5c;
extern int data_020a0f94;

/* The callback func_0203fcec hands WM_Disable. On success the link status data_020a0f94 drops to 0 (idle),
   and a pending target mode data_020a0f5c starts the link again: 3 as
   parent (func_02040820), 4 as child (func_02040790). An error sets the
   status to 1. */
void func_02040504(void *arg) {
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode == WM_ERRCODE_SUCCESS) {
        switch (data_020a0f5c) {
        case 0:
            data_020a0f94 = 0;
            break;
        case 3:
            data_020a0f94 = 0;
            func_02040820();
            break;
        case 4:
            data_020a0f94 = 0;
            func_02040790();
            break;
        }
    } else {
        data_020a0f94 = 1;
    }
}
