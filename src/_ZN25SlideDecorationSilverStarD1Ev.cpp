//cpp
// @symbol _ZN25SlideDecorationSilverStarD1Ev

struct dActor_c {
    char pad[0xd0];
    virtual ~dActor_c();
};

struct Model { char pad[0x4]; ~Model(); };

struct SlideDecorationSilverStar : dActor_c {
    Model m0;   /* 0xd4 */
    virtual ~SlideDecorationSilverStar();
};

SlideDecorationSilverStar::~SlideDecorationSilverStar()
{
}
