//cpp
// @symbol _ZN10BrickBlockD1Ev

struct dActor_c {
    virtual ~dActor_c();
};


struct BrickBlock : dActor_c {
    virtual ~BrickBlock();
};

BrickBlock::~BrickBlock()
{
}
