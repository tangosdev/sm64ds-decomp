//cpp
// @symbol _ZN11WingFeatherD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct Model { char pad[0x50]; ~Model(); };
struct MovingCylinderClsn { char pad[0x34]; ~MovingCylinderClsn(); };
struct WithMeshClsn { char pad[0x1bc]; ~WithMeshClsn(); };
struct ShadowModel { char pad[0x4]; ~ShadowModel(); };

struct WingFeather : Actor {
    Model m0;   /* 0xd4 */
    MovingCylinderClsn m1;   /* 0x124 */
    WithMeshClsn m2;   /* 0x158 */
    ShadowModel m3;   /* 0x314 */
    virtual ~WingFeather();
};

WingFeather::~WingFeather()
{
}
