//cpp
typedef int s32;

struct Vec3 { s32 x, y, z; };

struct CylinderClsn {
    s32 pad[3];
    s32 dx;
    s32 pad2;
    s32 dz;
};

extern "C" void AddVec3(Vec3 *a, Vec3 *b, Vec3 *c);
extern "C" void func_02010da4(int *p);

struct Actor {
    char pad[0x5c];
    Vec3 pos;
    char pad2[0xa4 - 0x68];
    Vec3 speed;
    void UpdatePosWithOnlySpeed(CylinderClsn *clsn);
};

void Actor::UpdatePosWithOnlySpeed(CylinderClsn *clsn) {
    AddVec3(&pos, &speed, &pos);
    if (clsn != 0) {
        *(s32 *)(((long long)(int)(&pos.x)) & 0xFFFFFFFFFFFFFFFFLL) += clsn->dx;
        *(s32 *)(((long long)(int)(&pos.z)) & 0xFFFFFFFFFFFFFFFFLL) += clsn->dz;
    }
    func_02010da4((int *)this);
}
