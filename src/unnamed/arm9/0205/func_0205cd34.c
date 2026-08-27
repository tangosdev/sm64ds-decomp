extern void func_0205a588(void *p, int v, int n);

struct Obj98 {
    char pad[0xc];
    short hc;
    short he;
};

void func_0205cd34(struct Obj98 *o) {
    func_0205a588(o, 0, 0x50);
    o->hc = 0;
    o->he = 0;
}
