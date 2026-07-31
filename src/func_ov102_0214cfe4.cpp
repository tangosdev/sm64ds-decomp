//cpp
// @symbol func_ov102_0214cfe4
// @emits BobOmb_Kill
/* recovered: renamed to Class_Method */
/* daBmb_c::Kill - recovered from vtable slot identity */
extern "C" void _ZN5Actor8PoofDustEv(void *c);
extern "C" void _ZN9ActorBase18MarkForDestructionEv(void *c);
extern "C" int BobOmb_Kill(char *c)
{
    int flags;
    int b;
    flags = *(int*)(c + 0xb0);
    b = (flags & 0x100) != 0;
    if (!b) {
        _ZN5Actor8PoofDustEv(c);
        _ZN9ActorBase18MarkForDestructionEv(c);
    }
    return 1;
}
