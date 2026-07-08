//cpp
// func_02043fdc at 0x02043fdc
class Obj { public: int dummy; };
typedef void (Obj::*PMF)();

struct Node {
    int pad;        // 0x0
    Node *next;     // 0x4
    Obj *obj;       // 0x8
};

struct Thing {
    Node *head;     // 0x0
    int pad;        // 0x4
    PMF callback;   // 0x8
};

extern "C" Node *CURRENT_OBJECT_LIST_NODE;

extern "C" void *func_02043fdc(Thing *thiz)
{
    Node *node;
    if (thiz->callback == 0) return (void *)1;
    node = thiz->head;
    while (node != 0) {
        Node *next;
        CURRENT_OBJECT_LIST_NODE = node;
        next = node->next;
        (node->obj->*thiz->callback)();
        node = next;
    }
    CURRENT_OBJECT_LIST_NODE = node;
    return (void *)1;
}
