extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN11dScMgBase_cC2Ev(void *);
extern int data_ov006_0213bbb4[];
/* Reconstructed source-style name: SM64DS proves dScMgBomroom_c through RTTI,
 * allocation size, vtable identity, and the MG_BOMROOM registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgSortOrSplode_Spawn. */
int *dScMgBomroom_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(25344);
    if (p) { _ZN11dScMgBase_cC2Ev(p); p[0] = (int)data_ov006_0213bbb4; }
    return p;
}
