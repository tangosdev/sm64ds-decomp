//cpp
// @symbol _ZN4ExitD1Ev

struct dActor_c {
    virtual ~dActor_c();
};


struct Exit : dActor_c {
    virtual ~Exit();
};

Exit::~Exit()
{
}
