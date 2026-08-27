//cpp
extern "C" {
extern unsigned int _ZNK6Player14GetBodyModelIDEjb(void* self, unsigned int idx, int flag);
extern void func_020167a4(void* p);
extern void func_ov002_020e640c(void* self);
extern void _ZN15ModelComponents21UpdateVertsUsingBonesEv(void* self);
extern int func_ov002_020becf4(void* self, unsigned int idx, int flag);
extern void MulMat4x3Mat4x3(void* dst, void* a, void* b);
extern int data_020a0e68[12];
}

struct VTable4 {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void DoThing(int arg);
};

typedef struct { int w[12]; } Mat43;

extern "C" void func_ov002_020e4768(char* self)
{
    unsigned int mid;
    void* comp;
    int idx;

    mid = _ZNK6Player14GetBodyModelIDEjb(self, *(unsigned int*)(self+8) & 0xff, 0);
    comp = *(void**)(self + mid*4 + 0xdc);
    func_020167a4(comp);

    func_ov002_020e640c(self);

    mid = _ZNK6Player14GetBodyModelIDEjb(self, *(unsigned int*)(self+8) & 0xff, 0);
    comp = *(void**)(self + mid*4 + 0xdc);
    _ZN15ModelComponents21UpdateVertsUsingBonesEv((char*)comp + 8);

    idx = func_ov002_020becf4(self, *(unsigned int*)(self+8) & 0xff, 0);

    if (idx == 3) {
        unsigned int m2 = _ZNK6Player14GetBodyModelIDEjb(self, *(unsigned int*)(self+8) & 0xff, 0);
        int a1 = *(int*)((char*)(*(void**)(self + m2*4 + 0xdc)) + 0x14) + 0x2d0;
        a1 = idx ? a1 : a1;
        ((VTable4**)(self + 0x154))[idx]->DoThing(a1);
    }

    {
        int hasFlag = (*(int*)(self + 0x358) != 0);
        if (hasFlag) {
            mid = _ZNK6Player14GetBodyModelIDEjb(self, *(unsigned int*)(self+8) & 0xff, 0);
            comp = *(void**)(self + mid*4 + 0xdc);
            *(Mat43*)data_020a0e68 = *(Mat43*)((char*)comp + 0x1c);

            mid = _ZNK6Player14GetBodyModelIDEjb(self, *(unsigned int*)(self+8) & 0xff, 0);
            comp = *(void**)(self + mid*4 + 0xdc);
            MulMat4x3Mat4x3((char*)*(int*)((char*)comp + 0x14) + 0x2a0, data_020a0e68, data_020a0e68);

            *(Mat43*)(self + 0x58c) = *(Mat43*)data_020a0e68;
            *(void**)(self + 0xc8) = self + 0x58c;
        }
    }

    mid = _ZNK6Player14GetBodyModelIDEjb(self, *(unsigned int*)(self+8) & 0xff, 0);
    comp = *(void**)(self + mid*4 + 0xdc);
    *(Mat43*)data_020a0e68 = *(Mat43*)((char*)comp + 0x1c);

    mid = _ZNK6Player14GetBodyModelIDEjb(self, *(unsigned int*)(self+8) & 0xff, 0);
    comp = *(void**)(self + mid*4 + 0xdc);
    MulMat4x3Mat4x3((char*)*(int*)((char*)comp + 0x14) + 0x2d0, data_020a0e68, data_020a0e68);

    *(Mat43*)(self + 0x5ec) = *(Mat43*)data_020a0e68;
}
