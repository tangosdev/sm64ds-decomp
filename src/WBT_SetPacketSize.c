#include "private/arm9_wbt.h"

extern WBTWork *func_02065b94(void);
extern void func_02065ba0(int v);
extern void func_02065bb0(int v);

int WBT_SetPacketSize(int a, int b) {
    int x = func_02065b94()->cmd.command;
    if (x == WBT_CMD_REQ_GET_BLOCK || x == WBT_CMD_REQ_GET_BLOCKINFO)
        return 0;
    func_02065ba0(a - 0xe);
    func_02065bb0(b - 0xe);
    return 1;
}
