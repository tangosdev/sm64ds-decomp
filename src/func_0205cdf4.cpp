//cpp
extern "C" {
extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int saved);
extern void func_0205c788(char *self, int arg);
extern void func_0205d8d8(void *node, void *list);
extern void func_0205cfa4(char *self);
extern void func_020580f0(char *self);
extern int func_0205cf5c(char *self);

#pragma opt_loop_invariants off
int func_0205cdf4(char *self, int arg1)
{
    char *list = *(char **)(self + 8);
    unsigned int mask = 1 << arg1;
    unsigned int saved;
    int t, u, v;

    *(int *)(self + 0x10) = arg1;
    *(int *)(self + 0x14) = 2;
    *(int *)(((long long)(int)(self + 0xc)) & 0xffffffffffffffffLL) |= 1;
    saved = _ZN3IRQ7DisableEv();
    if (*(int *)(list + 0x10) & 0x80) {
        func_0205c788(self, 3);
        _ZN3IRQ7RestoreEj(saved);
        return 0;
    }
    if (mask & 0x1fc) {
        *(int *)(((long long)(unsigned int)(self + 0xc)) & 0xffffffffffffffffLL) |= 4;
    }
    func_0205d8d8(self, list + 0x14);
    t = (*(int *)(list + 0x10) & 0x10) != 0;
    if (!t) {
        *(int *)(((long long)(int)(list + 0x10)) & 0xffffffffffffffffLL) |= 0x10;
        _ZN3IRQ7RestoreEj(saved);
        if (*(int *)(list + 0x4c) & 0x200) {
            (*(void (**)(char *, int))(list + 0x48))(self, 9);
        }
        v = (*(int *)(self + 0xc) & 4) != 0;
        if (v) goto tail;
        func_0205cfa4(self);
        return 1;
    }
    u = (*(int *)(self + 0xc) & 4) != 0;
    if (!u) {
        _ZN3IRQ7RestoreEj(saved);
        return 1;
    }
    do {
        func_020580f0(self + 0x18);
    } while ((int)self != *(int *)(list + 0x18));
    _ZN3IRQ7RestoreEj(saved);
tail:
    return func_0205cf5c(self);
}
}
