extern int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern int _ZN16MeshColliderBase9IsEnabledEv(void*);
extern int _ZN16MeshColliderBase6EnableEP5Actor(void*, void*);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void*, int, int);
extern unsigned char data_0209f2d8;

int _ZN11CastleWater8BehaviorEv(void* c) {
    int b;
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    b = (int)(data_0209f2d8 == 1);
    if (b != 0) {
        if (_ZN16MeshColliderBase9IsEnabledEv((char*)c+0x124) == 0)
            _ZN16MeshColliderBase6EnableEP5Actor((char*)c+0x124, c);
    } else {
        _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0);
    }
    return 1;
}
