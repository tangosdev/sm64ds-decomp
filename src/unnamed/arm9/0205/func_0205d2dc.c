extern void CpuCopy8(void *p, int b, int c);

struct Obj99 {
    char pad[0x1c];
    char *f1c;
};

int func_0205d2dc(struct Obj99 *o, int a, int b, int c) {
    CpuCopy8(o->f1c + b, a, c);
    return 0;
}
