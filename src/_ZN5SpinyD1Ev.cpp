//cpp
// @symbol _ZN5SpinyD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct Model { char pad[0x50]; ~Model(); };
struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct ShadowModel { char pad[0x28]; ~ShadowModel(); };
struct MovingCylinderClsn { char pad[0x34]; ~MovingCylinderClsn(); };
struct WithMeshClsn { char pad[0x4]; ~WithMeshClsn(); };

struct Spiny : dActor_c {
    Model m0;   /* 0xd4 */
    ModelAnim m1;   /* 0x124 */
    ShadowModel m2;   /* 0x188 */
    MovingCylinderClsn m3;   /* 0x1b0 */
    WithMeshClsn m4;   /* 0x1e4 */
    virtual ~Spiny();
};

Spiny::~Spiny()
{
}
