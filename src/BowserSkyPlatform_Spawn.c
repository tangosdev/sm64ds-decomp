extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV17BowserSkyPlatform[];
/* Reconstructed source-style name: SM64DS proves daKpa3Bg_c through RTTI,
 * allocation size, vtable identity, and the KOOPA3BG registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: BowserSkyPlatform_Spawn. */
int *daKpa3Bg_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(812);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV17BowserSkyPlatform; }
    return p;
}
