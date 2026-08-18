//cpp
// @symbol _ZN13TreasureChestD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct MovingCylinderClsn { char pad[0x4]; ~MovingCylinderClsn(); };

struct TreasureChest : dActor_c {
    ModelAnim m0;   /* 0xd4 */
    MovingCylinderClsn m1;   /* 0x138 */
    virtual ~TreasureChest();
};

TreasureChest::~TreasureChest()
{
}
