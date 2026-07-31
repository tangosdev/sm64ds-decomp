//cpp
// @symbol func_ov006_020e9d1c
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMg3DEsp_c.h"
// @emits dScMg3DEsp_c_Render
/* recovered: renamed to Class_Method */
/* dScMg3DEsp_c::Render - recovered from vtable slot identity */
extern "C" void func_ov006_020c0134(char *c);

struct ModelComponents;
struct TextureTransformer { void Update(ModelComponents &m); };

struct Obj { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void vcall(int); };

extern "C" int dScMg3DEsp_c_Render(char *c)
{
    struct dScMg3DEsp_c *self = (struct dScMg3DEsp_c *)(void *)c;
    func_ov006_020e81a4(c);
    func_ov006_020e8e10(c);
    func_ov006_020e8b18(c);
    func_ov006_020e8354(c);
    self->unk_4700 = 0;
    self->unk_4704 = 0xd0000;
    self->unk_4708 = 0x40000;
    self->unk_470c = 0xffed3000;
    self->unk_4710 = 0xe0000;
    self->unk_4714 = 0x40000;
    self->unk_4718 = 0xc00;
    func_ov006_020c0134(c + 0x4660);
    ((TextureTransformer *)(c + 0x51f4))->Update(*(ModelComponents *)(c + 0x4f40));
    ((Obj *)(c + 0x4f38))->vcall(0);
    func_ov006_020e7b44(c + 0x4fd8);
    ((Obj *)(c + 0x4f88))->vcall(0);
    return 1;
}
