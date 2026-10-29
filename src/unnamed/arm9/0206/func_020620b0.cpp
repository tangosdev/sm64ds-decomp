//cpp
#include "types.h"
#include "WmWork.h"
#include "CP15.h"

extern "C" {

extern int WM_CheckStateEx(int a, int b);
extern void func_02062024(void *o);
extern void WM_SetCallbackTable(int i, int val);
extern int WM_SendCommand(int a, int b, void *c);

/* NitroSDK WM_SetParentParameter(callback, pparaBuf): refuse user game info
 * without a buffer and MP sizes that, with the key-sharing overhead, pass
 * 0x200 bytes; then flush the parameter block (and the game info) and send
 * the command. */
int func_020620b0(int self, char *o)
{
    WmParentParam *param = (WmParentParam *)o;
    int r;
    u16 ksFlag;
    r = WM_CheckStateEx(1, 2);
    if (r != 0) return r;
    if (o == 0) return 6;
    if (param->userGameInfoLength != 0) {
        if (param->userGameInfo == 0)
            return 6;
    }
    ksFlag = param->KS_Flag;
    if (param->parentMaxSize + (ksFlag != 0 ? 0x2a : 0) > 0x200 ||
        param->childMaxSize + (ksFlag != 0 ? 6 : 0) > 0x200)
        return 6;
    func_02062024(o);
    WM_SetCallbackTable(7, self);
    CP15::FlushDataCache((u32)o, sizeof(WmParentParam));
    if (param->userGameInfoLength != 0) {
        CP15::FlushDataCache((u32)param->userGameInfo, param->userGameInfoLength);
    }
    r = WM_SendCommand(7, 1, o);
    if (r == 0) return 2;
    return r;
}
}
