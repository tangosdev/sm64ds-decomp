//cpp
// @symbol _ZN19OrangeBallBillboardD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct Model { char pad[0x4]; ~Model(); };

struct OrangeBallBillboard : Actor {
    Model m0;   /* 0xd4 */
    virtual ~OrangeBallBillboard();
};

OrangeBallBillboard::~OrangeBallBillboard()
{
}
