//cpp
// @symbol _ZN14CutsceneObjectD1Ev

struct Actor {
    virtual ~Actor();
};


struct CutsceneObject : Actor {
    virtual ~CutsceneObject();
};

CutsceneObject::~CutsceneObject()
{
}
