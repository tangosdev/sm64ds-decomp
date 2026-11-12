#include "nitro/wbt.h"

extern WBTBlockInfoList *func_02065ae0(void);
extern int func_02065bc0(void);

char *func_02064888(unsigned int id, int idx) {
    WBTBlockInfoList *node = func_02065ae0();

    if (id < 0x3e8) {
        unsigned int i = 0;
        for (;;) {
            if (i == id)
                return (char *)node + idx * func_02065bc0();
            i++;
            if (node != 0)
                node = node->next;
            else
                return 0;
        }
    }

    while (node != 0) {
        if (node->data_info.id == id) {
            char *base = node->data_ptr;
            return base + idx * func_02065bc0();
        }
        node = node->next;
    }

    return 0;
}
