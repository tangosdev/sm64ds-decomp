//cpp
extern "C" {
extern void *_ZN9ActorBasenwEj(unsigned int sz);
extern int func_ov004_020b2adc(void *p);
extern void func_020733a8(void *obj, int a, int b, void *cb1, void *cb2);
extern int data_ov006_0213b918[];
extern void func_0203d738(void *);
extern void func_0203d47c(void *);
extern void func_ov006_020d5a50(void *);
extern void func_ov006_020d116c(void *);
void *func_ov006_020d5974(void);
void *func_ov006_020d5974(void) {
    char *o = (char *)_ZN9ActorBasenwEj(0x53fc);
    if (o != 0) {
        func_ov004_020b2adc(o);
        *(int *)o = (int)data_ov006_0213b918;
        func_020733a8(o + 0x4660, 4, 8, (void *)func_0203d738, (void *)func_0203d47c);
        func_020733a8(o + 0x4724, 4, 8, (void *)func_0203d738, (void *)func_0203d47c);
        func_020733a8(o + 0x4744, 4, 8, (void *)func_0203d738, (void *)func_0203d47c);
        func_020733a8(o + 0x4768, 0x80, 0x18, (void *)func_ov006_020d5a50, (void *)func_ov006_020d116c);
    }
    return o;
}
}
