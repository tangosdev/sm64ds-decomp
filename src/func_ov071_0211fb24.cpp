//cpp
extern "C" {
struct Vector3 { int x, y, z; };
extern short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern void _Z14ApproachLinearRsss(short& dst, short a, short b);
extern void _ZN9Animation7AdvanceEv(void* thiz);
extern int Vec3_Dist(const Vector3* a, const Vector3* b);
extern void func_ov071_021202ec(void* c, int n);
extern void func_ov071_0211f0b4(char* c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* thiz, void* cc);
extern void func_ov071_0211f148(void* c, void* p);
extern void func_ov071_0211f29c(void* c);
extern void _ZN12CylinderClsn5ClearEv(void* thiz);
extern void _ZN12CylinderClsn6UpdateEv(void* thiz);
extern void func_0201267c(int a, void* v);

int func_ov071_0211fb24(char* c)
{
    short ang = Vec3_HorzAngle((Vector3*)(c + 0x5c), (Vector3*)(c + 0x390));
    _Z14ApproachLinearRsss(*(short*)(c + 0x8e), ang, 0x2bc);
    *(short*)(c + 0x94) = *(short*)(c + 0x8e);
    _ZN9Animation7AdvanceEv(c + 0x124);
    if (Vec3_Dist((Vector3*)(c + 0x5c), (Vector3*)(c + 0x390)) < 0x12c000)
        func_ov071_021202ec(c, 2);
    func_ov071_0211f0b4(c);
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x160);
    func_ov071_0211f148(c, c + 0x194);
    func_ov071_0211f29c(c);
    _ZN12CylinderClsn5ClearEv(c + 0x160);
    _ZN12CylinderClsn6UpdateEv(c + 0x160);
    unsigned short v = (unsigned short)(*(int*)(c + 0x12c) >> 0xc);
    if (v == 0 || v == 8 || v == 0x17 || v == 0x1f)
        func_0201267c(0xf0, c + 0x74);
    return 1;
}
}
