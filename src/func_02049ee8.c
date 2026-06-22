typedef unsigned short u16;
typedef unsigned int u32;

extern void func_0204a5c8(void* o);

typedef struct Node {
    struct Node* next;     // 0
    char pad[0x18];
    u32 f1c;               // 0x1c
} Node;

typedef struct Obj {
    int f0;
    Node* f4;              // 0x4
    char pad[0x2c];
    Node* f34;             // 0x34
    int f38;               // 0x38
} Obj;

void func_02049ee8(Obj* o, int v)
{
    Node* n;
    u16 m = *(volatile u16*)0x4000060;
    *(volatile u16*)0x4000060 = (m & ~0x3000) | 8;
    o->f38 = v;
    n = o->f4;
    if (n == 0) return;
    do {
        o->f34 = n;
        if (((n->f1c << 0x1c) >> 0x1f) == 0) {
            func_0204a5c8(o);
        }
        n = n->next;
    } while (n != 0);
}
