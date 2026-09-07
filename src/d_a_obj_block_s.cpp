//cpp
extern "C" {
void* _ZN7fBase_cnwEj(unsigned int sz);
void _ZN10dBgActor_cC2Ev(void* p);
void _ZN10dBgCh_ActrC1Ev(void* p);
void _ZN11ShadowModelC1Ev(void* p);
void _ZN10dCcAcPos_cC1Ev(void* p);
}
extern void* _ZTV5Crate;

/* Reconstructed source-style name: SM64DS proves daObjBlockS_c through RTTI,
 * allocation size, vtable identity, and the BLOCK_S registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Crate_Spawn. */
extern "C" void* daObjBlockS_c_classInit(void)
{
    char* p = (char*)_ZN7fBase_cnwEj(0x608);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        *(void**)p = &_ZTV5Crate;
        _ZN10dBgCh_ActrC1Ev(p + 0x320);
        _ZN11ShadowModelC1Ev(p + 0x508);
        _ZN10dCcAcPos_cC1Ev(p + 0x564);
        _ZN10dCcAcPos_cC1Ev(p + 0x5a4);
    }
    return p;
}
