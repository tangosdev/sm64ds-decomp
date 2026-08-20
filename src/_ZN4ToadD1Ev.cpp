//cpp
// @symbol _ZN4ToadD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct dCcAc_c { char pad[0x34]; ~dCcAc_c(); };
struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct ShadowModel { char pad[0x4]; ~ShadowModel(); };

struct Toad : dActor_c {
    dCcAc_c m0;   /* 0xd4 */
    ModelAnim m1;   /* 0x108 */
    ShadowModel m2;   /* 0x16c */
    virtual ~Toad();
};

Toad::~Toad()
{
}
