//cpp
#include "types.h"
struct Heap {
    char pad0[4];
    int field4;     /* 0x4 */
    int MaxAllocationUnitSize();
    static void RestoreFromTemporary();
    void _Destroy();
    static Heap *InitializeSolidHeapAsDefault(u32, Heap *, int);
};

struct Memory {
    static void *Allocate(u32, int, Heap *);
};

struct fBase_c {
    virtual void v0();   virtual void v1();   virtual void v2();   virtual void v3();
    virtual void v4();   virtual void v5();   virtual void v6();   virtual void v7();
    virtual void v8();   virtual void v9();   virtual void v10();  virtual void v11();
    virtual void v12();  virtual void v13();  virtual void v14();
    virtual int v15();   /* vtable offset 0x3c */
    void MarkForDestruction();
    int Virtual38(u32 a, u32 b);

    char pad[0x48];
    Heap *heap_4c;   /* 0x4c */
};

int fBase_c::Virtual38(u32 a, u32 b) {
    if (this->heap_4c != 0) return 1;
    if (a != 0) {
        Heap *h = Heap::InitializeSolidHeapAsDefault(a, (Heap *)b, 0x20);
        if (h != 0) {
            int ok;
            int flag = h->field4 & 0x10;
            if (flag != 0) Memory::Allocate(0x10, 4, 0);
            ok = this->v15();
            if (flag == 0) {
                if (Memory::Allocate(0x10, 4, 0) == 0) ok = 0;
            }
            h->MaxAllocationUnitSize();
            Heap::RestoreFromTemporary();
            if (ok == 0) {
                h->_Destroy();
            } else {
                this->heap_4c = h;
                return 1;
            }
        }
    }
    this->MarkForDestruction();
    return 0;
}
