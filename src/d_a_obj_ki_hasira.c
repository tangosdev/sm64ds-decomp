// @symbol daObjKi_Hasira_c_classInit
extern void *_ZN7fBase_cnwEj(unsigned);
extern void *_ZN10dBgActor_cC2Ev(void *);
extern int _ZTV16daObjKi_Hasira_c[];
/* Reconstructed source-style name: SM64DS proves daObjKi_Hasira_c through
 * RTTI, allocation size, vtable identity, and the KI_HASIRA registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical aliases: RockPillar_Spawn, and RockPillar for the
 * class itself. */
int *daObjKi_Hasira_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(808);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV16daObjKi_Hasira_c; }
    return p;
}
