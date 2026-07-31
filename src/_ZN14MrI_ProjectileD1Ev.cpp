//cpp
// @symbol _ZN14MrI_ProjectileD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct ShadowModel { char pad[0x28]; ~ShadowModel(); };
struct MovingCylinderClsnWithPos { char pad[0x40]; ~MovingCylinderClsnWithPos(); };
struct WithMeshClsn { char pad[0x4]; ~WithMeshClsn(); };

struct MrI_Projectile : Actor {
    ShadowModel m0;   /* 0xd4 */
    MovingCylinderClsnWithPos m1;   /* 0xfc */
    WithMeshClsn m2;   /* 0x13c */
    virtual ~MrI_Projectile();
};

MrI_Projectile::~MrI_Projectile()
{
}
