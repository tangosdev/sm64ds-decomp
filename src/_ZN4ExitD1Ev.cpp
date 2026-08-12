//cpp
// @symbol _ZN4ExitD1Ev

struct Actor {
    virtual ~Actor();
};


struct Exit : Actor {
    virtual ~Exit();
};

Exit::~Exit()
{
}
