//cpp
// @symbol _ZN25SlideDecorationSilverStarD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct Model { char pad[0x4]; ~Model(); };

struct SlideDecorationSilverStar : Actor {
    Model m0;   /* 0xd4 */
    virtual ~SlideDecorationSilverStar();
};

SlideDecorationSilverStar::~SlideDecorationSilverStar()
{
}
