//cpp
#include "types.h"
#include "private/arm9_wbt.h"
#include "IRQ.h"

extern "C" {

extern int data_020a94c0;
extern void func_02065a84(int x);
extern void func_02065ad0(int v);
extern void *func_02065b94(void);
extern int func_02065af0(int *thiz);
extern void func_02065be0(void);
extern void func_02065ba0(int v);
extern void func_02065bb0(int v);

/* NitroSDK WBT_InitChild(callback): as WBT_InitParent (func_020652fc), but
 * stores -1 through func_02065a84 and sets both packet sizes to 0. The
 * callback becomes the system command's callback; func_02040c34
 * passes func_02040f30, the file-share child's. */
void func_02065234(void (*callback)(void *)) {
    unsigned int saved = IRQ::Disable();
    int i;
    WBTWork *work;
    WBTCommand cmd;

    if (data_020a94c0 == 1) {
        IRQ::Restore(saved);
        return;
    }
    data_020a94c0 = 1;
    func_02065a84(-1);
    func_02065ad0(0);
    work = (WBTWork *)func_02065b94();
    work->cmd.command = 0;
    work->cmd.target_bmp = 0;
    work->cmd.peer_bmp = 0;
    work->cmd.callback = 0;
    work->system_cmd.command = 0;
    work->system_cmd.target_bmp = 0;
    work->system_cmd.peer_bmp = 0;
    work->system_cmd.callback = (void (*)(char *))callback;
    for (i = 0; i < 0x10; i++) {
        work->peer[i].last_cmd_counter = 0;
    }
    cmd.command = 0;
    func_02065af0((int *)&cmd);
    func_02065be0();
    func_02065ba0(0);
    func_02065bb0(0);
    IRQ::Restore(saved);
}
}
