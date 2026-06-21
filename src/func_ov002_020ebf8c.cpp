//cpp
extern "C" {
extern void *data_ov002_0210ac00;
extern void _ZN5ModelD1Ev(void *self);
extern void func_0207328c(void *p, int a, int b, void *fn);
extern void _ZN5ActorD1Ev(void *self);
void *func_ov002_020ebf8c(char *r0) {
    char *r4 = r0;
    *(void**)r4 = &data_ov002_0210ac00;
    func_0207328c(r4 + 0xd4, 5, 0x50, (void*)&_ZN5ModelD1Ev);
    _ZN5ActorD1Ev(r4);
    return r4;
}
}
