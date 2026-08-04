//cpp
// @symbol _ZN4FlagD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct ModelAnim { char pad[0x4]; ~ModelAnim(); };

struct Flag : Actor {
    ModelAnim m0;   /* 0xd4 */
    virtual ~Flag();
};

Flag::~Flag()
{
}
