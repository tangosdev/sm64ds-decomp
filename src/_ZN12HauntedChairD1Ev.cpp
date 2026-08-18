//cpp
// @symbol _ZN12HauntedChairD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct Model { char pad[0x50]; ~Model(); };
struct ShadowModel { char pad[0x58]; ~ShadowModel(); };
struct MovingCylinderClsnWithPos { char pad[0x40]; ~MovingCylinderClsnWithPos(); };
struct WithMeshClsn { char pad[0x4]; ~WithMeshClsn(); };

struct HauntedChair : dActor_c {
    Model m0;   /* 0xd4 */
    ShadowModel m1;   /* 0x124 */
    MovingCylinderClsnWithPos m2;   /* 0x17c */
    WithMeshClsn m3;   /* 0x1bc */
    virtual ~HauntedChair();
};

HauntedChair::~HauntedChair()
{
}
