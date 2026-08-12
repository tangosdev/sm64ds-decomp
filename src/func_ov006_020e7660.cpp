//cpp
struct TextureTransformer { ~TextureTransformer(); };
struct Model { ~Model(); };
namespace Particle { struct SysTracker { ~SysTracker(); }; }
extern "C" void func_ov006_020e80d8(void *p);
extern "C" void *_ZN11dScMgBase_cD2Ev(void *p);
extern void *data_ov006_0213c8c4;
extern void *_ZTV19dScMgSingle3DBase_c;
extern "C" void *func_ov006_020e7660(int this_) {
    *(void**)this_ = &data_ov006_0213c8c4;
    ((TextureTransformer*)(this_ + 0x51f4))->~TextureTransformer();
    func_ov006_020e80d8((void*)(this_ + 0x4fd8));
    ((Model*)(this_ + 0x4f88))->~Model();
    ((Model*)(this_ + 0x4f38))->~Model();
    *(void**)this_ = &_ZTV19dScMgSingle3DBase_c;
    ((Particle::SysTracker*)(this_ + 0x471c))->~SysTracker();
    _ZN11dScMgBase_cD2Ev((void*)this_);
    return (void*)this_;
}
