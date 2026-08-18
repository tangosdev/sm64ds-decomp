extern char *_ZN8dActor_c13ClosestPlayerEv(char *self);
extern int _ZN6Player15IsEnteringLevelEv(char *self);
extern char *func_ov010_0211139c(char *c);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int a, void *v);
void func_ov010_02111320(char *c) {
    char *p;
    char *q;
    p = _ZN8dActor_c13ClosestPlayerEv(c);
    if (p == 0) return;
    if (_ZN6Player15IsEnteringLevelEv(p) != 0) {
        *(int*)(c + 0x3a0) = 4;
        return;
    }
    *(short*)(c + 0x300 + 0xa8) = 0x400;
    q = func_ov010_0211139c(c);
    if (q == 0) return;
    if (*(unsigned char*)(q + 0x3aa) == 0) return;
    *(int*)(c + 0x3a0) = 1;
    _ZN5Sound9PlayBank3EjRK7Vector3(0xe, c + 0x74);
}
