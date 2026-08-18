//cpp
// @symbol _ZN14FlameChompFireD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct ShadowModel { char pad[0x28]; ~ShadowModel(); };
struct MovingCylinderClsn { char pad[0x34]; ~MovingCylinderClsn(); };
struct WithMeshClsn { char pad[0x4]; ~WithMeshClsn(); };

struct FlameChompFire : dActor_c {
    ShadowModel m0;   /* 0xd4 */
    MovingCylinderClsn m1;   /* 0xfc */
    WithMeshClsn m2;   /* 0x130 */
    virtual ~FlameChompFire();
};

FlameChompFire::~FlameChompFire()
{
}
