//cpp
struct CylinderClsn;
struct Actor { void UpdatePos(CylinderClsn *c); };
extern "C" short Vec3_HorzAngle(const void *a, const void *b);
extern "C" void _Z14ApproachLinearRsss(short *dst, short target, short rate);
extern "C" void func_ov018_02111bf0(char *c, void *w);
extern "C" void func_ov018_02111a48(char *c, void *p);
extern "C" void func_0201267c(int a, void *b);
extern "C" int Vec3_Dist(const void *a, const void *b);
extern "C" void func_ov018_021123d0(char *c);

extern "C" int func_ov018_02111e28(Actor *self)
{
    char *s = (char*)self;
    _Z14ApproachLinearRsss((short*)(s + 0x8e),
        Vec3_HorzAngle(s + 0x5c, *(char**)(s + 0x374) + 0x5c), 0x514);
    *(short*)(s + 0x94) = *(short*)(s + 0x8e);
    self->UpdatePos((CylinderClsn*)(s + 0x174));
    func_ov018_02111bf0(s, s + 0x1a8);
    func_ov018_02111a48(s, *(char**)(s + 0x374));
    unsigned int v = (unsigned int)(*(int*)(s + 0x12c) << 4) >> 0x10;
    if (v == 9 || v == 0x15)
        func_0201267c(0xde, s + 0x74);
    char *p2 = *(char**)(s + 0x374);
    int r;
    if (*(int*)(p2 + 8) == 3)
        r = (*(int*)(p2 + 0x360) != 0);
    else
        r = (*(int*)(p2 + 0x358) != 0);
    if (r == 0 || Vec3_Dist(s + 0x364, p2 + 0x5c) > 0x5dc000) {
        *(int*)(s + 0x374) = 0;
        func_ov018_021123d0(s);
    }
    return 1;
}
