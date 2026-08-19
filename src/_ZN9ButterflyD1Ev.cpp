//cpp
// @symbol _ZN9ButterflyD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct Model { char pad[0x50]; ~Model(); };
struct ShadowModel { char pad[0x28]; ~ShadowModel(); };
struct dBgCh_Actr { char pad[0x1bc]; ~dBgCh_Actr(); };
struct dCcAcPos_c { char pad[0x4]; ~dCcAcPos_c(); };

struct Butterfly : dActor_c {
    ModelAnim m0;   /* 0xd4 */
    Model m1;   /* 0x138 */
    ShadowModel m2;   /* 0x188 */
    ShadowModel m3;   /* 0x1b0 */
    dBgCh_Actr m4;   /* 0x1d8 */
    dCcAcPos_c m5;   /* 0x394 */
    virtual ~Butterfly();
};

Butterfly::~Butterfly()
{
}
