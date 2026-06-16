//cpp
extern "C" {
struct M4 { int w[12]; };
struct MMC { char p[0x124]; };
struct Obj { char p[0x2ec]; M4 m; };
int _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(MMC*, M4&, short);
void func_ov036_0211224c(char* self){
    Obj* o = (Obj*)self;
    o->m = *(M4*)(self + 0x46c);
    *(int*)(self+0x310) = *(int*)(self+0x5c);
    *(int*)(self+0x314) = *(int*)(self+0x60) + *(int*)(self+0x4bc);
    *(int*)(self+0x318) = *(int*)(self+0x64);
    _ZN18MovingMeshCollider9TransformERK9Matrix4x3s((MMC*)(self+0x124), o->m, *(short*)(self+0x8e));
}
}
