#include "nitro/wbt.h"

WBTBlockInfoList *func_02065ae0(void);

int WBT_NumOfRegisteredBlock(void) {
    int n = 0;
    WBTBlockInfoList *p = func_02065ae0();
    if (p != 0) {
        do {
            p = p->next;
            n++;
        } while (p != 0);
    }
    return n;
}
