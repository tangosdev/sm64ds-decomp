//cpp
struct TextureTransformer { ~TextureTransformer(); };
struct Model { ~Model(); };
namespace Particle { struct SysTracker { ~SysTracker(); }; }
extern "C" void func_ov006_020e80d8(void *p);
extern "C" void *func_ov004_020b29c0(void *p);
extern void *data_ov006_0213c8c4;
extern void *data_ov006_0213e448;
extern "C" void *func_ov006_020e7660(int this_) {
    *(void**)this_ = &data_ov006_0213c8c4;
    ((TextureTransformer*)(this_ + 0x51f4))->~TextureTransformer();
    func_ov006_020e80d8((void*)(this_ + 0x4fd8));
    ((Model*)(this_ + 0x4f88))->~Model();
    ((Model*)(this_ + 0x4f38))->~Model();
    *(void**)this_ = &data_ov006_0213e448;
    ((Particle::SysTracker*)(this_ + 0x471c))->~SysTracker();
    func_ov004_020b29c0((void*)this_);
    return (void*)this_;
}
