//cpp
// @symbol _ZN11CrazedCrateD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct Model { char pad[0x50]; ~Model(); };
struct ShadowModel { char pad[0x28]; ~ShadowModel(); };
struct MovingCylinderClsn { char pad[0x34]; ~MovingCylinderClsn(); };
struct WithMeshClsn { char pad[0x4]; ~WithMeshClsn(); };

struct CrazedCrate : Actor {
    Model m0;   /* 0xd4 */
    ShadowModel m1;   /* 0x124 */
    MovingCylinderClsn m2;   /* 0x14c */
    WithMeshClsn m3;   /* 0x180 */
    virtual ~CrazedCrate();
};

CrazedCrate::~CrazedCrate()
{
}
