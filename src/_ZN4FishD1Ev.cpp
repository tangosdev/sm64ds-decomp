//cpp
// @symbol _ZN4FishD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct ModelAnim { char pad[0x4]; ~ModelAnim(); };

struct Fish : Actor {
    ModelAnim m0;   /* 0xd4 */
    virtual ~Fish();
};

Fish::~Fish()
{
}
