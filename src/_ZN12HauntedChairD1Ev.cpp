//cpp
// @symbol _ZN12HauntedChairD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct Model { char pad[0x50]; ~Model(); };
struct ShadowModel { char pad[0x58]; ~ShadowModel(); };
struct MovingCylinderClsnWithPos { char pad[0x40]; ~MovingCylinderClsnWithPos(); };
struct WithMeshClsn { char pad[0x4]; ~WithMeshClsn(); };

struct HauntedChair : Actor {
    Model m0;   /* 0xd4 */
    ShadowModel m1;   /* 0x124 */
    MovingCylinderClsnWithPos m2;   /* 0x17c */
    WithMeshClsn m3;   /* 0x1bc */
    virtual ~HauntedChair();
};

HauntedChair::~HauntedChair()
{
}
