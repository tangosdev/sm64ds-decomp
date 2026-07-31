//cpp
// @symbol _ZN7TornadoD1Ev

struct Actor {
    char pad[0xd0];
    virtual ~Actor();
};

struct MovingCylinderClsn { char pad[0x34]; ~MovingCylinderClsn(); };
struct WithMeshClsn { char pad[0x1bc]; ~WithMeshClsn(); };
struct ModelAnim { char pad[0x64]; ~ModelAnim(); };
struct TextureTransformer { char pad[0x4]; ~TextureTransformer(); };

struct Tornado : Actor {
    MovingCylinderClsn m0;   /* 0xd4 */
    WithMeshClsn m1;   /* 0x108 */
    ModelAnim m2;   /* 0x2c4 */
    TextureTransformer m3;   /* 0x328 */
    virtual ~Tornado();
};

Tornado::~Tornado()
{
}
