//cpp
// @symbol _ZN12HealingHeartD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct MovingCylinderClsn { char pad[0x4]; ~MovingCylinderClsn(); };

struct HealingHeart : Actor {
    ModelAnim m0;   /* 0xd4 */
    MovingCylinderClsn m1;   /* 0x138 */
    virtual ~HealingHeart();
};

HealingHeart::~HealingHeart()
{
}
