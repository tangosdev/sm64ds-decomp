//cpp
typedef unsigned int u32;

struct Heap;
struct ActorBase;

extern "C" {
    Heap* _ZN4Heap28InitializeSolidHeapAsDefaultEjPS_i(u32 size, Heap* heap, int flag);
    void* _ZN6Memory8AllocateEjiP4Heap(u32 size, int align, Heap* heap);
    void  _ZN4Heap20RestoreFromTemporaryEv(void);
    void  _ZN4Heap8_DestroyEv(Heap* h);
    u32   _ZN4Heap21MaxAllocationUnitSizeEv(Heap* h);
    void  _ZN4Heap11ResizeToFitEv(Heap* h);
    void  _ZN9ActorBase18MarkForDestructionEv(ActorBase* self);
}

struct ActorBase {
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14();
    virtual int  v15();

    int Virtual34(u32 a, u32 b);
};

int ActorBase::Virtual34(u32 a, u32 b)
{
    Heap* h = 0;
    u32 avail;

    if (*(Heap**)((char*)this + 0x4c) != 0)
        return 1;

    if (a != 0) {
        h = _ZN4Heap28InitializeSolidHeapAsDefaultEjPS_i(a, (Heap*)b, 0x20);
        if (h != 0) {
            u32 flagA = (*(u32*)((char*)h + 4)) & 0x10;
            if (flagA != 0)
                _ZN6Memory8AllocateEjiP4Heap(0x10, 4, 0);
            int res = this->v15();
            u32 okA;
            if (flagA != 0) {
                okA = (u32)res;
            } else {
                void* allocRes = _ZN6Memory8AllocateEjiP4Heap(0x10, 4, 0);
                okA = (allocRes != 0) ? (u32)res : 0;
            }
            _ZN4Heap20RestoreFromTemporaryEv();
            if (okA == 0) {
                _ZN4Heap8_DestroyEv(h);
                h = 0;
            } else {
                u32 topA = *(u32*)((char*)h + 8);
                avail = topA - _ZN4Heap21MaxAllocationUnitSizeEv(h);
                avail = (avail + 0x1f) & ~0x1f;
                if (a == avail) {
                    _ZN4Heap11ResizeToFitEv(h);
                    *(Heap**)((char*)this + 0x4c) = h;
                    return 1;
                }
            }
        }
    }

    if (h == 0) {
        h = _ZN4Heap28InitializeSolidHeapAsDefaultEjPS_i((u32)-1, (Heap*)b, 0x20);
        u32 flagB = (*(u32*)((char*)h + 4)) & 0x10;
        if (flagB != 0)
            _ZN6Memory8AllocateEjiP4Heap(0x10, 4, 0);
        a = (u32)this->v15();
        if (flagB == 0) {
            void* allocRes2 = _ZN6Memory8AllocateEjiP4Heap(0x10, 4, 0);
            if (allocRes2 == 0)
                a = 0;
        }
        _ZN4Heap20RestoreFromTemporaryEv();
        if (a == 0) {
            _ZN4Heap8_DestroyEv(h);
            _ZN9ActorBase18MarkForDestructionEv(this);
            return 0;
        }
        u32 topB = *(u32*)((char*)h + 8);
        avail = topB - _ZN4Heap21MaxAllocationUnitSizeEv(h);
        avail = (avail + 0x1f) & ~0x1f;
    }

    if (h == 0)
        goto fail;

    {
        u32 topH = *(u32*)((char*)h + 8);
        Heap* h2 = 0;
        u32 availInH = topH - _ZN4Heap21MaxAllocationUnitSizeEv(h);
        u32 needed = ((availInH + 0xf) & ~0xf) + 0x30;
        if (needed < _ZN4Heap21MaxAllocationUnitSizeEv((Heap*)b)) {
            h2 = _ZN4Heap28InitializeSolidHeapAsDefaultEjPS_i(avail, (Heap*)b, 0x20);
        }
        if (h2 != 0) {
            if ((u32)h2 < (u32)h) {
                _ZN4Heap8_DestroyEv(h);
                h = 0;
                int res3 = this->v15();
                u32 okC = (u32)res3;
                _ZN4Heap20RestoreFromTemporaryEv();
                if (okC == 0) {
                    _ZN4Heap8_DestroyEv(h2);
                    h2 = h;
                }
            } else {
                _ZN4Heap20RestoreFromTemporaryEv();
                _ZN4Heap8_DestroyEv(h2);
                h2 = 0;
            }
        }
        if (h2 != 0) {
            _ZN4Heap11ResizeToFitEv(h2);
            *(Heap**)((char*)this + 0x4c) = h2;
            return 1;
        }
    }

    if (h != 0) {
        _ZN4Heap11ResizeToFitEv(h);
        *(Heap**)((char*)this + 0x4c) = h;
        return 1;
    }

fail:
    _ZN9ActorBase18MarkForDestructionEv(this);
    return 0;
}
