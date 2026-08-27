//cpp
extern "C" void _ZN8dActor_c8PoofDustEv(void *c);
extern "C" void _ZN7fBase_c18MarkForDestructionEv(void *c);
extern "C" int func_ov070_021214f8(char *c)
{
    int flags;
    int b;
    flags = *(int*)(c + 0xb0);
    b = (flags & 0x20000) != 0;
    if (!b) {
        b = (flags & 0x40000) != 0;
        if (!b) {
            _ZN8dActor_c8PoofDustEv(c);
            _ZN7fBase_c18MarkForDestructionEv(c);
        }
    }
    return 1;
}
