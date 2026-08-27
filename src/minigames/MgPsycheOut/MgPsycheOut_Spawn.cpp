//cpp
extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned int sz);
extern int func_ov004_020b2adc(void *p);
extern void _ZN8Particle10SysTrackerC1Ev(void *);
extern void _ZN5ModelC1Ev(void *);
extern void func_ov006_020e814c(void *);
extern void _ZN18TextureTransformerC1Ev(void *);
extern int _ZTV19dScMgSingle3DBase_c[];
extern int _ZTV12dScMg3DEsp_c[];
void *MgPsycheOut_Spawn(void);
void *MgPsycheOut_Spawn(void){
    char *o = (char *)_ZN7fBase_cnwEj(0x5558);
    if(o != 0){
        func_ov004_020b2adc(o);
        *(int *)o = (int)_ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(o + 0x471c);
        *(int *)o = (int)_ZTV12dScMg3DEsp_c;
        _ZN5ModelC1Ev(o + 0x4f38);
        _ZN5ModelC1Ev(o + 0x4f88);
        func_ov006_020e814c(o + 0x4fd8);
        _ZN18TextureTransformerC1Ev(o + 0x51f4);
    }
    return o;
}
}
