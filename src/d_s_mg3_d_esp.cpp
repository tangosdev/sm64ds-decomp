//cpp
extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned int sz);
extern int _ZN11dScMgBase_cC2Ev(void *p);
extern void _ZN8Particle10SysTrackerC1Ev(void *);
extern void _ZN5ModelC1Ev(void *);
extern void _ZN15dMg3DEspModel_cC1Ev(void *);
extern void _ZN18TextureTransformerC1Ev(void *);
extern int _ZTV19dScMgSingle3DBase_c[];
extern int _ZTV12dScMg3DEsp_c[];
void *dScMg3DEsp_c_classInit(void);
/* Reconstructed source-style name: SM64DS proves dScMg3DEsp_c through RTTI,
 * allocation size, vtable identity, and the MG_3DESP registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgPsycheOut_Spawn. */
void *dScMg3DEsp_c_classInit(void){
    char *o = (char *)_ZN7fBase_cnwEj(0x5558);
    if(o != 0){
        _ZN11dScMgBase_cC2Ev(o);
        *(int *)o = (int)_ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(o + 0x471c);
        *(int *)o = (int)_ZTV12dScMg3DEsp_c;
        _ZN5ModelC1Ev(o + 0x4f38);
        _ZN5ModelC1Ev(o + 0x4f88);
        _ZN15dMg3DEspModel_cC1Ev(o + 0x4fd8);
        _ZN18TextureTransformerC1Ev(o + 0x51f4);
    }
    return o;
}
}
