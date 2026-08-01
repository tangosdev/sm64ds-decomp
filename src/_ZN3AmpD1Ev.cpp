//cpp
// @symbol _ZN3AmpD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct Model { char pad[0x50]; ~Model(); };
struct TextureSequence { char pad[0x14]; ~TextureSequence(); };
struct TextureTransformer { char pad[0x14]; ~TextureTransformer(); };
struct ShadowModel { char pad[0x28]; ~ShadowModel(); };
struct MovingCylinderClsnWithPos { char pad[0x40]; ~MovingCylinderClsnWithPos(); };
struct WithMeshClsn { char pad[0x4]; ~WithMeshClsn(); };

struct Amp : Actor {
    ModelAnim m0;   /* 0xd4 */
    Model m1;   /* 0x138 */
    TextureSequence m2;   /* 0x188 */
    TextureTransformer m3;   /* 0x19c */
    ShadowModel m4;   /* 0x1b0 */
    MovingCylinderClsnWithPos m5;   /* 0x1d8 */
    WithMeshClsn m6;   /* 0x218 */
    virtual ~Amp();
};

Amp::~Amp()
{
}
