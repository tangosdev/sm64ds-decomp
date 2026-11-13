//cpp
#include "types.h"
#include "nitro/os/system.h"
#include "nitro/wbt.h"
#include "nitro/mi/memory.h"

extern "C" {


extern WBTBlockInfoList *func_02065ae0(void);
extern void func_02065ad0(WBTBlockInfoList *list);
}


int WBT_RegisterBlock(WBTBlockInfoList *block_info_list, u32 block_id, const void *user_id,
                      const void *data_ptr, int data_size, u16 permission_bmp) {
    OSIntrMode irq = OS_DisableInterrupts();
    if (func_02065ae0() == 0) {
        func_02065ad0(block_info_list);
    } else {
        WBTBlockInfoList *p = func_02065ae0();
        WBTBlockInfoList *nx = p->next;
        if (nx != 0) {
            do {
                p = nx;
                nx = nx->next;
            } while (nx != 0);
        }
        p->next = block_info_list;
    }
    if (data_ptr == 0) {
        block_info_list->block_type = WBT_BLOCK_LIST_TYPE_USER;
    } else {
        block_info_list->data_ptr = (void *)data_ptr;
        block_info_list->block_type = WBT_BLOCK_LIST_TYPE_COMMON;
    }
    block_info_list->next = 0;
    block_info_list->data_info.id = block_id;
    block_info_list->data_info.block_size = data_size;
    block_info_list->permission_bmp = permission_bmp;
    MI_CpuCopy8(user_id, block_info_list->data_info.user_id, WBT_USER_ID_LEN);
    OS_RestoreInterrupts(irq);
    return 1;
}
