//cpp
// @symbol _ZN9MontyMoleD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct MovingCylinderClsn { char pad[0x4]; ~MovingCylinderClsn(); };

struct MontyMole : Actor {
    ModelAnim m0;   /* 0xd4 */
    MovingCylinderClsn m1;   /* 0x138 */
    virtual ~MontyMole();
};

MontyMole::~MontyMole()
{
}
