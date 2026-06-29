//cpp
struct Vector3 { int x, y, z; };
struct Actor {
    virtual void f00(); virtual void f01(); virtual void f02(); virtual void f03();
    virtual void f04(); virtual void f05(); virtual void f06(); virtual void f07();
    virtual void f08(); virtual void f09(); virtual void f10(); virtual void f11();
    virtual void f12(); virtual void f13(); virtual void f14(); virtual void f15();
    virtual void f16(); virtual void f17(); virtual void f18(); virtual void f19();
    virtual void f20(); virtual void f21(); virtual void f22(); virtual void f23();
    virtual void f24(); virtual void f25(); virtual void f26(); virtual void f27();
    virtual void f28(); virtual int f29();
};
extern "C" {
extern void* _ZN5Actor18ClosestWithActorIDEj(void*, unsigned int);
extern int Vec3_Dist(void*, void*);
extern int _ZN16MeshColliderBase9IsEnabledEv(void*);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void*, void*);

int func_ov091_02132dc0(char* c) {
    void* p = _ZN5Actor18ClosestWithActorIDEj(c, 9);
    if (p != 0) {
        volatile struct Vector3 v;
        v.x = *(int*)(c + 0x5c);
        v.y = *(int*)(c + 0x60);
        v.z = *(int*)(c + 0x64);
        v.y = v.y + ((Actor*)c)->f29();
        if (Vec3_Dist((char*)c + 0x5c, (char*)p + 0x5c) < (*(int*)(c + 0xb8) << 3)) {
            if (!_ZN16MeshColliderBase9IsEnabledEv(c + 0x124)) {
                _ZN16MeshColliderBase6EnableEP5Actor(c + 0x124, c);
                return 1;
            }
        }
    }
    return 0;
}
}
