//cpp
// @symbol _ZN7TornadoD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct dCcAc_c { char pad[0x34]; ~dCcAc_c(); };
struct dBgCh_Actr { char pad[0x1bc]; ~dBgCh_Actr(); };
struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct TextureTransformer { char pad[0x4]; ~TextureTransformer(); };

struct Tornado : dActor_c {
    dCcAc_c m0;   /* 0xd4 */
    dBgCh_Actr m1;   /* 0x108 */
    ModelAnim m2;   /* 0x2c4 */
    TextureTransformer m3;   /* 0x328 */
    virtual ~Tornado();
};

Tornado::~Tornado()
{
}
