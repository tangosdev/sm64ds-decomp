//cpp
// @symbol _ZN21MegaMushroomCreateTagD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct MovingCylinderClsn { char pad[0x4]; ~MovingCylinderClsn(); };

struct MegaMushroomCreateTag : dActor_c {
    MovingCylinderClsn m0;   /* 0xd4 */
    virtual ~MegaMushroomCreateTag();
};

MegaMushroomCreateTag::~MegaMushroomCreateTag()
{
}
