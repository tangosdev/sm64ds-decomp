//cpp
struct CylinderClsn { void Clear(); void Update(); };
struct Actor {
    short HorzAngleToCPlayer();
    void UpdatePos(CylinderClsn *c);
};
extern "C" void _Z14ApproachLinearRsss(short *dst, short target, short rate);
extern "C" unsigned char DecIfAbove0_Byte(unsigned char *p);
extern "C" void func_ov030_021141a8(char *c, int x);
struct Animation { void Advance(); };
extern "C" void func_ov030_02111dd0(char *c);
extern "C" void func_ov030_02111f6c(char *c, void *w);
extern "C" void func_ov030_02111bc4(char *c);
extern "C" void func_ov030_02111ea4(char *c);
extern "C" void func_ov030_02111890(char *c);

extern "C" int func_ov030_021122b0(Actor *self)
{
    char *s = (char*)self;
    short ang = self->HorzAngleToCPlayer() + 0x8000;
    _Z14ApproachLinearRsss((short*)(s + 0x8e), ang, 0xa28);
    *(short*)(s + 0x94) = *(short*)(s + 0x8e);
    if (DecIfAbove0_Byte((unsigned char*)(s + 0x3c6)) == 0)
        func_ov030_021141a8(s, 0);
    ((Animation*)(s + 0x124))->Advance();
    int b = (int)(*(unsigned short*)(s + 0xc) == 0x10b);
    if (b) {
        self->UpdatePos((CylinderClsn*)(s + 0x160));
        func_ov030_02111dd0(s);
        func_ov030_02111f6c(s, s + 0x194);
        func_ov030_02111bc4(s);
    } else {
        self->UpdatePos((CylinderClsn*)(s + 0x160));
        func_ov030_02111f6c(s, s + 0x194);
        func_ov030_02111bc4(s);
        func_ov030_02111ea4(s);
    }
    ((CylinderClsn*)(s + 0x160))->Clear();
    ((CylinderClsn*)(s + 0x160))->Update();
    func_ov030_02111890(s);
    return 1;
}
