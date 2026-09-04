//cpp
extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned int sz);
extern int _ZN11dScMgBase_cC2Ev(void *p);
extern void func_020733a8(void *obj, int a, int b, void *cb1, void *cb2);
extern int data_ov006_0213b918[];
extern void func_0203d738(void *);
extern void NullDestructor_0203d47c(void *);
extern void func_ov006_020d5a50(void *);
extern void func_ov006_020d116c(void *);
void *dScMgAmida_c_classInit(void);
/* Reconstructed source-style name: SM64DS proves dScMgAmida_c through RTTI,
 * allocation size, vtable identity, and the MG_AMIDA registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: func_ov006_020d5974. */
void *dScMgAmida_c_classInit(void) {
    char *o = (char *)_ZN7fBase_cnwEj(0x53fc);
    if (o != 0) {
        _ZN11dScMgBase_cC2Ev(o);
        *(int *)o = (int)data_ov006_0213b918;
        func_020733a8(o + 0x4660, 4, 8, (void *)func_0203d738, (void *)NullDestructor_0203d47c);
        func_020733a8(o + 0x4724, 4, 8, (void *)func_0203d738, (void *)NullDestructor_0203d47c);
        func_020733a8(o + 0x4744, 4, 8, (void *)func_0203d738, (void *)NullDestructor_0203d47c);
        func_020733a8(o + 0x4768, 0x80, 0x18, (void *)func_ov006_020d5a50, (void *)func_ov006_020d116c);
    }
    return o;
}
}
