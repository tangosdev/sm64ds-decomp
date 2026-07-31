typedef unsigned int u32;
typedef unsigned short u16;
extern u32 _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 s);
extern int func_02065ae0(void);
extern void func_02065ad0(int v);
extern void CpuCopy8(void *a, void *b, int n);

struct Node {
    int f0;        /* 0x0 */
    int f4;        /* 0x4 */
    char pad8[0x28 - 8];
    struct Node *next;  /* 0x28 */
    int f2c;       /* 0x2c */
    u16 f30;       /* 0x30 */
    u16 f32;       /* 0x32 */
};

int func_020658c0(struct Node *node, int a1, void *a2, int a3, int arg5, u16 arg6)
{
    u32 irq = _ZN3IRQ7DisableEv();
    if (func_02065ae0() == 0) {
        func_02065ad0((int)node);
    } else {
        struct Node *p = (struct Node *)func_02065ae0();
        struct Node *nx = p->next;
        if (nx != 0) {
            do {
                p = nx;
                nx = nx->next;
            } while (nx != 0);
        }
        p->next = node;
    }
    if (a3 == 0) {
        node->f32 = 1;
    } else {
        node->f2c = a3;
        node->f32 = 0;
    }
    node->next = 0;
    node->f0 = a1;
    node->f4 = arg5;
    node->f30 = arg6;
    CpuCopy8(a2, (char *)node + 8, 0x20);
    _ZN3IRQ7RestoreEj(irq);
    return 1;
}
