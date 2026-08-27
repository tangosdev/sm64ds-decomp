//cpp
struct Animation { void Advance(); };
struct dCc_c { void Clear(); void Update(); };
struct dActor_c { void UpdatePos(dCc_c *cc); };

extern "C" void func_0201267c(int a, void *p);
extern "C" void func_ov071_0211f0b4(char *c);
extern "C" void func_ov071_0211f148(char *c, void *p);
extern "C" void func_ov071_0211f29c(char *c);

extern "C" int func_ov071_0211fee4(char *c)
{
    *(short *)(c + 0x8e) = (short)(*(short *)(c + 0x8e) + 0x2bc);
    *(short *)(c + 0x94) = *(short *)(c + 0x8e);
    ((Animation *)(c + 0x124))->Advance();
    unsigned short f = (unsigned short)(*(int *)(c + 0x12c) >> 12);
    if (f == 0 || f == 8 || f == 0x17 || f == 0x1f) {
        func_0201267c(0xf0, (void *)(c + 0x74));
    }
    func_ov071_0211f0b4(c);
    ((dActor_c *)c)->UpdatePos((dCc_c *)(c + 0x160));
    func_ov071_0211f148(c, (void *)(c + 0x194));
    func_ov071_0211f29c(c);
    ((dCc_c *)(c + 0x160))->Clear();
    ((dCc_c *)(c + 0x160))->Update();
    return 1;
}
