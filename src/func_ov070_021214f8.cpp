//cpp
extern "C" void _ZN5Actor8PoofDustEv(void *c);
extern "C" void _ZN9ActorBase18MarkForDestructionEv(void *c);
extern "C" int func_ov070_021214f8(char *c)
{
    int flags;
    int b;
    flags = *(int*)(c + 0xb0);
    b = (flags & 0x20000) != 0;
    if (!b) {
        b = (flags & 0x40000) != 0;
        if (!b) {
            _ZN5Actor8PoofDustEv(c);
            _ZN9ActorBase18MarkForDestructionEv(c);
        }
    }
    return 1;
}
