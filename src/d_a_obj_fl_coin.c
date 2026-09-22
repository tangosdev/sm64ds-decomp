// @symbol daObjFl_Coin_c_classInit
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN8dActor_cC2Ev(void *);
extern int _ZTV19BowserPuzzleManager[];
/* Reconstructed source-style name: SM64DS proves daObjFl_Coin_c through RTTI,
 * allocation size, vtable identity, and the FL_COIN registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * The project's BowserPuzzleManager implementation aliases remain unchanged.
 * Historical alias: BowserPuzzleManager_Spawn. */
int *daObjFl_Coin_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(216);
    if (p) { _ZN8dActor_cC2Ev(p); p[0] = (int)_ZTV19BowserPuzzleManager; }
    return p;
}
