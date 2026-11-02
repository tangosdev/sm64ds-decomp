#include "private/fs_views.h"

typedef struct FSFile FSFile;

extern int FSi_SendCommand(FSFile *node, int a, int b, int c);

int func_0205d568(FSFile *node, int b, ...) {
    unsigned int *p;
    if (b == 0) {
        return 0;
    }
    node->arc = (FSArchive *)b;
    {
        int b2 = b;
        int c = *(int *)((char *)&b + 4);
        node->arg.writefile.src = (const void *)b2;
        node->arg.writefile.len_org = c;
        if (FSi_SendCommand(node, 6, c, b2) == 0) {
            return 0;
        }
    }
    p = &node->stat;
    *p |= 0x10;
    *p &= ~0x20;
    return 1;
}
