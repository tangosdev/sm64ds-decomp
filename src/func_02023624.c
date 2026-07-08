extern void* _ZN9ActorBasenwEj(unsigned int size);
extern void _ZN9ActorBaseC1Ev(void* self);
extern int _ZTV12ActorDerived[];
extern int _ZTV5Scene[];
extern int _ZTV9BootScene[];

void* func_02023624(void)
{
    char* self = (char*)_ZN9ActorBasenwEj(0x58);
    if (self != 0) {
        _ZN9ActorBaseC1Ev(self);
        *(int**)self = _ZTV12ActorDerived;
        *(int**)self = _ZTV5Scene;
        {
            unsigned char* f = (unsigned char*)(((long long)(int)(self + 0x13)) & 0xFFFFFFFFFFFFFFFFLL);
            *f |= 1;
            *f |= 4;
        }
        *(int**)self = _ZTV9BootScene;
    }
    return self;
}
