//cpp
// @symbol _ZN3MrID1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct TextureSequence { char pad[0x14]; ~TextureSequence(); };
struct ShadowModel { char pad[0x28]; ~ShadowModel(); };
struct dCcAcPos_c { char pad[0x4]; ~dCcAcPos_c(); };

struct MrI : dActor_c {
    ModelAnim m0;   /* 0xd4 */
    TextureSequence m1;   /* 0x138 */
    ShadowModel m2;   /* 0x14c */
    dCcAcPos_c m3;   /* 0x174 */
    virtual ~MrI();
};

MrI::~MrI()
{
}
