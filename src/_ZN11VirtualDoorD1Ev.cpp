//cpp
// @symbol _ZN11VirtualDoorD1Ev

struct dActor_c {
    virtual ~dActor_c();
};


struct VirtualDoor : dActor_c {
    virtual ~VirtualDoor();
};

VirtualDoor::~VirtualDoor()
{
}
