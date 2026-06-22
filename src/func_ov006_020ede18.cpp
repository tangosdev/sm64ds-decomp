//cpp
extern "C" {
extern void *_ZN9ActorBasenwEj(unsigned int sz);
extern int func_ov004_020b2adc(void *p);
extern void func_020733a8(void *obj, int a, int b, void *cb1, void *cb2);
extern int data_ov006_0213cab8[];
extern void func_ov006_020ede80(void *);
extern void func_ov006_020ea324(void *);
void *func_ov006_020ede18(void);
void *func_ov006_020ede18(void) {
    char *o = (char *)_ZN9ActorBasenwEj(0x4f68);
    if (o != 0) {
        func_ov004_020b2adc(o);
        *(int *)o = (int)data_ov006_0213cab8;
        func_020733a8(o + 0x4678, 0xf, 0x98, (void *)func_ov006_020ede80, (void *)func_ov006_020ea324);
    }
    return o;
}
}
