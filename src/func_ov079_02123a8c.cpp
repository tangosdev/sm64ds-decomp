//cpp
struct Vec3 { int x, y, z; };

struct Actor {
    virtual void vf00();  virtual void vf01();  virtual void vf02();  virtual void vf03();
    virtual void vf04();  virtual void vf05();  virtual void vf06();  virtual void vf07();
    virtual void vf08();  virtual void vf09();  virtual void vf10();  virtual void vf11();
    virtual void vf12();  virtual void vf13();  virtual void vf14();  virtual void vf15();
    virtual void vf16();  virtual void vf17();  virtual void vf18();  virtual void vf19();
    virtual void vf20();  virtual void vf21();  virtual void vf22();  virtual void vf23();
    virtual void vf24();  virtual void vf25();  virtual void vf26();  virtual void vf27();
    virtual void vf28();  virtual int  vf29();
    Actor *ClosestWithActorID(unsigned int id);
};

extern "C" int Vec3_Dist(void* a, void* b);
extern "C" int _ZN16MeshColliderBase9IsEnabledEv(void* m);
extern "C" void _ZN16MeshColliderBase6EnableEP5Actor(void* m, void* actor);

extern "C" int func_ov079_02123a8c(Actor *self)
{
    Vec3 v;

    if (*(unsigned char*)((char*)self + 0x404) == 0)
        return 0;

    Actor *closest = self->ClosestWithActorID(9);
    if (closest) {
        v.x = *(int*)((char*)self + 0x5c);
        v.y = *(int*)((char*)self + 0x60);
        v.z = *(int*)((char*)self + 0x64);
        v.y = v.y + self->vf29();
        int dist = Vec3_Dist(&v, (char*)closest + 0x5c);
        if (dist < (*(int*)((char*)self + 0xb8) << 3)) {
            if (!_ZN16MeshColliderBase9IsEnabledEv((char*)self + 0x418))
                _ZN16MeshColliderBase6EnableEP5Actor((char*)self + 0x418, self);
            return 1;
        }
    }
    return 0;
}
