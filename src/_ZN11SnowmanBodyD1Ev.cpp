//cpp
// @symbol _ZN11SnowmanBodyD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct Model { char pad[0x50]; ~Model(); };
struct ShadowModel { char pad[0x28]; ~ShadowModel(); };
struct dCcAc_c { char pad[0x34]; ~dCcAc_c(); };
struct WithMeshClsn { char pad[0x4]; ~WithMeshClsn(); };

struct SnowmanBody : dActor_c {
    Model m0;   /* 0xd4 */
    ShadowModel m1;   /* 0x124 */
    dCcAc_c m2;   /* 0x14c */
    WithMeshClsn m3;   /* 0x180 */
    virtual ~SnowmanBody();
};

SnowmanBody::~SnowmanBody()
{
}
