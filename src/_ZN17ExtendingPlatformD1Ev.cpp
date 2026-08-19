//cpp
// @symbol _ZN17ExtendingPlatformD1Ev

struct dActor_c {
    char pad[0xd4];
    virtual ~dActor_c();
};

struct Model { char pad[0x80]; ~Model(); };
struct dBgW_KcMbgSclY { char pad[0x4]; ~dBgW_KcMbgSclY(); };

struct ExtendingPlatform : dActor_c {
    Model m0;   /* 0xd8 */
    dBgW_KcMbgSclY m1;   /* 0x158 */
    virtual ~ExtendingPlatform();
};

ExtendingPlatform::~ExtendingPlatform()
{
}
