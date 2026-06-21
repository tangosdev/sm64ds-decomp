//cpp
struct Heap;
struct Actor;
extern void* data_ov002_0210ac00;
extern void _ZN5ModelD1Ev();
extern Heap* data_020a0eac;
extern "C" {

void func_0207328c(void* p, int a, int b, void* fn);
void _ZN5ActorD1Ev(Actor* self);
void _ZN6Memory10DeallocateEPvP4Heap(void* p, Heap* h);

void* func_ov002_020ebfcc(Actor* self)
{
    char* p = (char*)self;
    *(void**)p = (void*)&data_ov002_0210ac00;
    func_0207328c(p + 0xd4, 5, 0x50, (void*)_ZN5ModelD1Ev);
    _ZN5ActorD1Ev(self);
    _ZN6Memory10DeallocateEPvP4Heap(self, data_020a0eac);
    return self;
}

}
