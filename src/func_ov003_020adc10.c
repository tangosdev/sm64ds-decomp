//cpp
// NONMATCHING: base materialization / addressing (div=14). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
extern void *_ZTV12ActorDerived;
extern void *_ZTV5Scene;
extern void *data_ov003_020b1650;
void *_ZN9ActorBasenwEj(unsigned int);
void _ZN9ActorBaseC1Ev(void *);
void *func_ov003_020adc10(void);
}

void *func_ov003_020adc10(void) {
    char *p = (char *)_ZN9ActorBasenwEj(0x54);
    if (p) {
        _ZN9ActorBaseC1Ev(p);
        *(void **)p = _ZTV12ActorDerived;
        *(void **)p = _ZTV5Scene;
        *(unsigned char *)(p + 0x13) |= 1;
        *(unsigned char *)(p + 0x13) |= 4;
        *(void **)p = data_ov003_020b1650;
    }
    return p;
}
