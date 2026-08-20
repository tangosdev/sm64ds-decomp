//cpp
// @symbol _ZN9KoopaFlagD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct dCcAc_c { char pad[0x34]; ~dCcAc_c(); };
struct ModelAnim { char pad[0x4]; ~ModelAnim(); };

struct KoopaFlag : dActor_c {
    dCcAc_c m0;   /* 0xd4 */
    ModelAnim m1;   /* 0x108 */
    virtual ~KoopaFlag();
};

KoopaFlag::~KoopaFlag()
{
}
