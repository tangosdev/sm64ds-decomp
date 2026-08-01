//cpp
// @symbol _ZN14BlueCoinSwitchD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct MovingCylinderClsn { char pad[0x4]; ~MovingCylinderClsn(); };

struct BlueCoinSwitch : Actor {
    MovingCylinderClsn m0;   /* 0xd4 */
    virtual ~BlueCoinSwitch();
};

BlueCoinSwitch::~BlueCoinSwitch()
{
}
