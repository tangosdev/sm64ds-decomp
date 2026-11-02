#include "private/wbt_block_info_list.h"

extern struct WBTBlockInfoList *func_02065ae0(void);

struct WBTBlockInfoList *func_02065a94(int key) {
    struct WBTBlockInfoList *p = func_02065ae0();
    while (p != 0) {
        if (p->block_id == key)
            return p;
        p = p->next;
    }
    return 0;
}
