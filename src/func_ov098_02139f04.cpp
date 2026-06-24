//cpp
extern "C" {
void* _ZN9ActorBasenwEj(unsigned int sz);
void _ZN8PlatformC2Ev(void* p);
void _ZN12WithMeshClsnC1Ev(void* p);
void _ZN11ShadowModelC1Ev(void* p);
void _ZN25MovingCylinderClsnWithPosC1Ev(void* p);
}
extern void* data_ov098_0213c534;

extern "C" void* func_ov098_02139f04(void)
{
    char* p = (char*)_ZN9ActorBasenwEj(0x608);
    if (p) {
        _ZN8PlatformC2Ev(p);
        *(void**)p = &data_ov098_0213c534;
        _ZN12WithMeshClsnC1Ev(p + 0x320);
        _ZN11ShadowModelC1Ev(p + 0x508);
        _ZN25MovingCylinderClsnWithPosC1Ev(p + 0x564);
        _ZN25MovingCylinderClsnWithPosC1Ev(p + 0x5a4);
    }
    return p;
}
