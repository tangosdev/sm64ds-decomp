//cpp
// @symbol _ZN10StarMarkerD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct dCcAcPos_c { char pad[0x40]; ~dCcAcPos_c(); };
struct Model { char pad[0x50]; ~Model(); };
struct ShadowModel { char pad[0x4]; ~ShadowModel(); };

struct StarMarker : dActor_c {
    dCcAcPos_c m0;   /* 0xd4 */
    Model m1;   /* 0x114 */
    ShadowModel m2;   /* 0x164 */
    virtual ~StarMarker();
};

StarMarker::~StarMarker()
{
}
