//cpp
// @symbol _ZN12MansionStepsD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct Model { char pad[0x88]; ~Model(); };
struct MovingMeshCollider { char pad[0x4]; ~MovingMeshCollider(); };

struct MansionSteps : Actor {
    Model m0;   /* 0xd4 */
    MovingMeshCollider m1;   /* 0x15c */
    virtual ~MansionSteps();
};

MansionSteps::~MansionSteps()
{
}
