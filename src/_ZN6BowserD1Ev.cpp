//cpp
// @symbol _ZN6BowserD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct TextureSequence { char pad[0x14]; ~TextureSequence(); };
struct WithMeshClsn { char pad[0x1bc]; ~WithMeshClsn(); };
struct ShadowModel { char pad[0x58]; ~ShadowModel(); };
struct MovingCylinderClsnWithPos { char pad[0x4]; ~MovingCylinderClsnWithPos(); };

struct Bowser : Actor {
    ModelAnim m0;   /* 0xd4 */
    TextureSequence m1;   /* 0x138 */
    WithMeshClsn m2;   /* 0x14c */
    ShadowModel m3;   /* 0x308 */
    MovingCylinderClsnWithPos m4;   /* 0x360 */
    virtual ~Bowser();
};

Bowser::~Bowser()
{
}
