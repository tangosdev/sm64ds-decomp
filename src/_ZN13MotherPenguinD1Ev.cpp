//cpp
// @symbol _ZN13MotherPenguinD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct TextureSequence { char pad[0x14]; ~TextureSequence(); };
struct ShadowModel { char pad[0x28]; ~ShadowModel(); };
struct dCcAc_c { char pad[0x34]; ~dCcAc_c(); };
struct dBgCh_Actr { char pad[0x4]; ~dBgCh_Actr(); };

struct MotherPenguin : dActor_c {
    ModelAnim m0;   /* 0xd4 */
    TextureSequence m1;   /* 0x138 */
    ShadowModel m2;   /* 0x14c */
    dCcAc_c m3;   /* 0x174 */
    dBgCh_Actr m4;   /* 0x1a8 */
    virtual ~MotherPenguin();
};

MotherPenguin::~MotherPenguin()
{
}
