//cpp
// @symbol _ZN3Boo6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Boo.h"
struct Vector3;
struct Model {
    virtual void f0(); virtual void f1(); virtual void f2();
    virtual void f3(); virtual void f4();
    virtual void Render(const Vector3 *);
};
struct Flags { unsigned short b0:1, b1:1, b2:1, b3:1; };

extern "C" {
void _ZN8CapEnemy14RenderCapModelEPK7Vector3(void *thiz, const void *v);
void _ZN5Model12HideMaterialEii(void *thiz, int a, int b);
void _ZN5Model6RenderEPK7Vector3(void *thiz, const void *v);
}

int Boo::Render()
{
    int b = (int)((unk_0b0 & 0x40000) != 0);
    if (b != 0)
        return 1;

    {
        Flags *f = (Flags *)((char *)&unk_5d4);
        if (!f->b3)
            return 1;
        if (f->b1) {
            ((Model *)((char *)&mModel))->Render((const Vector3 *)((char *)&unk_510));
        }
        _ZN8CapEnemy14RenderCapModelEPK7Vector3(((char *)this), 0);
    }

    if (unk_5c8 < 8)
        return 1;

    {
        unsigned char st = unk_5cf;
        if (st >= 0xc && st != 0xf)
            _ZN5Model12HideMaterialEii(((char *)this) + 0x380, 0, 2);
    }

    if (unk_10c != 8 &&
        (unk_5cc == 3 ||
         unk_5cc == 3 ||
         unk_5cc == 3 ||
         unk_5cc == 3)) {
        _ZN5Model6RenderEPK7Vector3(((char *)this) + 0x380, ((char *)this) + 0x80);
    } else {
        ((Model *)((char *)&mModelAnim))->Render((const Vector3 *)((char *)&unk_080));
    }

    return 1;
}
