//cpp
// @symbol _ZN13PrincessPeachD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct ShadowModel { char pad[0x28]; ~ShadowModel(); };
struct dCcAc_c { char pad[0x34]; ~dCcAc_c(); };
struct dBgCh_Actr { char pad[0x4]; ~dBgCh_Actr(); };

struct PrincessPeach : dActor_c {
    ModelAnim m0;   /* 0xd4 */
    ShadowModel m1;   /* 0x138 */
    dCcAc_c m2;   /* 0x160 */
    dBgCh_Actr m3;   /* 0x194 */
    virtual ~PrincessPeach();
};

PrincessPeach::~PrincessPeach()
{
}
