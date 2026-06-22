typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern void IRQ_DisableIRQs(u32 mask);
extern int func_02053c10(int enable);
extern void IRQ_SetIRQHandler(u32 irq, void *handler);

struct Obj {
    char pad0[0x14];
    int f14;     /* 0x14 */
    int f18;     /* 0x18 */
    char pad1c[0x28-0x1c];
    u8 f28;      /* 0x28 */
};

void func_0202fb30(struct Obj *o)
{
    u16 ime = *(volatile u16 *)0x4000208;
    *(volatile u16 *)0x4000208 = 0;
    IRQ_DisableIRQs(2);
    func_02053c10(0);
    IRQ_SetIRQHandler(2, 0);
    if (ime != 0) {
        u16 dead = *(volatile u16 *)0x4000208;
        *(volatile u16 *)0x4000208 = 1;
        (void)dead;
    }
    *(volatile u32 *)0x4000000 &= ~0xe000;
    *(volatile u32 *)0x4001000 &= ~0xe000;
    if (o->f28 == 1) {
        o->f14 = o->f18;
        o->f28 = 0;
    }
}
