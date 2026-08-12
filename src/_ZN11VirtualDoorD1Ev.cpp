//cpp
// @symbol _ZN9CameraTagD1Ev

struct Actor {
    virtual ~Actor();
};


struct CameraTag : Actor {
    virtual ~CameraTag();
};

CameraTag::~CameraTag()
{
}
