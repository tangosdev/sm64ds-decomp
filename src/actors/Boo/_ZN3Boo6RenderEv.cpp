//cpp
// @symbol _ZN3Boo6RenderEv
/* recovered: named members + shared header, real C++ method.
 * unk_080/unk_0b0/unk_10c are raw-offset reads into the inherited dCapEnemy_c
 * (really dEnemyBase_c/dActor_c) span, 0x180 bytes this class no longer
 * declares fields for now that Boo.h derives dCapEnemy_c -- reached the same way
 * dBgActor_c.h documents for other classes' generic padding: `this + offset`
 * rather than a named member. The local ModelVT shadow (renamed from the
 * generated header's placeholder `Model`, which collided with the real
 * include/Model.h class this file also pulls in via Boo.h) simulates only the
 * vtable slot this call needs, not the model's full layout. */
#include "Boo.h"
struct Vector3;
struct ModelVT {
    virtual void f0(); virtual void f1(); virtual void f2();
    virtual void f3(); virtual void f4();
    virtual void Render(const Vector3 *);
};
struct Flags { unsigned short b0:1, b1:1, b2:1, b3:1; };

extern "C" {
void _ZN11dCapEnemy_c14RenderCapModelEPK7Vector3(void *thiz, const void *v);
void _ZN5Model12HideMaterialEii(void *thiz, int a, int b);
void _ZN5Model6RenderEPK7Vector3(void *thiz, const void *v);
}

int Boo::Render()
{
    int b = (int)(((*(int *)((char *)this + 0xb0)) & 0x40000) != 0);
    if (b != 0)
        return 1;

    {
        Flags *f = (Flags *)((char *)this + 0x5d4);
        if (!f->b3)
            return 1;
        if (f->b1) {
            ((ModelVT *)((char *)&mModel))->Render((const Vector3 *)((char *)&unk_510));
        }
        _ZN11dCapEnemy_c14RenderCapModelEPK7Vector3(((char *)this), 0);
    }

    if (unk_5c8 < 8)
        return 1;

    {
        unsigned char st = unk_5cf;
        if (st >= 0xc && st != 0xf)
            _ZN5Model12HideMaterialEii(((char *)this) + 0x380, 0, 2);
    }

    if ((*(int *)((char *)this + 0x10c)) != 8 &&
        (unk_5cc == 3 ||
         unk_5cc == 3 ||
         unk_5cc == 3 ||
         unk_5cc == 3)) {
        _ZN5Model6RenderEPK7Vector3(((char *)this) + 0x380, ((char *)this) + 0x80);
    } else {
        ((ModelVT *)((char *)&mModelAnim))->Render((const Vector3 *)((char *)this + 0x80));
    }

    return 1;
}
