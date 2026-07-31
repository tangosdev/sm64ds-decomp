//cpp
// @symbol _ZN8PoleLiftD1Ev

struct Actor {
    char pad[0xd4];
    virtual ~Actor();
};

struct Model { char pad[0x80]; ~Model(); };
struct ExtendingMeshCollider { char pad[0x4]; ~ExtendingMeshCollider(); };

struct PoleLift : Actor {
    Model m0;   /* 0xd8 */
    ExtendingMeshCollider m1;   /* 0x158 */
    virtual ~PoleLift();
};

PoleLift::~PoleLift()
{
}
