//cpp
// @symbol _ZN4FishD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct ModelAnim { char pad[0x4]; ~ModelAnim(); };

struct Fish : dActor_c {
    ModelAnim m0;   /* 0xd4 */
    virtual ~Fish();
};

Fish::~Fish()
{
}
