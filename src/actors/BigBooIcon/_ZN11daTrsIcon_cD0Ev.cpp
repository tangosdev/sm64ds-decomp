//cpp
// @symbol _ZN11daTrsIcon_cD0Ev

struct Heap;
extern Heap* data_020a0eac;
namespace Memory { void Deallocate(void* ptr, Heap* heap); }

struct dActor_c {
    static void operator delete(void* ptr)
    {
        Memory::Deallocate(ptr, data_020a0eac);
    }
    virtual ~dActor_c();
};

struct daTrsIcon_c : dActor_c {
    virtual ~daTrsIcon_c();
};

daTrsIcon_c::~daTrsIcon_c()
{
}
