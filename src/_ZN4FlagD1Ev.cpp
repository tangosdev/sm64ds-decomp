//cpp
// @symbol _ZN4FlagD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct ModelAnim { char pad[0x4]; ~ModelAnim(); };

struct Flag : dActor_c {
    ModelAnim m0;   /* 0xd4 */
    virtual ~Flag();
};

Flag::~Flag()
{
}
