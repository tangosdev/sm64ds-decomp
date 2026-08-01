//cpp
// @symbol _ZN22ClockPaintingHandShortD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct Model { char pad[0x4]; ~Model(); };

struct ClockPaintingHandShort : Actor {
    Model m0;   /* 0xd4 */
    virtual ~ClockPaintingHandShort();
};

ClockPaintingHandShort::~ClockPaintingHandShort()
{
}
