//cpp
// @symbol _ZN9KoopaFlagD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct MovingCylinderClsn { char pad[0x34]; ~MovingCylinderClsn(); };
struct ModelAnim { char pad[0x4]; ~ModelAnim(); };

struct KoopaFlag : dActor_c {
    MovingCylinderClsn m0;   /* 0xd4 */
    ModelAnim m1;   /* 0x108 */
    virtual ~KoopaFlag();
};

KoopaFlag::~KoopaFlag()
{
}
