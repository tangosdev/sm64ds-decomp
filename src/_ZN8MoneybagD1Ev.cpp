//cpp
// @symbol _ZN8MoneybagD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct Model { char pad[0x50]; ~Model(); };
struct ShadowModel { char pad[0x28]; ~ShadowModel(); };
struct MovingCylinderClsn { char pad[0x34]; ~MovingCylinderClsn(); };
struct WithMeshClsn { char pad[0x4]; ~WithMeshClsn(); };

struct Moneybag : Actor {
    ModelAnim m0;   /* 0xd4 */
    Model m1;   /* 0x138 */
    ShadowModel m2;   /* 0x188 */
    MovingCylinderClsn m3;   /* 0x1b0 */
    WithMeshClsn m4;   /* 0x1e4 */
    virtual ~Moneybag();
};

Moneybag::~Moneybag()
{
}
