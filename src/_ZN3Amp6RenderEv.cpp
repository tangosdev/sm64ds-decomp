//cpp
struct Obj { virtual void m0(); virtual void m1(); virtual void m2(); virtual void m3(); virtual void m4(); virtual int m5(int); };
struct Obj2 { virtual void n0(); virtual void n1(); virtual void n2(); virtual void n3(); virtual void n4(); virtual int n5(void*); };
extern "C" {
extern int _ZN15TextureSequence6UpdateER15ModelComponents(void* a, void* b);
extern int _ZN18TextureTransformer6UpdateER15ModelComponents(void* a, void* b);
int _ZN3Amp6RenderEv(char* c){
    ((Obj*)(c+0xd4))->m5(0);
    int s = *(int*)(c+0x420);
    if(s != 0 && s != 2){
        _ZN15TextureSequence6UpdateER15ModelComponents(c+0x188, c+0x140);
        _ZN18TextureTransformer6UpdateER15ModelComponents(c+0x19c, c+0x140);
        ((Obj2*)(c+0x138))->n5(c+0x80);
    }
    return 1;
}
}
