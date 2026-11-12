//cpp
#include "types.h"
#include "nitro/os/system.h"
#include "nitro/wbt.h"

extern "C" {

extern WBTBlockInfoList *func_02065ae0(void);
extern void func_02065ad0(WBTBlockInfoList *list);
WBTBlockInfoList *WBT_UnregisterBlock(u32 block_id) {
    OSIntrMode saved = (0, OS_DisableInterrupts());
    WBTBlockInfoList *head = func_02065ae0();
    WBTBlockInfoList *cur;
    WBTBlockInfoList *prev;
    if (head == 0) {
        OS_RestoreInterrupts(saved);
        return 0;
    }
    if (head->data_info.id == block_id) {
        func_02065ad0(head->next);
        OS_RestoreInterrupts(saved);
        return head;
    }
    prev = head;
    cur = prev->next;
    while (cur != 0) {
        if (cur->data_info.id == block_id) {
            prev->next = cur->next;
            OS_RestoreInterrupts(saved);
            return cur;
        }
        prev = cur;
        cur = cur->next;
    }

    OS_RestoreInterrupts(saved);
    return 0;
}
}
