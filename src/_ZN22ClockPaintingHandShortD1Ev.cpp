//cpp
// @symbol _ZN22ClockPaintingHandShortD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct Model { char pad[0x4]; ~Model(); };

struct ClockPaintingHandShort : dActor_c {
    Model m0;   /* 0xd4 */
    virtual ~ClockPaintingHandShort();
};

ClockPaintingHandShort::~ClockPaintingHandShort()
{
}
