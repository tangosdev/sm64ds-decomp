//cpp
struct ModelComponents;
struct TextureSequence { void Update(ModelComponents &m); };
struct VObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int x);
};

extern "C" int func_ov077_02124828(char *c)
{
    int b = (int)((*(int *)(c + 0xb0) & 0x40000) != 0);
    if (b != 0)
        return 1;
    ((TextureSequence *)(c + 0x1b0))->Update(*(ModelComponents *)(c + 0xdc));
    ((VObj *)(c + 0xd4))->m(0);
    if (*(int *)(c + 0x3f4) == 1) {
        unsigned int v = ((unsigned int)(*(int *)(c + 0x12c) << 4)) >> 0x10;
        if (v >= 0x19 && v <= 0x3a)
            ((VObj *)(c + 0x138))->m(0);
    }
    return 1;
}
