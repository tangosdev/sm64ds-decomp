//cpp
// @symbol _ZN10BrickBlockD1Ev

struct Actor {
    virtual ~Actor();
};


struct BrickBlock : Actor {
    virtual ~BrickBlock();
};

BrickBlock::~BrickBlock()
{
}
