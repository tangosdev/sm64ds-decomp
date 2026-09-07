//cpp
extern "C" void *_ZN7fBase_cnwEj(unsigned int sz);
extern "C" void *_ZN8dActor_cC1Ev(void *thiz);

extern "C" void *StarCamera_Spawn(void)
{
    void *p = _ZN7fBase_cnwEj(0xd4);
    if (p == 0) return p;
    return _ZN8dActor_cC1Ev(p);
}
