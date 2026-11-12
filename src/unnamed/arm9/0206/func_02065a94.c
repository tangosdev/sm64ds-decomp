#include "nitro/wbt.h"

extern WBTBlockInfoList *func_02065ae0(void);

WBTBlockInfoList *func_02065a94(int key) {
    WBTBlockInfoList *p = func_02065ae0();
    while (p != 0) {
        if (p->data_info.id == key)
            return p;
        p = p->next;
    }
    return 0;
}
