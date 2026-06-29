//cpp
extern "C" {
extern void *_ZN9ActorBasenwEj(unsigned int sz);
extern int func_ov004_020b2adc(void *p);
extern void func_020733a8(void *obj, int a, int b, void *cb1, void *cb2);
extern int data_ov006_0213eefc[];
extern void func_0203d738(void);
extern void func_0203d47c(void);
extern void func_ov006_02119900(void);
extern void func_ov006_0210d894(void);
void *func_ov006_02119824(void);
void *func_ov006_02119824(void) {
    char *o = (char *)_ZN9ActorBasenwEj(0x629c);
    if (o != 0) {
        func_ov004_020b2adc(o);
        *(int *)o = (int)data_ov006_0213eefc;
        func_020733a8(o + 0x47c8, 5, 8, (void *)func_0203d738, (void *)func_0203d47c);
        func_020733a8(o + 0x4854, 0x10, 8, (void *)func_0203d738, (void *)func_0203d47c);
        func_020733a8(o + 0x48d4, 0x10, 8, (void *)func_0203d738, (void *)func_0203d47c);
        func_020733a8(o + 0x599c, 0x40, 0x24, (void *)func_ov006_02119900, (void *)func_ov006_0210d894);
    }
    return o;
}
}
