//cpp
// @symbol _ZN3Amp6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Amp.h"
struct Obj { virtual void m0(); virtual void m1(); virtual void m2(); virtual void m3(); virtual void m4(); virtual int m5(int); };
struct Obj2 { virtual void n0(); virtual void n1(); virtual void n2(); virtual void n3(); virtual void n4(); virtual int n5(void*); };
extern "C" {
extern int _ZN15TextureSequence6UpdateER15ModelComponents(void* a, void* b);
extern int _ZN18TextureTransformer6UpdateER15ModelComponents(void* a, void* b);
}

int Amp::Render()
{
    ((Obj*)((char*)&mModelAnim))->m5(0);
    int s = unk_420;
    if(s != 0 && s != 2){
        _ZN15TextureSequence6UpdateER15ModelComponents(((char*)this)+0x188, ((char*)this)+0x140);
        _ZN18TextureTransformer6UpdateER15ModelComponents(((char*)this)+0x19c, ((char*)this)+0x140);
        ((Obj2*)((char*)&mModel))->n5((char*)&unk_080);
    }
    return 1;
}
