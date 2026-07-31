//cpp
// @symbol _ZN3MrID1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct TextureSequence { char pad[0x14]; ~TextureSequence(); };
struct ShadowModel { char pad[0x28]; ~ShadowModel(); };
struct MovingCylinderClsnWithPos { char pad[0x4]; ~MovingCylinderClsnWithPos(); };

struct MrI : Actor {
    ModelAnim m0;   /* 0xd4 */
    TextureSequence m1;   /* 0x138 */
    ShadowModel m2;   /* 0x14c */
    MovingCylinderClsnWithPos m3;   /* 0x174 */
    virtual ~MrI();
};

MrI::~MrI()
{
}
