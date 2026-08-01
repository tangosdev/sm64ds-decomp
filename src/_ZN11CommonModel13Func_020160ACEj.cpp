//cpp
#include "types.h"
struct Holder {
    char _pad[0x24];
    u32 count;
};

struct Node {
    char _pad[0x24];
    u32 flags;
    char _pad2[0x30 - 0x28];
};

struct Sub {
    Holder *holder;
    Node *arr;
};

struct CommonModel {
    char _pad[8];
    Sub *sub;
    void Func_020160AC(u32 arg);
};

void CommonModel::Func_020160AC(u32 arg)
{
    Sub *sub = this->sub;
    Holder *h = sub->holder;
    u32 n = h->count;
    Node *p = sub->arr;
    u32 i;

    for (i = 0; i < n; i++) {
        u32 *f = (u32 *)(((long long)(int)((char *)p + 0x24)));
        *f |= arg;
        p++;
    }
}
