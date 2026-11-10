//cpp
#include "types.h"
#include "IRQ.h"
#include "private/wbt_block_info_list.h"

extern "C" {

extern struct WBTBlockInfoList *func_02065ae0(void);
extern void func_02065ad0(struct WBTBlockInfoList *list);
extern void CpuCopy8(const void *src, void *dst, unsigned int size);

int WBT_RegisterBlock(struct WBTBlockInfoList *node, int blockId, void *userId, int dataPtr,
                      int dataSize, u16 permissionBmp) {
    u32 irq = IRQ::Disable();
    if (func_02065ae0() == 0) {
        func_02065ad0(node);
    } else {
        struct WBTBlockInfoList *p = func_02065ae0();
        struct WBTBlockInfoList *nx = p->next;
        if (nx != 0) {
            do {
                p = nx;
                nx = nx->next;
            } while (nx != 0);
        }
        p->next = node;
    }
    if (dataPtr == 0) {
        node->block_type = 1;
    } else {
        node->data_ptr = (char *)dataPtr;
        node->block_type = 0;
    }
    node->next = 0;
    node->block_id = blockId;
    node->block_size = dataSize;
    node->permission_bmp = permissionBmp;
    CpuCopy8(userId, node->user_id, 0x20);
    IRQ::Restore(irq);
    return 1;
}
}
