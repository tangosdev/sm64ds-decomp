//cpp
#include "types.h"
#include "IRQ.h"
#include "private/wbt_block_info_list.h"

extern "C" {

extern int func_02065ae0(void);
extern void func_02065ad0(int v);
struct WBTBlockInfoList *func_020657fc(int *key) {
    u32 saved = (0, IRQ::Disable());
    struct WBTBlockInfoList *head = (struct WBTBlockInfoList *)func_02065ae0();
    struct WBTBlockInfoList *cur;
    struct WBTBlockInfoList *prev;
    if (head == 0) {
        IRQ::Restore(saved);
        return 0;
    }
    if (head->block_id == (int)key) {
        func_02065ad0((int)head->next);
        IRQ::Restore(saved);
        return head;
    }
    prev = head;
    cur = prev->next;
    while (cur != 0) {
        if (cur->block_id == (int)key) {
            prev->next = cur->next;
            IRQ::Restore(saved);
            return cur;
        }
        prev = cur;
        cur = cur->next;
    }

    IRQ::Restore(saved);
    return 0;
}
}
