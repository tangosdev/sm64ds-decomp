//cpp
// @symbol _ZN21MegaMushroomCreateTagD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct dCcAc_c { char pad[0x4]; ~dCcAc_c(); };

struct MegaMushroomCreateTag : dActor_c {
    dCcAc_c m0;   /* 0xd4 */
    virtual ~MegaMushroomCreateTag();
};

MegaMushroomCreateTag::~MegaMushroomCreateTag()
{
}
