//cpp
// @symbol _ZN4BirdD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct ShadowModel { char pad[0x4]; ~ShadowModel(); };

struct Bird : dActor_c {
    ModelAnim m0;   /* 0xd4 */
    ShadowModel m1;   /* 0x138 */
    virtual ~Bird();
};

Bird::~Bird()
{
}
