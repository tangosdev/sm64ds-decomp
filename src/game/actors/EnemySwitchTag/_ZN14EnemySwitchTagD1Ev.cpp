//cpp
// @symbol _ZN14EnemySwitchTagD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct dCcAc_c { char pad[0x4]; ~dCcAc_c(); };

struct EnemySwitchTag : dActor_c {
    dCcAc_c m0;   /* 0xd4 */
    virtual ~EnemySwitchTag();
};

EnemySwitchTag::~EnemySwitchTag()
{
}
