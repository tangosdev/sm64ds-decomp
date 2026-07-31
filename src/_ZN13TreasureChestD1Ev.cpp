//cpp
// @symbol _ZN13TreasureChestD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct MovingCylinderClsn { char pad[0x4]; ~MovingCylinderClsn(); };

struct TreasureChest : Actor {
    ModelAnim m0;   /* 0xd4 */
    MovingCylinderClsn m1;   /* 0x138 */
    virtual ~TreasureChest();
};

TreasureChest::~TreasureChest()
{
}
