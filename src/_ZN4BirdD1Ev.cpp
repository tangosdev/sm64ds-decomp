//cpp
// @symbol _ZN4BirdD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct ShadowModel { char pad[0x4]; ~ShadowModel(); };

struct Bird : Actor {
    ModelAnim m0;   /* 0xd4 */
    ShadowModel m1;   /* 0x138 */
    virtual ~Bird();
};

Bird::~Bird()
{
}
