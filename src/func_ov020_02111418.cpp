//cpp
extern "C" {
extern int func_ov020_021115ac(char *c);
extern void func_ov020_02112110(char *c);
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, void *v, unsigned int a, int fix, unsigned int b, unsigned int d, unsigned int e);
extern int _ZN6Player6BounceE5Fix12IiE(void *self, int fix);
int func_ov020_02111418(char *c) {
    int r = func_ov020_021115ac(c);
    if (r == 1) { func_ov020_02112110(c); return 1; }
    if (r == 2) {
        int v[3];
        v[0] = *(int*)(c+0x5c); v[1] = *(int*)(c+0x60); v[2] = *(int*)(c+0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(*(void**)(c+0x41c), v, 0, 0xc000, 1, 0, 1);
        func_ov020_02112110(c);
        return 1;
    }
    if (r == -1) {
        int eq = (*(unsigned short*)(c+0xc) == 0x147);
        if (eq) {
            int v[3];
            v[0] = *(int*)(c+0x5c); v[1] = *(int*)(c+0x60); v[2] = *(int*)(c+0x64);
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(*(void**)(c+0x41c), v, 2, 0xc000, 1, 0, 1);
        } else {
            int v[3];
            v[0] = *(int*)(c+0x5c); v[1] = *(int*)(c+0x60); v[2] = *(int*)(c+0x64);
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(*(void**)(c+0x41c), v, 1, 0xc000, 1, 0, 1);
        }
        *(char*)(c+0x108) = 0;
        func_ov020_02112110(c);
        return 1;
    }
    if (r != -2) return 0;
    _ZN6Player6BounceE5Fix12IiE(*(void**)(c+0x41c), 0x28000);
    func_ov020_02112110(c);
    return 1;
}
}
