//cpp
extern "C" void *_ZN9ActorBasenwEj(unsigned int sz);
extern "C" void _ZN8PlatformC2Ev(void *thiz);
extern "C" void _ZN25MovingCylinderClsnWithPosC1Ev(void *thiz);
extern "C" void _ZN14BlendModelAnimC1Ev(void *thiz);
extern "C" void _ZN5ModelC1Ev(void *thiz);
extern "C" void _ZN11ShadowModelC1Ev(void *thiz);
extern "C" void _ZN15TextureSequenceC1Ev(void *thiz);
extern "C" void func_020733a8(void *p, int a, int b, void *f1, void *f2);
extern "C" void _ZN18MovingMeshColliderC1Ev(void *thiz);
extern "C" void func_020072c0(void);
extern "C" void func_0203d384(void);
extern void *_ZTV6Eyerok[];

extern "C" void *Eyerok_Spawn(void)
{
    unsigned char *c = (unsigned char *)_ZN9ActorBasenwEj(0x874);
    if (c) {
        _ZN8PlatformC2Ev(c);
        *(void **)c = _ZTV6Eyerok;
        _ZN25MovingCylinderClsnWithPosC1Ev(c + 0x320);
        _ZN14BlendModelAnimC1Ev(c + 0x360);
        _ZN5ModelC1Ev(c + 0x3d0);
        _ZN11ShadowModelC1Ev(c + 0x420);
        _ZN15TextureSequenceC1Ev(c + 0x448);
        func_020733a8(c + 0x4dc, 0x14, 0xc, (void *)func_0203d384, (void *)func_020072c0);
        _ZN18MovingMeshColliderC1Ev(c + 0x674);
    }
    return c;
}
