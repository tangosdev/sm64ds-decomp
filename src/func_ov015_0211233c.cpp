//cpp
extern "C" {
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);
extern void _ZN5Actor10PoofDustAtERK7Vector3(void *self, void *v);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int, void *);
extern void _ZN9ActorBase18MarkForDestructionEv(void *);
struct Vector3 { int x, y, z; };
void func_ov015_0211233c(char *c)
{
    struct Vector3 v;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x48, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
    v.x = *(int *)(c + 0x5c);
    v.y = *(int *)(c + 0x60);
    v.z = *(int *)(c + 0x64);
    _ZN5Actor10PoofDustAtERK7Vector3(c, &v);
    _ZN5Sound9PlayBank3EjRK7Vector3(0x41, c + 0x74);
    _ZN9ActorBase18MarkForDestructionEv(c);
}
}
