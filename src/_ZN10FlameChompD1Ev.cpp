//cpp
// @symbol _ZN10FlameChompD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct ShadowModel { char pad[0x28]; ~ShadowModel(); };
struct MovingCylinderClsnWithPos { char pad[0x40]; ~MovingCylinderClsnWithPos(); };
struct WithMeshClsn { char pad[0x4]; ~WithMeshClsn(); };

struct FlameChomp : Actor {
    ModelAnim m0;   /* 0xd4 */
    ShadowModel m1;   /* 0x138 */
    MovingCylinderClsnWithPos m2;   /* 0x160 */
    WithMeshClsn m3;   /* 0x1a0 */
    virtual ~FlameChomp();
};

FlameChomp::~FlameChomp()
{
}
