// @symbol daObjBkKillerdai_c_classInit
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV11BillBlaster[];
/* Reconstructed source-style name: SM64DS proves daObjBkKillerdai_c through
 * RTTI, allocation size, vtable identity, and the BK_KILLER_DAI registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. The project's BillBlaster implementation aliases remain
 * unchanged. Historical alias: BillBlaster_Spawn. */
int *daObjBkKillerdai_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(804);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV11BillBlaster; }
    return p;
}
