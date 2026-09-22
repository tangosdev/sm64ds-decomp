//cpp
typedef int s32;

struct Vec3 { s32 x, y, z; };

struct dCc_c {
    s32 pad[3];
    s32 dx;
    s32 pad2;
    s32 dz;
};

extern "C" void AddVec3(Vec3 *a, Vec3 *b, Vec3 *c);
extern "C" void func_02010da4(int *p);

struct dActor_c {
    char pad[0x5c];
    Vec3 pos;
    char pad2[0xa4 - 0x68];
    Vec3 speed;
    void UpdatePosWithOnlySpeed(dCc_c *clsn);
};

void dActor_c::UpdatePosWithOnlySpeed(dCc_c *clsn) {
    AddVec3(&pos, &speed, &pos);
    if (clsn != 0) {
        *(s32 *)(&pos.x) += clsn->dx;
        *(s32 *)(&pos.z) += clsn->dz;
    }
    func_02010da4((int *)this);
}
