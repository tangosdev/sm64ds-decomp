//cpp
// @symbol _ZN9BlueFlameD1Ev

struct Actor {
    char pad[0xe0];
    virtual ~Actor();
};

struct MovingCylinderClsn { char pad[0x4]; ~MovingCylinderClsn(); };

struct BlueFlame : Actor {
    MovingCylinderClsn m0;   /* 0xe4 */
    virtual ~BlueFlame();
};

BlueFlame::~BlueFlame()
{
}
