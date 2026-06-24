struct Vec3 { int x, y, z; };
extern void Vec3_Sub(struct Vec3* out, struct Vec3* a, struct Vec3* b);
extern void Vec3_Asr(struct Vec3* d, struct Vec3* s, int sh);
extern void func_ov075_0211af84(char* self, int a, int b);

void func_ov075_0211ab38(char* c, struct Vec3* arg2)
{
    struct Vec3 a;
    struct Vec3 r;
    struct Vec3 out;
    a.x = 0;
    a.y = 0x15e000;
    a.z = -0x320000;
    Vec3_Sub(&out, &a, arg2);
    Vec3_Asr(&r, &out, 3);
    r.x = r.x + 0xfa000;
    r.y = r.y + 0xfa000;
    func_ov075_0211af84(c, r.x, r.y);
}
