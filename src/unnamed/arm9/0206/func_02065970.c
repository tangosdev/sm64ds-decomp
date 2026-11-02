#include "private/wbt_block_info_list.h"

struct WBTBlockInfoList *func_02065ae0(void);

int func_02065970(void) {
    int n = 0;
    struct WBTBlockInfoList *p = func_02065ae0();
    if (p != 0) {
        do {
            p = p->next;
            n++;
        } while (p != 0);
    }
    return n;
}
