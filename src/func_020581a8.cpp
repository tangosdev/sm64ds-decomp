//cpp
struct Obj581 {
    char pad[0x64];
    int field64;
    char pad2[0x6c - 0x68];
    int idx;
    char pad3[0x90 - 0x70];
    char sub90[4];
};
extern "C" void _ZN3IRQ7DisableEv(void);
extern void func_02058a44(struct Obj581 *o);
extern void func_02058488(int target);
extern void func_0205807c(void *p);
extern void func_02057f54(void);
extern struct Obj581 *data_020a612c;
extern int data_020a6148[];

extern "C" void func_020581a8(void);
void func_020581a8(void) {
    struct Obj581 *o;
    _ZN3IRQ7DisableEv();
    o = *(struct Obj581 **)data_020a612c;
    func_02058a44(o);
    func_02058488((int)o);
    data_020a6148[o->idx] = 0;
    o->field64 = 2;
    func_0205807c(o->sub90);
    func_02057f54();
}
