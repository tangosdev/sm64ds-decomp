//cpp
// @symbol _ZN13RollingLogLllD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct MovingCylinderClsn { char pad[0x4]; ~MovingCylinderClsn(); };

struct RollingLogLll : Actor {
    MovingCylinderClsn m0;   /* 0xd4 */
    virtual ~RollingLogLll();
};

RollingLogLll::~RollingLogLll()
{
}
