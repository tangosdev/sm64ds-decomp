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
    virtual void v11();
    virtual void m12(Vector3& out);

    void UpdatePosWithVelocity(MeshColliderBase& a1, Actor* a2, ClsnResult& a3,
                               Vector3& a4, Vector3_16* a5, Vector3_16* a6);
};

void MeshColliderBase::UpdatePosWithVelocity(MeshColliderBase& a1, Actor* a2, ClsnResult& a3,
                                             Vector3& a4, Vector3_16* a5, Vector3_16* a6)
{
    Vector3 vel;
    this->m12(vel);
    a3.x = a3.x + vel.x;
    {
        int* py = (int*)(((long long)(int)&a3.y));
        *py = *py + vel.y;
    }
    {
        int* pz = (int*)(((long long)(int)&a3.z));
        *pz = *pz + vel.z;
    }
}
