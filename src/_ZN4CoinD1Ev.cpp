//cpp
// @symbol _ZN4CoinD1Ev

struct Actor {
    char pad[0xd4];
    virtual ~Actor();
};

struct CommonModel { char pad[0x3c]; ~CommonModel(); };
struct ShadowModel { char pad[0x28]; ~ShadowModel(); };
struct MovingCylinderClsn { char pad[0x34]; ~MovingCylinderClsn(); };
struct WithMeshClsn { char pad[0x4]; ~WithMeshClsn(); };

struct Coin : Actor {
    CommonModel m0;   /* 0xd8 */
    CommonModel m1;   /* 0x114 */
    ShadowModel m2;   /* 0x150 */
    MovingCylinderClsn m3;   /* 0x178 */
    WithMeshClsn m4;   /* 0x1ac */
    virtual ~Coin();
};

Coin::~Coin()
{
}
