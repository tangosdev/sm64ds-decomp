//cpp
extern "C" {
extern void *_ZN9ActorBasenwEj(unsigned int sz);
extern int func_ov004_020b2adc(void *p);
extern void _ZN8Particle10SysTrackerC1Ev(void *);
extern void _ZN5ModelC1Ev(void *);
extern void func_ov006_020e814c(void *);
extern void _ZN18TextureTransformerC1Ev(void *);
extern int data_ov006_0213e448[];
extern int data_ov006_0213c8c4[];
void *MgPsycheOut_Spawn(void);
void *MgPsycheOut_Spawn(void){
    char *o = (char *)_ZN9ActorBasenwEj(0x5558);
    if(o != 0){
        func_ov004_020b2adc(o);
        *(int *)o = (int)data_ov006_0213e448;
        _ZN8Particle10SysTrackerC1Ev(o + 0x471c);
        *(int *)o = (int)data_ov006_0213c8c4;
        _ZN5ModelC1Ev(o + 0x4f38);
        _ZN5ModelC1Ev(o + 0x4f88);
        func_ov006_020e814c(o + 0x4fd8);
        _ZN18TextureTransformerC1Ev(o + 0x51f4);
    }
    return o;
}
}
