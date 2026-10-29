//cpp
#include "types.h"
#include "private/arm9_wbt.h"
#include "IRQ.h"

extern "C" {

extern int data_020a94c0;
extern void func_02065a84(int x);
extern void func_02065ad0(int v);
extern char *func_02065b94(void);
extern int func_02065af0(int *thiz);
extern void func_02065be0(void);
extern void func_02065ba0(int v);
extern void func_02065bb0(int v);

/* NitroSDK WBT_InitParent(send_packet_size, recv_packet_size, callback): once
 * (data_020a94c0), store 0 through func_02065a84 (a child stores -1), reset
 * the running and system commands of the WBTWork that func_02065b94 returns,
 * make callback the system command's callback, forget each peer's last
 * command counter, queue an empty command (func_02065af0 copies its 0x9c
 * bytes into queued_cmd), and set the two packet sizes less the 0xe-byte
 * header (func_02065ba0, func_02065bb0). func_02040c34 passes the file-share
 * packet sizes and func_020412f0, the file-share parent's callback. */
void func_020652fc(int send_packet_size, int recv_packet_size, int callback) {
    unsigned int saved = IRQ::Disable();
    int i;
    char *p;
    WBTCommand cmd;

    if (data_020a94c0 == 1) {
        IRQ::Restore(saved);
        return;
    }
    data_020a94c0 = 1;
    func_02065a84(0);
    func_02065ad0(0);
    p = func_02065b94();
    ((WBTWork *)p)->cmd.command = 0;
    ((WBTWork *)p)->cmd.target_bmp = 0;
    ((WBTWork *)p)->cmd.peer_bmp = 0;
    ((WBTWork *)p)->cmd.callback = 0;
    ((WBTWork *)p)->system_cmd.command = 0;
    ((WBTWork *)p)->system_cmd.target_bmp = 0;
    ((WBTWork *)p)->system_cmd.peer_bmp = 0;
    ((WBTWork *)p)->system_cmd.callback = (void (*)(char *))callback;
    i = 0;
    do {
        /* p advances one WBTPeerRecord per pass: this walks peer[0..15]. */
        ((WBTWork *)p)->peer[0].last_cmd_counter = 0;
        i++;
        p += sizeof(WBTPeerRecord);
    } while (i < 0x10);
    cmd.command = 0;
    func_02065af0((int *)&cmd);
    func_02065be0();
    func_02065ba0(send_packet_size - 0xe);
    func_02065bb0(recv_packet_size - 0xe);
    IRQ::Restore(saved);
}
}
