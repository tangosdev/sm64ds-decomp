//cpp
// @symbol _ZN13PeachPaintingD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct Model { char pad[0x4]; ~Model(); };

struct PeachPainting : Actor {
    Model m0;   /* 0xd4 */
    virtual ~PeachPainting();
};

PeachPainting::~PeachPainting()
{
}
