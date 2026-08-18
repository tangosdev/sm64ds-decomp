//cpp
// @symbol _ZN11WingFeatherD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct Model { char pad[0x50]; ~Model(); };
struct MovingCylinderClsn { char pad[0x34]; ~MovingCylinderClsn(); };
struct WithMeshClsn { char pad[0x1bc]; ~WithMeshClsn(); };
struct ShadowModel { char pad[0x4]; ~ShadowModel(); };

struct WingFeather : dActor_c {
    Model m0;   /* 0xd4 */
    MovingCylinderClsn m1;   /* 0x124 */
    WithMeshClsn m2;   /* 0x158 */
    ShadowModel m3;   /* 0x314 */
    virtual ~WingFeather();
};

WingFeather::~WingFeather()
{
}
