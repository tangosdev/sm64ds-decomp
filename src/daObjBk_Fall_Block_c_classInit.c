// @symbol daObjBk_Fall_Block_c_classInit
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV16daObjFallBlock_c[];
extern int _ZTV20daObjBk_Fall_Block_c[];
/* Reconstructed source-style name: SM64DS proves daObjBk_Fall_Block_c through
 * RTTI, allocation size, most-derived vtable identity, and the BK_DOWN_B
 * registry profile; later EAD lineage supplies classInit. Exact original
 * spelling is not preserved. The project's daObjBk_Fall_Block_c implementation aliases
 * remain unchanged. Historical alias: daObjBk_Fall_Block_c_Spawn. */
int *daObjBk_Fall_Block_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(844);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV16daObjFallBlock_c;
        p[0] = (int)_ZTV20daObjBk_Fall_Block_c;
    }
    return p;
}
