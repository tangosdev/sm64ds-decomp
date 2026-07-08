//cpp
extern "C" {
extern void *_ZTV12ActorDerived;
extern void *_ZTV5Scene;
extern void *data_ov003_020b179c;
void *_ZN9ActorBasenwEj(unsigned int);
void _ZN9ActorBaseC1Ev(void *);
void *func_ov003_020b1118(void);
}

void *func_ov003_020b1118(void) {
    char *p = (char *)_ZN9ActorBasenwEj(0x98);
    if (p) {
        _ZN9ActorBaseC1Ev(p);
        *(void **)p = &_ZTV12ActorDerived;
        *(void **)p = &_ZTV5Scene;
        *(unsigned char *)(int)(((int)p + 0x13) & 0xFFFFFFFFFFFFFFFF) |= 1;
        *(unsigned char *)(int)(((int)p + 0x13) & 0xFFFFFFFFFFFFFFFF) |= 4;
        *(void **)p = &data_ov003_020b179c;
    }
    return p;
}
