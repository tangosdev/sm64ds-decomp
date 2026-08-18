//cpp
// @symbol _ZN11BobOmbBuddyD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct MovingCylinderClsn { char pad[0x34]; ~MovingCylinderClsn(); };
struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct ShadowModel { char pad[0x4]; ~ShadowModel(); };

struct BobOmbBuddy : dActor_c {
    MovingCylinderClsn m0;   /* 0xd4 */
    ModelAnim m1;   /* 0x108 */
    ShadowModel m2;   /* 0x16c */
    virtual ~BobOmbBuddy();
};

BobOmbBuddy::~BobOmbBuddy()
{
}
