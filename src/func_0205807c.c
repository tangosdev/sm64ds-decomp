typedef unsigned int u32;
typedef unsigned short u16;

struct Node {
    char pad[0x64];
    u32 unk64;
    struct Node *unk68;
    u32 unk6c;
};

struct Manager {
    char padc[0xc];
    struct Node *first;
};

extern struct Manager gManager_0x020a6134;

u32 _ZN3IRQ7DisableEv(void);
void _ZN3IRQ7RestoreEj(u32 state);
void func_02057f54(void);

void func_0205807c(u16 *self) {
    u32 r4 = _ZN3IRQ7DisableEv();
    u32 mask = *self;
    if (mask != 0) {
        struct Node *node = gManager_0x020a6134.first;
        if (node != 0) {
            u32 r1 = 1;
            do {
                if (mask & (r1 << node->unk6c)) {
                    node->unk64 = r1;
                }
                node = node->unk68;
            } while (node != 0);
        }
        *self = 0;
        func_02057f54();
    }
    _ZN3IRQ7RestoreEj(r4);
}
