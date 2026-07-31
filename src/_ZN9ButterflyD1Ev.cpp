//cpp
// @symbol _ZN9ButterflyD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct Model { char pad[0x50]; ~Model(); };
struct ShadowModel { char pad[0x28]; ~ShadowModel(); };
struct WithMeshClsn { char pad[0x1bc]; ~WithMeshClsn(); };
struct MovingCylinderClsnWithPos { char pad[0x4]; ~MovingCylinderClsnWithPos(); };

struct Butterfly : Actor {
    ModelAnim m0;   /* 0xd4 */
    Model m1;   /* 0x138 */
    ShadowModel m2;   /* 0x188 */
    ShadowModel m3;   /* 0x1b0 */
    WithMeshClsn m4;   /* 0x1d8 */
    MovingCylinderClsnWithPos m5;   /* 0x394 */
    virtual ~Butterfly();
};

Butterfly::~Butterfly()
{
}
