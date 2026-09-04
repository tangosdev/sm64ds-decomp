//cpp
extern "C" void *_ZN7fBase_cnwEj(unsigned int sz);
extern "C" void _ZN10dBgActor_cC2Ev(void *thiz);
extern "C" void _ZN10dCcAcPos_cC1Ev(void *thiz);
extern "C" void _ZN14BlendModelAnimC1Ev(void *thiz);
extern "C" void _ZN5ModelC1Ev(void *thiz);
extern "C" void _ZN11ShadowModelC1Ev(void *thiz);
extern "C" void _ZN15TextureSequenceC1Ev(void *thiz);
extern "C" void func_020733a8(void *p, int a, int b, void *f1, void *f2);
extern "C" void _ZN10dBgW_KcMbgC1Ev(void *thiz);
extern "C" void _ZN7Vector3D1Ev(void);
extern "C" void func_0203d384(void);
extern void *_ZTV6Eyerok[];

/* Reconstructed source-style name: SM64DS proves daIwante_c through RTTI,
 * allocation size, vtable identity, and the IWANTE registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Eyerok_Spawn. */
extern "C" void *daIwante_c_classInit(void)
{
    unsigned char *c = (unsigned char *)_ZN7fBase_cnwEj(0x874);
    if (c) {
        _ZN10dBgActor_cC2Ev(c);
        *(void **)c = _ZTV6Eyerok;
        _ZN10dCcAcPos_cC1Ev(c + 0x320);
        _ZN14BlendModelAnimC1Ev(c + 0x360);
        _ZN5ModelC1Ev(c + 0x3d0);
        _ZN11ShadowModelC1Ev(c + 0x420);
        _ZN15TextureSequenceC1Ev(c + 0x448);
        func_020733a8(c + 0x4dc, 0x14, 0xc, (void *)func_0203d384, (void *)_ZN7Vector3D1Ev);
        _ZN10dBgW_KcMbgC1Ev(c + 0x674);
    }
    return c;
}
