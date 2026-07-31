//cpp
// @symbol _ZN6LakituD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct Model { char pad[0x50]; ~Model(); };
struct ShadowModel { char pad[0x28]; ~ShadowModel(); };
struct TextureSequence { char pad[0x14]; ~TextureSequence(); };
struct MovingCylinderClsnWithPos { char pad[0x40]; ~MovingCylinderClsnWithPos(); };
struct WithMeshClsn { char pad[0x4]; ~WithMeshClsn(); };

struct Lakitu : Actor {
    ModelAnim m0;   /* 0xd4 */
    Model m1;   /* 0x138 */
    ShadowModel m2;   /* 0x188 */
    TextureSequence m3;   /* 0x1b0 */
    MovingCylinderClsnWithPos m4;   /* 0x1c4 */
    WithMeshClsn m5;   /* 0x204 */
    virtual ~Lakitu();
};

Lakitu::~Lakitu()
{
}
