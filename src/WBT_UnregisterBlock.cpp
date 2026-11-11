//cpp
#include "types.h"
#include "nitro/os/system.h"
#include "private/wbt_block_info_list.h"

extern "C" {

extern struct WBTBlockInfoList *func_02065ae0(void);
extern void func_02065ad0(struct WBTBlockInfoList *list);
struct WBTBlockInfoList *WBT_UnregisterBlock(int key) {
    OSIntrMode saved = (0, OS_DisableInterrupts());
    struct WBTBlockInfoList *head = func_02065ae0();
    struct WBTBlockInfoList *cur;
    struct WBTBlockInfoList *prev;
    if (head == 0) {
        OS_RestoreInterrupts(saved);
        return 0;
    }
    if (head->block_id == key) {
        func_02065ad0(head->next);
        OS_RestoreInterrupts(saved);
        return head;
    }
    prev = head;
    cur = prev->next;
    while (cur != 0) {
        if (cur->block_id == key) {
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
