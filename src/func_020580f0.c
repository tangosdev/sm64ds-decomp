typedef unsigned int u32;
typedef unsigned short u16;

struct Node {
    char pad[0x64];
    u32 unk64;
    struct Node *unk68;
    u32 unk6c;
};

struct NodeMgr {
    struct Node *node;
};

extern struct NodeMgr *gNodeMgrP_020a612c;

u32 _ZN3IRQ7DisableEv(void);
void _ZN3IRQ7RestoreEj(u32 state);
void func_02057f54(void);

void func_020580f0(u16 *self) {
    u32 r4 = _ZN3IRQ7DisableEv();
    struct Node *r2 = gNodeMgrP_020a612c->node;
    if (self != 0) {
        u32 r0 = 1 << r2->unk6c;
        u16 r1 = *self;
        *self = r1 | (u16)r0;
    }
    r2->unk64 = 0;
    func_02057f54();
    _ZN3IRQ7RestoreEj(r4);
}
