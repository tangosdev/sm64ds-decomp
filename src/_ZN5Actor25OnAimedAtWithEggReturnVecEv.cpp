//cpp
// Actor::OnAimedAtWithEggReturnVec(): returns (sret) the actor position
// (+0x5c..+0x64) with the vtable+0x74 virtual's result added to y.
typedef int s32;

struct Vec { s32 x, y, z; };

struct Base {
    virtual int v0(); virtual int v1(); virtual int v2(); virtual int v3();
    virtual int v4(); virtual int v5(); virtual int v6(); virtual int v7();
    virtual int v8(); virtual int v9(); virtual int v10(); virtual int v11();
    virtual int v12(); virtual int v13(); virtual int v14(); virtual int v15();
    virtual int v16(); virtual int v17(); virtual int v18(); virtual int v19();
    virtual int v20(); virtual int v21(); virtual int v22(); virtual int v23();
    virtual int v24(); virtual int v25(); virtual int v26(); virtual int v27();
    virtual int v28(); virtual int m74();
};

extern "C" void _ZN5Actor25OnAimedAtWithEggReturnVecEv(Vec *ret, Base *self)
{
    char *a = (char *)self;
    ret->x = *(s32 *)(a + 0x5c);
    ret->y = *(s32 *)(a + 0x60);
    ret->z = *(s32 *)(a + 0x64);
    *(s32 *)(((long long)(int)((char *)ret + 4)) & 0xFFFFFFFFFFFFFFFFLL) += self->m74();
}
