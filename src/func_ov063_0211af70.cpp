//cpp
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

int func_ov063_0211af70(char *c)
{
    int b = (int)((*(int *)(c + 0xb0) & 0x40000) != 0);
    if (b != 0)
        return 1;

    {
        Flags *f = (Flags *)(c + 0x5d4);
        if (!f->b3)
            return 1;
        if (f->b1) {
            ((Model *)(c + 0x3e4))->Render((const Vector3 *)(c + 0x510));
        }
        _ZN8CapEnemy14RenderCapModelEPK7Vector3(c, 0);
    }

    if (*(unsigned char *)(c + 0x5c8) < 8)
        return 1;

    {
        unsigned char st = *(unsigned char *)(c + 0x5cf);
        if (st >= 0xc && st != 0xf)
            _ZN5Model12HideMaterialEii(c + 0x380, 0, 2);
    }

    if (*(int *)(c + 0x10c) != 8 &&
        (*(unsigned char *)(c + 0x5cc) == 3 ||
         *(unsigned char *)(c + 0x5cc) == 3 ||
         *(unsigned char *)(c + 0x5cc) == 3 ||
         *(unsigned char *)(c + 0x5cc) == 3)) {
        _ZN5Model6RenderEPK7Vector3(c + 0x380, c + 0x80);
    } else {
        ((Model *)(c + 0x380))->Render((const Vector3 *)(c + 0x80));
    }

    return 1;
}
}
