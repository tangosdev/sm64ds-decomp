//cpp
extern "C" {
int _ZN6Player9IsOnShellEv(void* p);
void _ZN8dActor_c8PoofDustEv(void* p);
void _ZN7fBase_c18MarkForDestructionEv(void* p);
}

extern "C" int func_ov102_0214d044(char* c)
{
    if (*(void**)(c + 0x3c0) != 0) {
        if (_ZN6Player9IsOnShellEv(*(void**)(c + 0x3c0)) != 0)
            goto copy;
    }
    _ZN8dActor_c8PoofDustEv(c);
    _ZN7fBase_c18MarkForDestructionEv(c);
    return 0;
copy:
    {
        void* base = *(void**)(c + 0x3c0);
        int* s = (int*)(((int)base + 0x5c));
        short* a = (short*)(((int)c + 0x8e));
        int v0 = s[0];
        int ret = 1;
        *(int*)(c + 0x5c) = v0;
        *(int*)(c + 0x60) = s[1];
        *(int*)(c + 0x64) = s[2];
        *a = *a + 0x1000;
        return ret;
    }
}
