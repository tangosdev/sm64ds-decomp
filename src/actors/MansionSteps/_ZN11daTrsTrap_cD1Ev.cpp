//cpp
// @symbol _ZN11daTrsTrap_cD1Ev

struct Heap;
extern Heap* data_020a0eac;
namespace Memory { void Deallocate(void* ptr, Heap* heap); }

struct dActor_c {
    char pad[0xd0];
    static void operator delete(void* ptr)
    {
        Memory::Deallocate(ptr, data_020a0eac);
    }
    virtual ~dActor_c();
};

struct Model { char pad[0x88]; ~Model(); };
struct dBgW_KcMbg { char pad[0x4]; ~dBgW_KcMbg(); };

struct daTrsTrap_c : dActor_c {
    Model model;                /* 0x0d4 */
    dBgW_KcMbg movingCollider;  /* 0x15c */
    virtual ~daTrsTrap_c();
};

daTrsTrap_c::~daTrsTrap_c()
{
}
