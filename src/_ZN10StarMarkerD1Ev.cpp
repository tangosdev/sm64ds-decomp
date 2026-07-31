//cpp
// @symbol _ZN10StarMarkerD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct MovingCylinderClsnWithPos { char pad[0x40]; ~MovingCylinderClsnWithPos(); };
struct Model { char pad[0x50]; ~Model(); };
struct ShadowModel { char pad[0x4]; ~ShadowModel(); };

struct StarMarker : Actor {
    MovingCylinderClsnWithPos m0;   /* 0xd4 */
    Model m1;   /* 0x114 */
    ShadowModel m2;   /* 0x164 */
    virtual ~StarMarker();
};

StarMarker::~StarMarker()
{
}
