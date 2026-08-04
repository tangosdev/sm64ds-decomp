//cpp
// @symbol _ZN14EnemySwitchTagD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct MovingCylinderClsn { char pad[0x4]; ~MovingCylinderClsn(); };

struct EnemySwitchTag : Actor {
    MovingCylinderClsn m0;   /* 0xd4 */
    virtual ~EnemySwitchTag();
};

EnemySwitchTag::~EnemySwitchTag()
{
}
