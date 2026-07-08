// NONMATCHING: base materialization / addressing (div=12). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern int _ZN9ActorBasenwEj(unsigned int);
extern int _ZN9ActorBaseC1Ev(void *);
extern int _ZN8Particle10SysTrackerC1Ev(void *);
extern int _ZN5ModelC1Ev(void *);
extern int _ZN12MeshColliderC1Ev(void *);
extern int _ZTV12ActorDerived[];
extern int _ZTV5Scene[];
extern int _ZTV5Stage[];

void *_ZN5StageC1Ev(void)
{
    char *p = (char*)_ZN9ActorBasenwEj(0x9c8);
    if (p) {
        unsigned char *flag = (unsigned char*)(p+0x13);
        _ZN9ActorBaseC1Ev(p);
        *(int*)(p) = (int)_ZTV12ActorDerived;
        *(int*)(p) = (int)_ZTV5Scene;
        *flag |= 1;
        *flag |= 4;
        *(int*)(p) = (int)_ZTV5Stage;
        _ZN8Particle10SysTrackerC1Ev(p+0x50);
        _ZN5ModelC1Ev(p+0x86c);
        _ZN12MeshColliderC1Ev(p+0x91c);
    }
    return p;
}
