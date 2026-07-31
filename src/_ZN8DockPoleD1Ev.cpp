//cpp
// @symbol _ZN8DockPoleD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct ModelAnim { char pad[0x4]; ~ModelAnim(); };

struct DockPole : Actor {
    ModelAnim m0;   /* 0xd4 */
    virtual ~DockPole();
};

DockPole::~DockPole()
{
}
