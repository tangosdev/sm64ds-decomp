//cpp
// @symbol _ZN14CutsceneObjectD1Ev

struct dActor_c {
    virtual ~dActor_c();
};


struct CutsceneObject : dActor_c {
    virtual ~CutsceneObject();
};

CutsceneObject::~CutsceneObject()
{
}
