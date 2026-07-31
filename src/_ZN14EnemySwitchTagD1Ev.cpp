//cpp
// @symbol _ZN14EnemySwitchTagD1Ev

struct Actor {
    virtual ~Actor();
};


struct EnemySwitchTag : Actor {
    virtual ~EnemySwitchTag();
};

EnemySwitchTag::~EnemySwitchTag()
{
}
