//cpp
struct ModelComponents;
struct TextureSequence { void Update(ModelComponents&); };
struct MaterialChanger { void Update(ModelComponents&); };
struct TextureTransformer { void Update(ModelComponents&); };
struct Vtbl;
struct Obj {
    void* vt;
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(void* arg);  /* slot 5 = 0x14 */
};
extern "C" int func_ov060_02118e1c(char *c);
int func_ov060_02118e1c(char *c)
{
    ((TextureSequence*)(c+0x138))->Update(*(ModelComponents*)(c+0xdc));
    ((TextureSequence*)(c+0x1d8))->Update(*(ModelComponents*)(c+0x17c));
    ((MaterialChanger*)(c+0x14c))->Update(*(ModelComponents*)(c+0xdc));
    ((MaterialChanger*)(c+0x1ec))->Update(*(ModelComponents*)(c+0x17c));
    ((TextureTransformer*)(c+0x160))->Update(*(ModelComponents*)(c+0xdc));
    ((TextureTransformer*)(c+0x200))->Update(*(ModelComponents*)(c+0x17c));
    ((Obj*)(c+0xd4))->m((void*)0);
    int v[3];
    v[0] = 0x800; v[1] = 0x800; v[2] = 0x800;
    ((Obj*)(c+0x174))->m(v);
    return 1;
}
