//cpp
// @symbol _ZN13RacingPenguinD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct TextureSequence { char pad[0x14]; ~TextureSequence(); };
struct ShadowModel { char pad[0x28]; ~ShadowModel(); };
struct MovingCylinderClsn { char pad[0x34]; ~MovingCylinderClsn(); };
struct WithMeshClsn { char pad[0x4]; ~WithMeshClsn(); };

struct RacingPenguin : Actor {
    ModelAnim m0;   /* 0xd4 */
    TextureSequence m1;   /* 0x138 */
    ShadowModel m2;   /* 0x14c */
    MovingCylinderClsn m3;   /* 0x174 */
    WithMeshClsn m4;   /* 0x1a8 */
    virtual ~RacingPenguin();
};

RacingPenguin::~RacingPenguin()
{
}
