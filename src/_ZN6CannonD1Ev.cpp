//cpp
// @symbol _ZN6CannonD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct Model { char pad[0x50]; ~Model(); };
struct MovingCylinderClsn { char pad[0x4]; ~MovingCylinderClsn(); };

struct Cannon : Actor {
    Model m0;   /* 0xd4 */
    MovingCylinderClsn m1;   /* 0x124 */
    virtual ~Cannon();
};

Cannon::~Cannon()
{
}
