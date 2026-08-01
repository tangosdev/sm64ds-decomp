//cpp
// @symbol _ZN21MegaMushroomCreateTagD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct MovingCylinderClsn { char pad[0x4]; ~MovingCylinderClsn(); };

struct MegaMushroomCreateTag : Actor {
    MovingCylinderClsn m0;   /* 0xd4 */
    virtual ~MegaMushroomCreateTag();
};

MegaMushroomCreateTag::~MegaMushroomCreateTag()
{
}
