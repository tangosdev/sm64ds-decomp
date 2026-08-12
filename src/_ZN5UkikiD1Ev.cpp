//cpp
// @symbol _ZN5UkikiD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct ShadowModel { char pad[0x28]; ~ShadowModel(); };
struct MovingCylinderClsn { char pad[0x34]; ~MovingCylinderClsn(); };
struct WithMeshClsn { char pad[0x4]; ~WithMeshClsn(); };

struct Ukiki : Actor {
    ModelAnim m0;   /* 0xd4 */
    ShadowModel m1;   /* 0x138 */
    MovingCylinderClsn m2;   /* 0x160 */
    WithMeshClsn m3;   /* 0x194 */
    virtual ~Ukiki();
};

Ukiki::~Ukiki()
{
}
