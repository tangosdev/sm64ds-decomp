//cpp
typedef int Fix12;
struct SharedFilePtr;
struct BMD_File;
struct BCA_File;
struct Actor;

struct Model {
    static void* LoadFile(SharedFilePtr& f);
};
struct ModelBase {
    void SetFile(BMD_File* f, int a, int b);
};
struct Animation {
    static void* LoadFile(SharedFilePtr& f);
};
struct ModelAnim {
    void SetAnim(BCA_File* f, int a, Fix12 b, unsigned int c);
};
struct MovingCylinderClsn {
    void Init(Actor* a, Fix12 b, Fix12 c, unsigned int d, unsigned int e);
};

extern SharedFilePtr data_ov062_0211e0d4;
extern SharedFilePtr data_ov062_0211e0dc;

extern "C" int _ZN9KoopaFlag13InitResourcesEv(char* c)
{
    ((ModelBase*)(c + 0x108))->SetFile(
        (BMD_File*)Model::LoadFile(data_ov062_0211e0d4), 1, -1);
    ((ModelAnim*)(c + 0x108))->SetAnim(
        (BCA_File*)Animation::LoadFile(data_ov062_0211e0dc), 0, 0x1000, 0);
    ((MovingCylinderClsn*)(c + 0xd4))->Init((Actor*)c, 0x35555, 0x294000, 0x280000c, 0);
    *(unsigned char*)(c + 0x16e) = 0xff;
    *(short*)(c + 0x16c) = 0;
    return 1;
}
