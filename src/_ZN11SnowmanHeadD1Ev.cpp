//cpp
// @symbol _ZN11SnowmanHeadD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct Model { char pad[0x50]; ~Model(); };
struct TextureSequence { char pad[0x14]; ~TextureSequence(); };
struct MovingCylinderClsn { char pad[0x34]; ~MovingCylinderClsn(); };
struct WithMeshClsn { char pad[0x4]; ~WithMeshClsn(); };

struct SnowmanHead : Actor {
    Model m0;   /* 0xd4 */
    TextureSequence m1;   /* 0x124 */
    MovingCylinderClsn m2;   /* 0x138 */
    WithMeshClsn m3;   /* 0x16c */
    virtual ~SnowmanHead();
};

SnowmanHead::~SnowmanHead()
{
}
