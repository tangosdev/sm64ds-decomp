// @symbol daObjKi_Hasira_c_classInit
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV10RockPillar[];
/* Reconstructed source-style name: SM64DS proves daObjKi_Hasira_c through
 * RTTI, allocation size, vtable identity, and the KI_HASIRA registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. The project's RockPillar implementation aliases remain
 * unchanged. Historical alias: RockPillar_Spawn. */
int *daObjKi_Hasira_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(808);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV10RockPillar; }
    return p;
}
