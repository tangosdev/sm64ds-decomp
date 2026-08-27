//cpp
// func_020441cc at 0x020441cc
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
class Obj { public: int dummy; };
typedef void (Obj::*PMF)();

struct Node {
    char pad[0x10];
    Obj *obj;     // 0x10
};

struct Thing {
    Node *head;   // 0
    PMF callback; // 4
};

extern "C" void *func_0203b394(Node *n);

extern "C" void *func_020441cc(Thing *thiz)
{
    Node *node;
    if (thiz->callback == 0) return (void *)1;
    node = thiz->head;
    while (node) {
        Node *next = (Node *)func_0203b394(node);
        (node->obj->*thiz->callback)();
        node = next;
    }
    return (void *)1;
}
