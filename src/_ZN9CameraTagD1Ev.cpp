//cpp
// @symbol _ZN13InvisiblePoleD1Ev

struct Actor {
    virtual ~Actor();
};


struct InvisiblePole : Actor {
    virtual ~InvisiblePole();
};

InvisiblePole::~InvisiblePole()
{
}
