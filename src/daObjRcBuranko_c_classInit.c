// @symbol daObjRcBuranko_c_classInit
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV16daObjRcBuranko_c[];
/* Reconstructed source-style name: SM64DS proves daObjRcBuranko_c through
 * RTTI, allocation size, vtable identity, and the RC_BURANKO registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. The project's daObjRcBuranko_c implementation alias remains
 * unchanged. Historical alias: SwingingPlatform_Spawn. */
int *daObjRcBuranko_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV16daObjRcBuranko_c; }
    return p;
}
