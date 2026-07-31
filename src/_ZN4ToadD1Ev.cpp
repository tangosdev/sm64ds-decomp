//cpp
// @symbol _ZN4ToadD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct MovingCylinderClsn { char pad[0x34]; ~MovingCylinderClsn(); };
struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct ShadowModel { char pad[0x4]; ~ShadowModel(); };

struct Toad : Actor {
    MovingCylinderClsn m0;   /* 0xd4 */
    ModelAnim m1;   /* 0x108 */
    ShadowModel m2;   /* 0x16c */
    virtual ~Toad();
};

Toad::~Toad()
{
}
