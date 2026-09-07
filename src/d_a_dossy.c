/* The factory remains a hand-written construction fold. mwccarm 2004/b56
 * rejects every source spelling of placement new/operator new available to
 * this tree (notes/ctor-migration.md 5d), so pretending this is a C++ factory
 * would only preserve the extern-C shell. The constructed member sequence is
 * nevertheless the exact Dorrie layout declared in include/Dorrie.h. */
extern void* _ZN7fBase_cnwEj(unsigned int);
extern void _ZN8dActor_cC2Ev(void*);
extern void _ZN9ModelAnimC1Ev(void*);
extern void _ZN10dBgCh_ActrC1Ev(void*);
extern void _ZN7dCcAc_cC1Ev(void*);
extern void _ZN10dCcAcPos_cC1Ev(void*);
extern void func_020733a8(void* arr, int count, int size, void(*ctor)(void*), void(*dtor)(void*));
extern void _ZN14DorriePlatformD1Ev(void*);
extern void _ZN14DorriePlatformC1Ev(void*);
extern void* _ZTV6Dorrie[];

/* Reconstructed source-style name: SM64DS proves daDossy_c through RTTI,
 * allocation size, vtable identity, and the DOSSY registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Dorrie_Spawn. */
int* daDossy_c_classInit(void)
{
    int* p = (int*)_ZN7fBase_cnwEj(0x11b8);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        *(void***)p = (void**)_ZTV6Dorrie;
        _ZN9ModelAnimC1Ev((char*)p + 0xec);
        func_020733a8((char*)p + 0x150, 7, 0x200,
                      _ZN14DorriePlatformC1Ev, _ZN14DorriePlatformD1Ev);
        _ZN10dBgCh_ActrC1Ev((char*)p + 0xf50);
        _ZN7dCcAc_cC1Ev((char*)p + 0x110c);
        _ZN10dCcAcPos_cC1Ev((char*)p + 0x1140);
    }
    return p;
}
