//cpp
// @symbol _ZN11VolcanoFireD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct MovingCylinderClsn { char pad[0x4]; ~MovingCylinderClsn(); };

struct VolcanoFire : Actor {
    MovingCylinderClsn m0;   /* 0xd4 */
    virtual ~VolcanoFire();
};

VolcanoFire::~VolcanoFire()
{
}
