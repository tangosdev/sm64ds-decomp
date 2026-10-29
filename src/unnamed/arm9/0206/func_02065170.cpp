//cpp
#include "private/arm9_wbt.h"
#include "IRQ.h"

extern "C" {

extern void func_02065a84(int x);
extern void func_02065ad0(int v);
extern char *func_02065b94(void);
extern int func_02065af0(int *thiz);
extern void func_02065be0(void);
extern void func_02065ba0(int v);
extern void func_02065bb0(int v);
extern int data_020a94c0;

/* NitroSDK WBT_End: the teardown twin of WBT_InitParent (func_020652fc) and
 * WBT_InitChild (func_02065234). Once initialized (data_020a94c0), clear the
 * flag, store -1 through func_02065a84 (the parent stores 0 there, a child -1),
 * reset the running and system commands of the WBTWork that func_02065b94
 * returns, forget each peer's last command counter, queue an empty command
 * (func_02065af0 copies its 0x9c bytes into queued_cmd) and set both packet
 * sizes (func_02065ba0, func_02065bb0) to -1. */
void func_02065170(void)
{
    int saved = IRQ::Disable();
    char *p;
    int i;
    WBTCommand cmd;
    if (data_020a94c0 == 0) {
        IRQ::Restore(saved);
        return;
    }
    data_020a94c0 = 0;
    func_02065a84(-1);
    func_02065ad0(0);
    p = func_02065b94();
    ((WBTWork *)p)->cmd.command = 0;
    ((WBTWork *)p)->cmd.target_bmp = 0;
    ((WBTWork *)p)->cmd.peer_bmp = 0;
    ((WBTWork *)p)->cmd.callback = 0;
    ((WBTWork *)p)->system_cmd.command = 0;
    ((WBTWork *)p)->system_cmd.target_bmp = 0;
    ((WBTWork *)p)->system_cmd.peer_bmp = 0;
    ((WBTWork *)p)->system_cmd.callback = 0;
    i = 0;
    do {
        /* p advances one WBTPeerRecord per pass: this walks peer[0..15]. */
        ((WBTWork *)p)->peer[0].last_cmd_counter = (unsigned char)0;
        i++;
        p += sizeof(WBTPeerRecord);
    } while (i < 0x10);
    cmd.command = 0;
    func_02065af0((int *)&cmd);
    func_02065be0();
    func_02065ba0(-1);
    func_02065bb0(-1);
    IRQ::Restore(saved);
}
}
