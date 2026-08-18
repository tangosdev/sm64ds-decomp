//cpp
// @symbol _ZN11PowerFlowerD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct Model { char pad[0x50]; ~Model(); };
struct ShadowModel { char pad[0x58]; ~ShadowModel(); };
struct MovingCylinderClsn { char pad[0x34]; ~MovingCylinderClsn(); };
struct WithMeshClsn { char pad[0x4]; ~WithMeshClsn(); };

struct PowerFlower : dActor_c {
    Model m0;   /* 0xd4 */
    Model m1;   /* 0x124 */
    ShadowModel m2;   /* 0x174 */
    MovingCylinderClsn m3;   /* 0x1cc */
    WithMeshClsn m4;   /* 0x200 */
    virtual ~PowerFlower();
};

PowerFlower::~PowerFlower()
{
}
