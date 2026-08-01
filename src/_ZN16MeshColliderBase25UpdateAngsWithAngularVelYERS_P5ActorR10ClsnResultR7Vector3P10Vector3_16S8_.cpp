//cpp
struct Vector3 { int x, y, z; };
struct Vector3_16 { short x, y, z; };
struct Actor;
struct ClsnResult { int x, y, z; };

struct MeshColliderBase {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual int GetAngularVelY();
};

extern "C" void _ZN16MeshColliderBase25UpdateAngsWithAngularVelYERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(
    MeshColliderBase* self, Actor* a2, ClsnResult* a3, Vector3* a4, Vector3_16* a5, Vector3_16* a6)
{
    int angY = self->GetAngularVelY();
    if (a6) {
        short* py = (short*)(((long long)(int)&a6->y));
        *py = *py + angY;
    }
    if (a5) {
        short* py = (short*)(((long long)(int)&a5->y));
        *py = *py + angY;
    }
}
