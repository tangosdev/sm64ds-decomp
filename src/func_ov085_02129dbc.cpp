//cpp
struct Actor { Actor *ClosestPlayer(); };

extern "C" int Vec3_HorzDist(const void *a, const void *b);
extern "C" short Vec3_HorzAngle(const void *a, const void *b);
extern "C" short Vec3_VertAngle(const void *a, const void *b);
extern "C" int AngleDiff(int a, int b);
extern "C" void _Z14ApproachLinearRsss(short *dst, short target, short rate);

struct V3 { int x, y, z; };

extern "C" void func_ov085_02129dbc(Actor *self)
{
    char *s = (char *)self;
    Actor *p = self->ClosestPlayer();
    if (p == 0)
        return;
    V3 v;
    V3 *psrc = (V3 *)(((long long)(int)((char *)p + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
    v = *psrc;
    int hd = Vec3_HorzDist(s + 0x5c, &v);
    v.y = v.y - 0x1e000;
    short ha = Vec3_HorzAngle(s + 0x5c, &v);
    short va = Vec3_VertAngle(s + 0x5c, &v);
    if (hd < 0x15e000 && AngleDiff(ha, *(short *)(s + 0x8e)) < 0x3000) {
        *(short *)(s + 0x364) = va;
        *(short *)(s + 0x366) = *(short *)(s + 0x8e) - ha;
    } else {
        *(short *)(s + 0x364) = 0;
        *(short *)(s + 0x366) = 0;
    }
    _Z14ApproachLinearRsss((short *)(s + 0x362), *(short *)(s + 0x366), 0x250);
    _Z14ApproachLinearRsss((short *)(s + 0x360), *(short *)(s + 0x364), 0x100);
}
